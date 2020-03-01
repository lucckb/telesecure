#include <app/tele_app.hpp>
#include <input/input_manager.hpp>
#include <gui/window_manager.hpp>
#include <gui/edit_box.hpp>
#include <gui/status_bar.hpp>
#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <readline/readline.h>

namespace app {

// Constructor
tele_app::tele_app()
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
   m_chats[0] = gui::chat_doc::clone(0,"cmd>"); 
   //Add bar user
   bar->add_user(0,"C M");
}

// Initialize input box
void tele_app::init_input()
{
    auto& inp = input::input_manager::get(); 
    auto& win = gui::window_manager::get();
    inp.register_add_char([&](std::string_view ch) {
     
        win.win<gui::edit_box>(win_edit)->add_new_char(ch);
        win.repaint();
    });
    inp.register_delete_char([&](){
        win.win<gui::edit_box>(win_edit)->del_char();
        win.repaint();
    });
    inp.register_line_completed(std::bind(&tele_app::on_line_completed,this));
    inp.register_switch_window(std::bind(&tele_app::on_switch_buffer,this,std::placeholders::_1));
    inp.forward_to_readline(true);
    //Readline refresh added
    m_console.registerRedisplayCommand([&]() {
        win.win<gui::edit_box>(win_edit)->on_readline_handle(rl_display_prompt, rl_line_buffer,rl_point);
    });
    //Readline callback bind function
    inp.register_readline_callback(
        std::bind(&CppReadline::Console::forwardToReadline,
        std::ref(m_console),std::placeholders::_1)
    );
}

// Run main handler
void tele_app::run()
{
    init_gui();
    init_input();
    auto& inp = input::input_manager::get(); 
    auto& win = gui::window_manager::get();
    win.create_all();
    win.repaint();
    inp.loop();
}

 //Callback when input data completed
void tele_app::on_line_completed( )
{
    auto& win = gui::window_manager::get();
    win.win<gui::edit_box>(win_edit)->clear();
    win.repaint();
}
// On switch buffer
void tele_app::on_switch_buffer(int num)
{
    auto& win = gui::window_manager::get();
    if(m_chats[num]) {
       win.win<gui::status_bar>(win_status)->set_active(m_chats[num]->id());
       m_current_buffer = num;
       input::input_manager::get().forward_to_readline(m_current_buffer==0); 
    }
}

}

