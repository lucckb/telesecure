#include <iostream>
#include <chrono>
#include <thread>
#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <ncurses.h>

int main() {
    initscr();
    curs_set(1);
    refresh();
    for(int i=0;i<100000;++i) {
          std::cout << "Hello world " << i << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}