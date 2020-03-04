#include <app/telegram_cli.hpp>
#include <app/tele_app.hpp>
#include <chrono>

namespace app {
namespace td_api = td::td_api;


namespace detail {
template <class... Fs>
struct overload;

template <class F>
struct overload<F> : public F {
  explicit overload(F f) : F(f) {
  }
};
template <class F, class... Fs>
struct overload<F, Fs...>
    : public overload<F>
    , overload<Fs...> {
  overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {
  }
  using overload<F>::operator();
  using overload<Fs...>::operator();
};
}  // namespace detail

template <class... F>
auto overloaded(F... f) {
  return detail::overload<F...>(f...);
}

//Constructor
telegram_cli::telegram_cli(tele_app& app)
    : m_app(app)
    , m_client(new td::Client())
{

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

 //Telegram main client thread
void telegram_cli::client_thread()
{
    while(m_thread_running)
    {
        if(m_need_restart) {
            restart();
        } else if(!m_are_authorized) {
            process_response(m_client->receive(10));
        } else {
             auto response = m_client->receive(1);
            if (response.object) {
            } else {
                break;
            }

        }
    }
}

  //Restart client
void telegram_cli::restart()
{
    m_client.reset();
    m_need_restart = false;
    m_are_authorized = false;
}

//Process response
void telegram_cli::process_response(td::Client::Response response)
{
    if(!response.object) {
        return;
    }
    if(response.id == 0) {
        return process_update(std::move(response.object)); 
    }
    auto it = m_handlers.find(response.id);
    if (it != m_handlers.end()) {
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
                auto sender_user_name = get_user_name(update_new_message.message_->sender_user_id_);
                std::string text;
                if (update_new_message.message_->content_->get_id() == td_api::messageText::ID)
                {
                    text = static_cast<td_api::messageText &>(*update_new_message.message_->content_).text_->text_;
                }
                std::cout << "Got message: [chat_id:" << chat_id << "] [from:" << sender_user_name << "] ["
                        << text << "]" << std::endl;
            },
            [](auto &update) {}));
 }

void telegram_cli::on_authorization_state_update() 
{
    m_authentication_query_id++;
    td_api::downcast_call(
        *m_authorization_state,
        overloaded(
            [this](td_api::authorizationStateReady &) {
              m_are_authorized = true;
              std::cout << "Got authorization" << std::endl;
            },
            [this](td_api::authorizationStateLoggingOut &) {
              m_are_authorized = false;
              std::cout << "Logging out" << std::endl;
            },
            [this](td_api::authorizationStateClosing &) { std::cout << "Closing" << std::endl; },
            [this](td_api::authorizationStateClosed &) {
              m_are_authorized = false;
              m_need_restart = true;
              std::cout << "Terminated" << std::endl;
            },
            [this](td_api::authorizationStateWaitCode &) {
              std::cout << "Enter authentication code: " << std::flush;
              std::string code;
              std::cin >> code;
              send_query(td_api::make_object<td_api::checkAuthenticationCode>(code),
                         create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitRegistration &) {
              std::string first_name;
              std::string last_name;
              std::cout << "Enter your first name: " << std::flush;
              std::cin >> first_name;
              std::cout << "Enter your last name: " << std::flush;
              std::cin >> last_name;
              send_query(td_api::make_object<td_api::registerUser>(first_name, last_name),
                         create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitPassword &) {
              std::cout << "Enter authentication password: " << std::flush;
              std::string password;
              std::cin >> password;
              send_query(td_api::make_object<td_api::checkAuthenticationPassword>(password),
                         create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
              std::cout << "Confirm this login link on another device: " << state.link_ << std::endl;
            },
            [this](td_api::authorizationStateWaitPhoneNumber &) {
              std::cout << "Enter phone number: " << std::flush;
              std::string phone_number;
              std::cin >> phone_number;
              send_query(td_api::make_object<td_api::setAuthenticationPhoneNumber>(phone_number, nullptr),
                         create_authentication_query_handler());
            },
            [this](td_api::authorizationStateWaitEncryptionKey &) {
              std::cout << "Enter encryption key or DESTROY: " << std::flush;
              std::string key;
              std::getline(std::cin, key);
              if (key == "DESTROY") {
                send_query(td_api::make_object<td_api::destroy>(), create_authentication_query_handler());
              } else {
                send_query(td_api::make_object<td_api::checkDatabaseEncryptionKey>(std::move(key)),
                           create_authentication_query_handler());
              }
            },
            [this](td_api::authorizationStateWaitTdlibParameters &) {
              auto parameters = td_api::make_object<td_api::tdlibParameters>();
              parameters->database_directory_ = "tdlib";
              parameters->use_message_database_ = true;
              parameters->use_secret_chats_ = true;
              parameters->api_id_ = 94575;
              parameters->api_hash_ = "a3406de8d171bb422bb6ddf3bbd800e2";
              parameters->system_language_code_ = "en";
              parameters->device_model_ = "Desktop";
              parameters->system_version_ = "Unknown";
              parameters->application_version_ = "1.0";
              parameters->enable_storage_optimizer_ = true;
              send_query(td_api::make_object<td_api::setTdlibParameters>(std::move(parameters)),
                         create_authentication_query_handler());
            }));
}

void telegram_cli::send_query(td_api::object_ptr<td_api::Function> f, std::function<void(Object)> handler) 
{
    auto query_id = next_query_id();
    if (handler) {
      m_handlers.emplace(query_id, std::move(handler));
    }
    m_client->send({query_id, std::move(f)});
}

void telegram_cli::check_authentication_error(Object object) 
{
    if (object->get_id() == td_api::error::ID) {
      auto error = td::move_tl_object_as<td_api::error>(object);
      std::cout << "Error: " << to_string(error) << std::flush;
      on_authorization_state_update();
    }
}

std::string get_user_name(std::int32_t user_id) 
{
    auto it = m_users.find(user_id);
    if (it == m_users.end()) {
      return "unknown user";
    }
    return it->second->first_name_ + " " + it->second->last_name_;
}

}
