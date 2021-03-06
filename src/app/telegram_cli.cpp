#include <app/telegram_cli.hpp>
#include <app/tele_app.hpp>
#include <chrono>
#include <string>
#include <util.hpp>
#include <gui/utility.hpp>
 

/** Use openChat/closeChat class for read acknowledgement
    * https://github.com/tdlib/td/issues/46
    * https://core.telegram.org/tdlib/getting-started#tdlib-interface
*/
namespace app
{
namespace td_api = td::td_api;

namespace detail
{
template <class... Fs>
struct overload;

template <class F>
struct overload<F> : public F
{
    explicit overload(F f) : F(f)
    {
    }
};
template <class F, class... Fs>
struct overload<F, Fs...>
    : public overload<F>, overload<Fs...>
{
    overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...)
    {
    }
    using overload<F>::operator();
    using overload<Fs...>::operator();
};
} // namespace detail

template <class... F>
auto overloaded(F... f)
{
    return detail::overload<F...>(f...);
}

//Constructor
telegram_cli::telegram_cli(tele_app &app)
    : m_app(app)
{
    td::Client::execute({0, td_api::make_object<td_api::setLogVerbosityLevel>(1)});
}

// Start telegram thread
void telegram_cli::start()
{
    m_thread_running = true;
    m_thread = std::thread(&telegram_cli::client_thread, this);
}

//Stop the thread
void telegram_cli::stop()
{
    m_thread_running = false;
}

//Wait for thread terminate
void telegram_cli::wait_for_terminate()
{
    m_thread_running = false;
    m_thread.join();
}

//Telegram main client thread loop
void telegram_cli::client_thread()
{
    m_client = std::make_unique<td::Client>();
    while (m_thread_running)
    {
        if (m_need_restart)
        {
            restart();
        }
        else if (!m_are_authorized)
        {
            process_response(m_client->receive(1));
        }
        else
        {
            auto response = m_client->receive(1);
            if (response.object)
            {
                process_response(std::move(response));
            }
        }
        update_is_typing_status();
    }
}

//Restart client
void telegram_cli::restart()
{
    m_need_restart = false;
    m_are_authorized = false;
    m_handlers.clear();
    m_users.clear();
    m_chat_title.clear();
    m_authorization_state.reset();
    m_current_query_id = 0;
    m_authentication_query_id = 0;
    m_client = std::make_unique<td::Client>();
}

//Process response
void telegram_cli::process_response(td::Client::Response response)
{
    if (!response.object)
    {
        return;
    }
    if (response.id == 0)
    {
        return process_update(std::move(response.object));
    }
    auto it = m_handlers.find(response.id);
    if (it != m_handlers.end())
    {
        it->second(std::move(response.object));
    }
}

//Process update
void telegram_cli::process_update(td_api::object_ptr<td_api::Object> update)
{
    td_api::downcast_call(
        *update, overloaded(
                     [this](td_api::updateAuthorizationState &update_authorization_state) {
                        m_authorization_state = std::move(update_authorization_state.authorization_state_);
                        on_authorization_state_update();
                     },
                     [this](td_api::updateNewChat &update_new_chat) {
                        m_chat_title[update_new_chat.chat_->id_] = update_new_chat.chat_->title_;
                        m_chat_typing[update_new_chat.chat_->id_] = std::make_pair(std::chrono::steady_clock::now(),false);
                     },
                     [this](td_api::updateChatTitle &update_chat_title) {
                         m_chat_title[update_chat_title.chat_id_] = update_chat_title.title_;
                     },
                     [this](td_api::updateUser &update_user) {
                         auto user_id = update_user.user_->id_;
                         m_users[user_id] = std::move(update_user.user_);
                     },
                     [this](td_api::updateNewMessage &update_new_message) {
                         auto chat_id = update_new_message.message_->chat_id_;
                         auto msg_id = update_new_message.message_->id_;
                         auto outgoing = update_new_message.message_->is_outgoing_;
                         auto sender_user_name = get_user_name(update_new_message.message_->sender_user_id_);
                         auto msg_date = update_new_message.message_->date_;
                         std::string text;
                         if (update_new_message.message_->content_->get_id() == td_api::messageText::ID)
                         {
                             text = static_cast<td_api::messageText &>(*update_new_message.message_->content_).text_->text_;
                         }
                         else if(update_new_message.message_->content_->get_id() == td_api::messagePhoto::ID) 
                         {
                             text = "Photo received caption:" + static_cast<td_api::messagePhoto &>(*update_new_message.message_->content_).caption_->text_;
                         }
                         m_app.on_new_message(chat_id, msg_id, sender_user_name, text, outgoing, msg_date);
                     },
                     [this](td_api::updateUserChatAction &chat_action) {
                         const auto action_id = chat_action.action_->get_id();
                         const auto user_id = chat_action.user_id_;
                         if(action_id==td_api::chatActionTyping::ID ) {
                            m_action_state[user_id] = action_id; 
                            m_app.on_user_typing(user_id,true);
                         } else if(action_id==td_api::chatActionCancel::ID) {
                            if(m_action_state[user_id]==td_api::chatActionTyping::ID) {
                                m_app.on_user_typing(user_id,false);
                            }
                            m_action_state[user_id] = 0;
                         }
                     },
                     [this](td_api::updateUserStatus& update_user_status) {
                        const auto user_id = update_user_status.user_id_;
                        switch(update_user_status.status_->get_id()) {
                        case td_api::userStatusOnline::ID:
                            m_app.set_online_status(user_id,true);
                            break;
                        case td_api::userStatusEmpty::ID:
                        case td_api::userStatusRecently::ID:
                        case td_api::userStatusOffline::ID:
                        case td_api::userStatusLastWeek::ID:
                        case td_api::userStatusLastMonth::ID:
                            m_app.set_online_status(user_id,false);
                        }
                     },
                     [](auto &update) {})); 
}

