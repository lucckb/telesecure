#include <iostream>
#include <chrono>
#include <thread>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <gui/window_manager.hpp>
#include <gui/window.hpp>
#include <gui/status_bar.hpp>

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
    auto wnd = status_bar::clone(color_t::green, color_t::black);
    wm.add_window(wnd);
    wm.repaint();

    for(;;)
    {
        auto c = getch();
        if(c=='r') {
          wm.repaint();
        } else if(c==27) break;
    }
   
}