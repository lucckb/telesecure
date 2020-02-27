#include <iostream>
#include <chrono>
#include <thread>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <gui/window_manager.hpp>
#include <gui/window.hpp>
#include <gui/status_bar.hpp>
#include <gui/chat_view.hpp>
#include <gui/edit_box.hpp>
#include <gui/chat_doc.hpp>
#include <input/input_manager.hpp>


int main() { 
    
    using namespace gui;
    using namespace input;
    auto& wm = window_manager::get();
    auto& im = input_manager::get();
    auto wnd = status_bar::clone(color_t::green, color_t::black);
    wnd->add_user(111,"Jolanta Nowaczyk");
    wnd->add_user(112,"Piotr Dąbrowski");
    wnd->set_newmsg(112,true);
    wnd->set_online(111,true);
    wm.add_window(wnd);

    auto wnd2 = chat_view::clone(color_t::yellow,color_t::black);
    wm.add_window(wnd2);
    auto wind2v = chat_doc::clone();
    wnd2->set_view(wind2v);
    //Testing lines only
    for( int i=0;i<200;++i) {
      wind2v->add_line( "Jaco", std::string("Linia testowa numer ") + std::to_string(i+1));
    }
    auto wnd3 = edit_box::clone(color_t::red,color_t::black);
    wm.add_window(wnd3);
    wm.create_all();
    wm.repaint();
    im.register_switch_window([](int key) {
        printw("KEY %i\n", key);
        refresh();
    });
    im.register_add_char([&](std::string_view ch) {
        wnd3->add_new_char(ch);
        wm.repaint();
    });
    im.register_delete_char([&]() {
        wnd3->del_char();
        wm.repaint();
    });
    im.register_line_completed([&](){
        wnd3->clear();
        wm.repaint();
        //Get string here
    });
    im.register_leave_session([&]() {
     
    });
    im.loop();
}