// Authorization state update
void telegram_cli::on_authorization_state_update()
{
    m_authentication_query_id++;
    td_api::downcast_call(
        *m_authorization_state,
        overloaded(
            [this](td_api::authorizationStateReady &) {
                m_are_authorized = true;
                m_app.new_control_message("Got authorization");
                m_app.restore_opened_buffers();
            },
            [this](td_api::authorizationStateLoggingOut &) {
                m_are_authorized = false;
                m_app.new_control_message("Logging out");
            },
            [this](td_api::authorizationStateClosing &) {
                m_app.new_control_message("Closing");
            },
            [this](td_api::authorizationStateClosed &) {
                m_are_authorized = false;
                m_need_restart = true;
                m_app.new_control_message("Terminated");
            },
            [this](td_api::authorizationStateWaitCode &) {
                m_app.new_control_message("Send auth code using authcode <code> command");
            },
            [this](td_api::authorizationStateWaitRegistration &) {
                m_app.new_control_message("Enter your name and surname using authname <name> <surname> command");
            },
            [this](td_api::authorizationStateWaitPassword &) {
                m_app.new_control_message("Enter auth password using authpass <password> command");
            },
            [this](td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
                m_app.new_control_message("Confirm this login link on another device: " + state.link_);
            },
            [this](td_api::authorizationStateWaitPhoneNumber &) {
                m_app.new_control_message("Enter your phone number using authphoneno");
            },
            [this](td_api::authorizationStateWaitEncryptionKey &) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1s);
                send_query(td_api::make_object<td_api::checkDatabaseEncryptionKey>("my_key"),
                           create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitTdlibParameters &) {
                auto parameters = td_api::make_object<td_api::tdlibParameters>();
                parameters->database_directory_ = util::home_dir()+ "/.config/telesecure/tdlib";
                parameters->use_message_database_ = true;
                parameters->use_secret_chats_ = true;
                parameters->api_id_ = 94575;
                parameters->api_hash_ = "a3406de8d171bb422bb6ddf3bbd800e2";
                parameters->system_language_code_ = "en";
                parameters->device_model_ = "Desktop";
                parameters->system_version_ = "Linux";
                parameters->application_version_ = "1.0";
                parameters->enable_storage_optimizer_ = true;
                send_query(td_api::make_object<td_api::setTdlibParameters>(std::move(parameters)),
                           create_authentication_query_handler());
            }));
}

// Send telegram query
void telegram_cli::send_query(td_api::object_ptr<td_api::Function> f, std::function<void(Object)> handler)
{
    auto query_id = next_query_id();
    if (handler)
    {
        m_handlers.emplace(query_id, std::move(handler));
    }
    m_client->send({query_id, std::move(f)});
}

