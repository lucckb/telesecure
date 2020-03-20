#pragma once
#include <string>
#include <functional>
#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <thread>
#include <atomic>
#include <map>
#include <chrono>
#include <shared_mutex>

namespace app {
    class tele_app;
    class telegram_cli 
    {
        using Object = td::td_api::object_ptr<td::td_api::Object>;
        //Authentication query handler
        auto create_authentication_query_handler() {
            return [this, id = m_authentication_query_id](Object object) {
                if(id == m_authentication_query_id) {
                    check_authentication_error(std::move(object));
                }
            };
        }
    public:
        //Contructors
        telegram_cli(tele_app& app);
        telegram_cli(telegram_cli&) = delete;
        telegram_cli& operator=(telegram_cli&) = delete;
        // Start telegram thread
        void start();
        //Stop the thread
        void stop();
        //Wait for thread terminate
        void wait_for_terminate();
        //Username message
        void set_auth_user(std::string first_name, std::string last_name)
        {
             send_query(td::td_api::make_object<td::td_api::registerUser>(first_name, last_name),
                create_authentication_query_handler());
        }
        //Set auth code
        void set_auth_code( std::string code ) {
            send_query(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code),
                    create_authentication_query_handler());
        }
        //Set auth passwoord
        void set_auth_password( std::string authpass ) {
            send_query(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(authpass),
                create_authentication_query_handler());
        }
        //Set phone number
        void set_phone_number( std::string phoneno ) {
            send_query(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phoneno, nullptr),
                create_authentication_query_handler());
        }
        // Open chat
        void open_chat(std::uint64_t id, std::function<void()> completed);
        // Close chat
        void close_chat(std::uint64_t id) {
            send_query(td::td_api::make_object<td::td_api::closeChat>(id),{});
        }
        // View message read
        void view_message(std::uint64_t id,std::int64_t msgid);
        // Get user list
        void get_user_list();
        // Get chat lists
        void get_chat_list();
        //Get chat title
        std::string get_chat_title(std::int64_t id) const noexcept {
            const auto ret = m_chat_title.find(id);
            return ret!=m_chat_title.end()?ret->second:"";
        }
        //Send message to the selected chat client
        void send_message_to(std::int64_t id, std::string_view msg);
        //Update typing
        void update_typing_chat(std::int64_t chat_id);
    private:
        //Telegram main client thread
        void client_thread();
        //Restart client
        void restart();
        //Process response
        void process_response(td::Client::Response response);
        //Process update
        void process_update(td::td_api::object_ptr<td::td_api::Object> update);
        //On authorization status update 
        void on_authorization_state_update();
        //On send query
        void send_query(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler);
        // Check authentication error
        void check_authentication_error(Object object);
        //Next query ID
        std::uint64_t next_query_id() noexcept {
            return ++m_current_query_id;
        }
        // Get user name
        std::string get_user_name(std::int32_t user_id);
        // Update is typing status
        void update_is_typing_status();
    private:
        tele_app& m_app;
        std::unique_ptr<td::Client> m_client;
        std::thread m_thread;
        std::atomic<bool> m_thread_running {};
        bool m_need_restart {};
        bool m_are_authorized {};
        using handlers_t = std::map<std::uint64_t, std::function<void(Object)>>;
        using users_t = std::map<std::int32_t, td::td_api::object_ptr<td::td_api::user>>;
        using chat_title_t = std::map<std::int64_t, std::string>;
        using authorization_state_t = td::td_api::object_ptr<td::td_api::AuthorizationState>;
        using user_action_state_t = std::map<std::int64_t, int>;
        using chat_last_typing_t = std::map<std::int64_t, std::pair<std::chrono::time_point<std::chrono::steady_clock>,bool>>;
        handlers_t m_handlers;
        users_t m_users;
        chat_title_t m_chat_title;
        chat_last_typing_t m_chat_typing;
        user_action_state_t m_action_state;
        authorization_state_t m_authorization_state;
        std::uint64_t m_current_query_id {};
        std::uint64_t m_authentication_query_id {};
        std::shared_mutex m_typing_lock;
    };
};