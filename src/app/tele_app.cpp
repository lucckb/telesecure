#include <app/tele_app.hpp>
#include <input/input_manager.hpp>
#include <gui/window_manager.hpp>
#include <gui/edit_box.hpp>
#include <gui/status_bar.hpp>
#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <app/telegram_cli.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <filesystem>
#include <gui/utility.hpp>
#include <boost/process.hpp>
#include <util.hpp>


namespace app {

namespace {
    constexpr auto conf_file = "/.config/telesecure/config.xml";
    constexpr auto conf_dir = "/.config/telesecure";
}

// Constructor
tele_app::tele_app()
    : m_inp(std::ref(*this)),m_tcli(new telegram_cli(*this))
{
    for( auto& e : m_edit_lines ) {
        e = std::make_shared<std::string>();
    }
}

//Destructor
tele_app::~tele_app()
{
}

//Initialize gui
void tele_app::init_gui()
{ 
   auto bar = gui::status_bar::clone(gui::color_t::green, gui::color_t::black);
   m_win.add_window(bar);
   auto view = gui::chat_view::clone(gui::color_t::black, gui::color_t::yellow);
   m_win.add_window(view);
   auto edit = gui::edit_box::clone(gui::color_t::blue, gui::color_t::white);
   m_win.add_window(edit);
   //Create first chat only
   m_chats[0] = gui::chat_doc::clone(0,"cmd");
   //Add bar user
   bar->add_user(0,"C M");
   m_chats[m_chats.size()-1] = gui::chat_doc::clone(m_chats.size()-1,"shell");
   bar->add_user(m_chats.size()-1,"S H");
}

// When char is deleted
void tele_app::on_add_char(std::string_view ch)
{
    std::unique_lock _lck(m_mtx);
    m_win.win<gui::edit_box>(win_edit)->add_new_char(ch);
    m_win.repaint();
    if(m_current_buffer!=0) {   //When the chat is selected
        m_tcli->update_typing_chat(m_chats[m_current_buffer]->id(),true);
    }
}

//On delete char
void tele_app::on_delete_char()
{
    std::unique_lock _lck(m_mtx);
    m_win.win<gui::edit_box>(win_edit)->del_char();
    m_win.repaint();
    if(m_current_buffer!=0) {   //When the chat is selected
        m_tcli->update_typing_chat(m_chats[m_current_buffer]->id(),true);
    }
}

//On forward to readline
void tele_app::on_forward_char_to_readline(char ch)
{
    m_console->forwardToReadline(ch);
}

//On switch right
void tele_app::on_switch_right()
{
    std::unique_lock _lck(m_mtx);
    for(int i=m_current_buffer+1;i<m_chats.size();++i) {
        if(m_chats[i]) {
            on_switch_buffer_nolock(i);
            m_win.repaint();
            return; 
        }
    }
}
//On switch left
void tele_app::on_switch_left()
{
    std::unique_lock _lck(m_mtx);
    for(int i=m_current_buffer-1;i>=0;--i) {
        if(m_chats[i]) {
            on_switch_buffer_nolock(i);
            m_win.repaint();
            return; 
        }
    }
}
//On page up 
void tele_app::on_page_up()
{
    std::unique_lock _lck(m_mtx);
    m_win.win<gui::chat_view>(win_view)->scrolling(gui::chat_view::scroll_mode::up);
    m_win.repaint();
}

//On page down
void tele_app::on_page_down()
{
    std::unique_lock _lck(m_mtx);
    m_win.win<gui::chat_view>(win_view)->scrolling(gui::chat_view::scroll_mode::down);
    m_win.repaint();
}

//On clear edit
void tele_app::on_clear_edit()
{
    std::unique_lock _lck(m_mtx);
    m_win.win<gui::edit_box>(win_edit)->clear();
    m_win.repaint();
}
// Initialize input box
void tele_app::init_input()
{
    m_console = std::make_unique<CppReadline::Console>(">");
    m_inp.forward_to_readline(true);
    m_win.win<gui::edit_box>(win_edit)->readline_mode(true);
    //Readline refresh added
    m_console->registerRedisplayCommand([this]() {
        std::unique_lock _lck(m_mtx);
        m_win.repaint();
    });
    //When readline option completed 
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
    //Create windows
    m_win.create_all();
    //Initial configuration
    m_win.win<gui::chat_view>(win_view)->set_view(m_chats[0]);
    m_win.repaint();
    //Start telegram client
    m_tcli->start();
    //Start input main loop
    m_inp.loop();
    //Wait Stop the telegram client and wait for termination
    m_tcli->stop();
    m_tcli->wait_for_terminate();
    save_opened_buffers();
}

