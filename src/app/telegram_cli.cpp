#include <app/telegram_cli.hpp>
#include <app/tele_app.hpp>
#include <chrono>


namespace app {
namespace td_api = td::td_api;

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
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
}

}
