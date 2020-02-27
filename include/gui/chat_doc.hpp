#pragma once

#include <string>
#include <list>
#include <ctime>
#include <string>
#include <memory>

namespace gui {

    //Chat document
    class chat_doc {
        static const auto maxnlines = 1000;
        struct item {
            item( std::string_view _who, std::string_view _line)
                : time(std::time(nullptr)),who(_who),line(_line)
            {}
            std::time_t time;
            std::string who;
            std::string line;
        };
     public:
        static std::shared_ptr<chat_doc> clone() {
            return std::make_shared<chat_doc>();
        }
          //Add single line
        void add_line(std::string_view who, std::string_view line);
         const auto& items() const noexcept {
            return m_items;
        }
    private:
        std::list<item> m_items;
        std::list<item>::iterator m_curr_line { m_items.end() };
    };

}