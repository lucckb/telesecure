#include <app/tele_app.hpp>
#include <input/input_manager.hpp>
#include <gui/window_manager.hpp>
#include <gui/edit_box.hpp>
#include <gui/status_bar.hpp>
#include <gui/chat_view.hpp>

namespace app {

// Constructor
tele_app::tele_app()
{
}

//Initialize gui
void tele_app::init_gui()
{
   auto inp = input::input_manager::get();  
   auto win = gui::window_manager::get();
   m_edit = gui::edit_box::clone(gui::color_t::white, gui::color_t::blue);
   win->add_window(m_edit);
   m_view = gui::chat_view::clone(gui::color_t::yellow, gui::color_t::black);
   win->add_window(m_view);
   m_bar = gui::status_bar::clone(gui::color_t::green, gui::color_t::black);
   win->add_window(m_bar);
   win->repaint();
}

// Run main handler
void tele_app::run()
{
    init_gui();
    auto inp = input::input_manager::get();  
    inp->loop();
}

}

