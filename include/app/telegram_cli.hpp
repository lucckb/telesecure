#pragma once
#include <string>
#include <functional>
#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <thread>
#include <atomic>

namespace app {
    class tele_app;
    class telegram_cli 
    {
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
    private:
        //Telegram main client thread
        void client_thread();
    private:
        tele_app& m_app;
        using Object = td::td_api::object_ptr<td::td_api::Object>;
        std::unique_ptr<td::Client> m_client;
        std::thread m_thread;
        std::atomic<bool> m_thread_running;
    };
};