 //Callback when input data completed
void tele_app::on_line_completed( )
{
    std::unique_lock _lck(m_mtx);
    auto ebox = m_win.win<gui::edit_box>(win_edit);
    const auto id = m_chats[m_current_buffer]->id();
    m_tcli->send_message_to(id,ebox->line());
    ebox->clear();
    m_win.repaint();
    if(m_current_buffer!=0) {   //When the chat is selected
        m_tcli->update_typing_chat(m_chats[m_current_buffer]->id(),false);
    }
} 

// On switch buffer
void tele_app::on_switch_buffer_nolock(int num)
{
    if(m_chats[num]) {
       auto swin = m_win.win<gui::status_bar>(win_status);
       auto edit = m_win.win<gui::edit_box>(win_edit);
       m_inp.forward_to_readline(num==0);
       edit->readline_mode(num==0);
       swin->set_active(num);
       swin->set_newmsg(num,false);
       //Assig buffer to chat view 
       if(num!=m_current_buffer) {
           auto chat = m_win.win<gui::chat_view>(win_view);
           chat->set_view(m_chats[num]);
           //Mark last message id as read
           if(num!=0 || num!=num_chats-1) {
                m_tcli->view_message( m_chats[num]->id(), m_chats[num]->last_message_id());
           }
           edit->set_line(m_edit_lines[num]);
       }
       m_current_buffer = num;
    }
}

void tele_app::on_switch_buffer(int window) 
{
    std::unique_lock _lck(m_mtx);
    on_switch_buffer_nolock(window);
    m_win.repaint();
}

//! When readline parser complete commmand
void tele_app::on_readline_completed(int code)
{
    std::unique_lock _lck(m_mtx);
    if(code == CppReadline::Console::NotFound) {
        m_chats[0]->add_line("Command not found",false,0);
    } else if(code==CppReadline::Console::Quit) {
        m_chats[0]->add_line("App closing. Please wait...",false,0);
        m_app_running = false;
    }
    m_win.repaint();
}

// Register commands
void tele_app::register_commands()
{
    //Handle help command on the terminal
    m_console->registerCommand( 
        "help", [&](const CppReadline::Console::Arguments&) {
            std::unique_lock _lck(m_mtx);
            auto view =  m_win.win<gui::chat_view>(win_view);
            m_chats[0]->add_line("Available commands are:",false,0);
            for(auto& cmd : m_console->getRegisteredCommands()) {
                m_chats[0]->add_line("\t " + cmd,false,0);
            }
            return 0;
    });
    //Authorization code
    m_console->registerCommand(
        "authcode", [&](const CppReadline::Console::Arguments& args) {
            if(args.size()<2) {
                new_control_message("authcode: missing code arg");
                return CppReadline::Console::ReturnCode::Error;
            }
            m_tcli->set_auth_code( args[1] );
            return CppReadline::Console::ReturnCode::Ok;
    });
    m_console->registerCommand(
        "authname", [&](const CppReadline::Console::Arguments& args) {
         if(args.size()<3) {
                new_control_message("name: missing name surname");
                return CppReadline::Console::ReturnCode::Error;
            }
            m_tcli->set_auth_user( args[1], args[2] );
            return CppReadline::Console::ReturnCode::Ok;
    });
    //Authorization name
    m_console->registerCommand(
        "authpass", [&](const CppReadline::Console::Arguments& args) {
            if(args.size()<2) {
                new_control_message("authcode: missing password");
                return CppReadline::Console::ReturnCode::Error;
            }
            m_tcli->set_auth_password( args[1] );
            return CppReadline::Console::ReturnCode::Ok;
    });
    //Authorization phone
    m_console->registerCommand(
        "authphoneno", [&](const CppReadline::Console::Arguments& args) {
            if(args.size()<2) {
                new_control_message("authcode: missing phone");
                return CppReadline::Console::ReturnCode::Error;
            }
            m_tcli->set_phone_number( args[1] );
            return CppReadline::Console::ReturnCode::Ok;
    });
    //Register command get chat list
    m_console->registerCommand(
        "userlist", [&](const CppReadline::Console::Arguments& args) {
         m_tcli->get_user_list();
         return CppReadline::Console::ReturnCode::Ok;
    });
    //Register command get chat list
    m_console->registerCommand(
        "chatlist", [&](const CppReadline::Console::Arguments& args) {
        m_tcli->get_chat_list();
        return CppReadline::Console::ReturnCode::Ok;
    });
    //Register command create new chat
    m_console->registerCommand(
        "newchat", std::bind(&tele_app::on_new_chat_create, this, std::placeholders::_1)
    );
    //Register command delete chat
    m_console->registerCommand(
        "delchat", std::bind(&tele_app::on_new_chat_delete, this, std::placeholders::_1)
    );
    //Send message to the selected chat
    m_console->registerCommand(
        "msg", [&](const CppReadline::Console::Arguments& args) {
        if(args.size()<3) {
            new_control_message("msg: missing id or message");
            return CppReadline::Console::ReturnCode::Error;
        }
        long id;
        try {
             id = std::stol(args[1]);
        } catch( std::invalid_argument& ) {
            new_control_message("msg: Invalid argument");
            return CppReadline::Console::ReturnCode::Error;
        }
        std::string msgs;
        for(auto iv=args.begin()+2;iv!=args.end();++iv) msgs += *iv + " ";
        m_tcli->send_message_to(id,msgs);
        return CppReadline::Console::ReturnCode::Ok;
    });
}

//When chat found
std::pair<std::shared_ptr<gui::chat_doc>,int> tele_app::find_chat(id_t id) noexcept 
{
    for(int i=0;i<m_chats.size();++i) 
        if(m_chats[i]&&m_chats[i]->id()==id) return std::make_pair(m_chats[i],i);
    return std::make_pair(m_chats[0],0);
}

//FInd existing chat
int tele_app::find_existing_chat(id_t id) noexcept
{
    for(int i=0;i<m_chats.size();++i) 
        if(m_chats[i]&&m_chats[i]->id()==id)  return i;
    return code_failure;
}

//When new message from chat
void tele_app::on_new_message(std::int64_t id, std::int64_t msgid, std::string_view name, std::string_view msg, bool outgoing, std::time_t date)
{
    using namespace std::string_literals;
    std::unique_lock _lck(m_mtx);
    auto chat = find_chat(id); 
    chat.first->add_line(!chat.second?("[" + std::to_string(id)+"] [" + std::string(name)+ "]: "s + std::string(msg)):msg,outgoing,date);
    chat.first->last_message_id(msgid);
    const auto nid = find_existing_chat(id);
    if(m_current_buffer==chat.second) m_tcli->view_message(id,msgid);
    else m_win.win<gui::status_bar>(win_status)->set_newmsg(nid>0?nid:0,true);
    m_win.repaint();
} 

//New control message
void tele_app::control_message_nlock(std::string_view msg)
{
    m_chats[0]->add_line(msg,false,0);
    if(m_current_buffer!=0) {
        m_win.win<gui::status_bar>(win_status)->set_newmsg(0,true);
    }
    m_win.repaint();
}

//! Find first free chat indentifier
int tele_app::find_free_chat_slot() noexcept
{
    for(int i=0;i<m_chats.size();++i) {
        if(!m_chats[i]) return i;
    }
    return code_failure;
}

//! Open and create new chat
int tele_app::on_new_chat_create(const CppReadline::Console::Arguments& args)
{
    std::unique_lock _lck(m_mtx);
    const auto nid = find_free_chat_slot();
    if(args.size()<2) {
        control_message_nlock("Error: invalid argument count. usage: newchat <id>");
        return CppReadline::Console::ReturnCode::Error;
    }
    if(nid<0) {
        control_message_nlock("Error: Unable to allocate console for new chat");
        return CppReadline::Console::ReturnCode::Error;
    }
    long chat_id;
    try {
        chat_id = std::stol(args[1]);
    } catch( std::invalid_argument& ) {
         control_message_nlock("newchat: Invalid argument <chatid>");
         return CppReadline::Console::ReturnCode::Error;
    }
    auto existing_id = find_existing_chat(chat_id);
    if(existing_id>=0) {
        control_message_nlock("newchat: Chat already opened");
        return CppReadline::Console::ReturnCode::Error;
    }
    m_tcli->open_chat(chat_id,[this,nid,chat_id]() {
        std::unique_lock _lck(m_mtx);
        const auto title = m_tcli->get_chat_title(chat_id);
        m_chats[nid] = gui::chat_doc::clone(chat_id,title);
        auto swin = m_win.win<gui::status_bar>(win_status);
        swin->add_user(nid,title);
        control_message_nlock("Chat: " + std::to_string(chat_id) + " opened at slot F" + std::to_string(nid+1));
        m_win.repaint();
    });
    return CppReadline::Console::ReturnCode::Ok;
}

//! Delete existing chat
int tele_app::on_new_chat_delete(const CppReadline::Console::Arguments& args)
{
    std::unique_lock _lck(m_mtx);
    if(args.size()<2) {
        control_message_nlock("Error: invalid argument count. usage: delchat <id>");
        return CppReadline::Console::ReturnCode::Error;
    }
    long chat_id;
    try {
        chat_id = std::stol(args[1]);
    } catch( std::invalid_argument& ) {
         control_message_nlock("delchat: Invalid argument <chatid>");
         return CppReadline::Console::ReturnCode::Error;
    }
    const auto ord = find_existing_chat(chat_id);
    if(ord<0) {
        control_message_nlock("delchat: Selected chat not found");
        return CppReadline::Console::ReturnCode::Error;
    }
    on_switch_buffer_nolock(0);
    m_win.win<gui::status_bar>(win_status)->del_user(ord);
    m_chats[ord].reset();
    control_message_nlock("Chat: " + std::to_string(chat_id) + " deleted from slot F" + std::to_string(ord+1));
    m_win.repaint();
    return CppReadline::Console::ReturnCode::Ok;
}


//Save buffer state
void tele_app::save_opened_buffers()
{
    namespace pt = boost::property_tree;
    pt::ptree tree;
    pt::ptree child;
    for(int i=1;i<m_chats.size();++i)
    {
        if(m_chats[i]) {
            pt::ptree item;
            item.put("chat_slot",i);
            item.put("chat_id",m_chats[i]->id());
            child.add_child("item", item);
        }
    }
    tree.add_child("config.chats",child);
    std::filesystem::create_directories(util::home_dir()+conf_dir);
    pt::write_xml(util::home_dir()+conf_file,tree);
}

//Read confgutation
std::vector<std::pair<int,long>> tele_app::read_config()
{
    namespace pt = boost::property_tree;
    pt::ptree tree;
    std::vector<std::pair<int,long>> ret;
    try {
        pt::read_xml(util::home_dir()+conf_file, tree);
        auto child =  tree.get_child("config.chats");
        int chat_slot {1};
        for(const auto& el: child) {
            if(el.first=="item") {
                auto chat_id = el.second.get<long>("chat_id");
                ret.push_back({chat_slot++,chat_id});
                if(chat_slot>=num_chats) break;
            }
        }
    } catch( const pt::xml_parser_error& er) {
      
    }
    return ret;
}

//Restore last buffers
void tele_app::restore_opened_buffers()
{
    const auto chat_list = read_config();
    for(const auto& chat: chat_list) {
        m_tcli->open_chat(chat.second,[this,chat]() {
            const auto title = m_tcli->get_chat_title(chat.second);
            m_chats[chat.first] = gui::chat_doc::clone(chat.second,title);
            auto swin = m_win.win<gui::status_bar>(win_status);
            swin->add_user(chat.first,title);
            control_message_nlock("Chat: " + std::to_string(chat.second)
                 + " opened at slot F" + std::to_string(chat.first+1));
            m_win.repaint();
        });
    }
}

//On leave session
void tele_app::on_leave_session()
{
    std::unique_lock _lck(m_mtx);
    on_switch_buffer_nolock(m_chats.size()-1);
    using namespace boost::process;
    ipstream pipe_stream;
    child c("ps aux", std_out > pipe_stream);
    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        m_chats[m_chats.size()-1]->add_line(line,false,0);
    c.wait();
    m_win.repaint();
}

//On user start or stop typing
void tele_app::on_user_typing(std::int64_t id, bool typing)
{
    std::unique_lock _lck(m_mtx);
    const auto nid = find_existing_chat(id);
    if(nid>=0) { //Handle message only when is visible
        m_win.win<gui::status_bar>(win_status)->set_typing(nid,typing);
        m_win.repaint();
    }
}

//Set online status   
void tele_app::set_online_status( std::int64_t id, bool online)
{
    std::unique_lock _lck(m_mtx);
    const auto nid = find_existing_chat(id);
    if(nid>=0) { //Handle message only when is visible
        m_win.win<gui::status_bar>(win_status)->set_online(nid,online);
        m_win.repaint();
    }
}

}

