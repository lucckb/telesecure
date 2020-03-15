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
            item( std::string_view _line, bool _outgoing)
                : time(std::time(nullptr)),line(_line),outgoing(_outgoing)
            {}
            std::time_t time;
            std::string line;
            bool outgoing;
        };
     public:
        //! Set owner
        chat_doc(id_t id, std::string_view who)
            :m_id(id), m_who(who)
        {}
        //! Clone
        static std::shared_ptr<chat_doc> clone(id_t id, std::string_view who) {
            return std::make_shared<chat_doc>(id,who);
        }
        //Add single line
        void add_line(std::string_view line, bool outgoing=false);
         const auto& items() const noexcept {
            return m_items;
        }
        //Get identifier
        auto id() const {
            return m_id;
        }
        // Get who identifier
        auto who() const {
            return m_who;
        }
        //Set and get last message ID
        auto last_message_id() const noexcept {
            return m_last_msg_id;
        }
        void last_message_id(id_t id) noexcept {
            m_last_msg_id = id;
        }
        //Set changed mode
        auto changed() const noexcept {
            return m_changed;
        }
        //Mark when displayed on screen
        void displayed() noexcept {
            m_changed = false;
        }
    private:
        std::list<item> m_items;
        std::list<item>::iterator m_curr_line { m_items.end() };
        const std::string m_who;
        const id_t m_id;
        id_t m_last_msg_id;
        bool m_changed { true };
    };
}