//Check authentication error
void telegram_cli::check_authentication_error(Object object)
{
    if (object->get_id() == td_api::error::ID)
    {
        auto error = td::move_tl_object_as<td_api::error>(object);
        auto str = to_string(error);
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        m_app.new_control_message("Error: " + str);
        on_authorization_state_update();
    }
}

std::string telegram_cli::get_user_name(std::int32_t user_id)
{
    auto it = m_users.find(user_id);
    if (it == m_users.end())
    {
        return "unknown user";
    }
    return it->second->first_name_ + " " + it->second->last_name_;
}

// View message read
void telegram_cli::view_message(std::uint64_t id, std::int64_t msgid)
{
    std::vector<std::int64_t> mids;
    mids.push_back(msgid);
    send_query(td_api::make_object<td_api::viewMessages>(id, std::move(mids), true), {});
}

// Get user list
void telegram_cli::get_user_list()
{
    send_query(td_api::make_object<td_api::getContacts>(),
    [this](Object object) {
        if(object->get_id() == td_api::error::ID) return;
        const auto users = td::move_tl_object_as<td_api::users>(object)->user_ids_;
        m_app.new_control_message("Loading user lists...");
        for( auto user : users ) {
            send_query(td_api::make_object<td_api::getUser>(user), 
            [this](Object object) {
                if(object->get_id() == td_api::error::ID) return; 
                const auto userinfo = td::move_tl_object_as<td_api::user>(object);
                m_app.new_control_message("[id:" + std::to_string(userinfo->id_) + 
                    + "] [Name:" + userinfo->first_name_ + "] [Surname:" + userinfo->last_name_ + "]");
            });
        }
    });
}

// Get chat lists
void telegram_cli::get_chat_list()
{
    send_query(td_api::make_object<td_api::getChats>(nullptr, std::numeric_limits<std::int64_t>::max(), 0, 20),
        [this](Object object) {
        if (object->get_id() == td_api::error::ID) {
            return;
        }
        m_app.new_control_message("Loading chat lists...");
        auto chats = td::move_tl_object_as<td_api::chats>(object);
        for (auto chat_id : chats->chat_ids_) {
            m_app.new_control_message("[id:" + std::to_string(chat_id) + "] [title:" +  m_chat_title[chat_id] + "]");
        }
    });
}

// Open chat
void telegram_cli::open_chat(std::uint64_t id, std::function<void()> completed) {
    send_query(td_api::make_object<td_api::openChat>(id), 
        [this,completed](Object object) {
            if (object->get_id() == td_api::error::ID) {
                return;
            }
        completed();
    });
}

//Send message to the selected chat client
void telegram_cli::send_message_to(std::int64_t id, std::string_view msg)
{
    auto send_message = td_api::make_object<td_api::sendMessage>();
    send_message->chat_id_ = id;
    auto message_content = td_api::make_object<td_api::inputMessageText>();
    message_content->text_ = td_api::make_object<td_api::formattedText>();
    message_content->text_->text_ = std::move(msg);
    send_message->input_message_content_ = std::move(message_content);
    send_query(std::move(send_message), {});
}

//Update typing
void telegram_cli::update_typing_chat(std::int64_t chat_id, bool typing)
{
    std::unique_lock _lck(m_typing_lock);
    const auto now = std::chrono::steady_clock::now();
    if(!m_chat_typing[chat_id].second && typing) {
        m_chat_typing[chat_id] = std::make_pair(now,typing);
        if(typing) {
            auto typing_obj = td_api::make_object<td_api::chatActionTyping>();
            auto chat_action_obj = td_api::make_object<td_api::sendChatAction>(chat_id,std::move(typing_obj));
            send_query(std::move(chat_action_obj),{});
        }
    } else {
         m_chat_typing[chat_id] = std::make_pair(now,typing);
    }
}

// Update is typing status
void telegram_cli::update_is_typing_status()
{
    const auto end = std::chrono::steady_clock::now();
    std::unique_lock _lck(m_typing_lock);
    for( auto& ts: m_chat_typing ) {
        using namespace std::chrono_literals;
        const auto diff = end - ts.second.first;
        if(diff>5s && ts.second.second) {
            //Send action event
            auto typing_obj = td_api::make_object<td_api::chatActionCancel>();
            auto chat_action_obj = td_api::make_object<td_api::sendChatAction>(ts.first,std::move(typing_obj));
            send_query(std::move(chat_action_obj),{});
            //Update variable
            ts.second.second = false;
        }
    }
}

} // namespace app
