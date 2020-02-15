#include <iostream>
#include <chrono>
#include <thread>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <gui/window_manager.hpp>
#include <gui/window.hpp>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>
#include <sys/ioctl.h>

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}



int main() { 
    
    using namespace gui;
    auto& wm = window_manager::get();

    auto wnd = window::clone( 0.0, 0.0, 1, 0.5, color_t::red, color_t::blue, false );
    wm.add_window(wnd);
    wnd = window::clone( 0.0, 0.6, 1, 0.2, color_t::yellow, color_t::blue, false );
    wm.add_window(wnd);
    wm.repaint();

    for(;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));  
    }
    struct winsize w;        
    ioctl(0, TIOCGWINSZ, &w);
    for(;!kbhit();) {
        if(1)
        for(int i=0;i<=1;++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));  
            resize_term(w.ws_row-5*i,w.ws_col-5*i);
            wm.resize_all();
        }
    }
}