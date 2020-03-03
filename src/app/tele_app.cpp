#include <app/tele_app.hpp>
#include <input/input_manager.hpp>
#include <gui/window_manager.hpp>
#include <gui/edit_box.hpp>
#include <gui/status_bar.hpp>
#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <readline/readline.h>
#include <app/telegram_cli.hpp>

namespace app {

// Constructor
tele_app::tele_app()
    : m_tcli(new telegram_cli(*this))
{
}

//Destructor
tele_app::~tele_app()
{

}

//Initialize gui
void tele_app::init_gui()
{ 
   auto& win = gui::window_manager::get();
   auto bar = gui::status_bar::clone(gui::color_t::green, gui::color_t::black);
   win.add_window(bar);
   auto view = gui::chat_view::clone(gui::color_t::black, gui::color_t::yellow);
   win.add_window(view);
   auto edit = gui::edit_box::clone(gui::color_t::blue, gui::color_t::white);
   win.add_window(edit);
   //Create first chat only
   m_chats[0] = gui::chat_doc::clone(0,"cmd"); 
   //Add bar user
   bar->add_user(0,"C M");
}

// Initialize input box
void tele_app::init_input()
{
    auto& inp = input::input_manager::get(); 
    auto& win = gui::window_manager::get();
    m_console = std::make_unique<CppReadline::Console>(">");
    inp.register_add_char([&](std::string_view ch) {
        std::unique_lock _lck(m_mtx);
        win.win<gui::edit_box>(win_edit)->add_new_char(ch);
        win.repaint();
    });
    inp.register_delete_char([&](){
        std::unique_lock _lck(m_mtx);
        win.win<gui::edit_box>(win_edit)->del_char();
        win.repaint();
    });
    inp.register_line_completed(std::bind(&tele_app::on_line_completed,this));
    inp.register_switch_window(std::bind(&tele_app::on_switch_buffer,this,std::placeholders::_1));
    inp.forward_to_readline(true);
    //Readline refresh added
    m_console->registerRedisplayCommand([&]() {
        std::unique_lock _lck(m_mtx);
        win.win<gui::edit_box>(win_edit)->on_readline_handle(rl_display_prompt, rl_line_buffer,rl_point);
    });
    //Readline callback bind function
    inp.register_readline_callback(
        std::bind(&CppReadline::Console::forwardToReadline,
        std::ref(m_console),std::placeholders::_1)
    );
    m_console->registerCommandCompleted(
        std::bind(&tele_app::on_readline_completed, this, std::placeholders::_1)
    );
}

// Run main handler
void tele_app::run()
{
    //Subsystem initialization
    init_gui();
    init_input();
    register_commands();

    auto& inp = input::input_manager::get(); 
    auto& win = gui::window_manager::get();
    //Create windows
    win.create_all();
    win.repaint();
    //Initial configuration
    win.win<gui::edit_box>(win_edit)->on_readline_handle(rl_display_prompt, rl_line_buffer,rl_point);
    win.win<gui::chat_view>(win_view)->set_view(m_chats[0]);
    //Start telegram client
    m_tcli->start();
    //Start input main loop
    inp.loop();
    //Wait Stop the telegram client and wait for termination
    m_tcli->stop();
    m_tcli->wait_for_terminate();
}

 //Callback when input data completed
void tele_app::on_line_completed( )
{
    std::unique_lock _lck(m_mtx);
    auto& win = gui::window_manager::get();
    win.win<gui::edit_box>(win_edit)->clear();
    win.repaint();
}

// On switch buffer
void tele_app::on_switch_buffer(int num)
{
    std::unique_lock _lck(m_mtx);
    auto& win = gui::window_manager::get();
    if(m_chats[num]) {
       win.win<gui::status_bar>(win_status)->set_active(m_chats[num]->id());
       input::input_manager::get().forward_to_readline(num==0); 
       //Assig buffer to chat view 
       if(num!=m_current_buffer) {
           auto chat =  win.win<gui::chat_view>(win_view);
           chat->set_view(m_chats[num]);
           //TODO: Redraw
       }
       m_current_buffer = num;
    }
}

//! When readline parser complete commmand
void tele_app::on_readline_completed(int code)
{
    std::unique_lock _lck(m_mtx);
    auto& win = gui::window_manager::get();
    if(code != CppReadline::Console::Ok) {
        m_chats[0]->add_line("Command not found");
    }
    win.repaint();
}

// Register commands
void  tele_app::register_commands()
{
    //Handle help command on the terminal
    m_console->registerCommand( 
        "help", [&](const CppReadline::Console::Arguments&) {
            std::unique_lock _lck(m_mtx);
            auto& win = gui::window_manager::get();
            auto view =  win.win<gui::chat_view>(win_view);
            m_chats[0]->add_line("Available commands are:");
            for(auto& cmd : m_console->getRegisteredCommands()) {
                m_chats[0]->add_line("\t " + cmd);
            }
            return 0;
    });
}

}

