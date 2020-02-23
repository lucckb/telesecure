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

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>
#include <sys/ioctl.h>


int main() { 
    
    using namespace gui;
    auto& wm = window_manager::get();
    auto wnd = status_bar::clone(color_t::green, color_t::black);
    wnd->add_user(111,"Jolanta Nowaczyk");
    wnd->add_user(112,"Piotr Dąbrowski");
    wnd->set_newmsg(112,true);
    wnd->set_online(111,true);
    wm.add_window(wnd);

    auto wnd2 = chat_view::clone(color_t::yellow,color_t::black);
    wm.add_window(wnd2);
    //Testing lines only
    for( int i=0;i<200;++i) {
      wnd2->add_line( "Jaco", std::string("Linia testowa numer ") + std::to_string(i+1));
    }
    auto wnd3 = edit_box::clone(color_t::red,color_t::black);
    wm.add_window(wnd3);
    wm.create_all();
    wm.repaint();
    for(;;)
    {
        auto c = getch();
        if(c==27) break;
        else if(c == KEY_BACKSPACE || c == KEY_DC || c == 127)
        {
          wnd3->del_char();
           wm.repaint();
        }
        else {
           wnd3->add_new_char(c);
           wm.repaint();
        }
    }
   
}