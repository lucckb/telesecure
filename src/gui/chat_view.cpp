#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <gui/window_driver_context.hpp>
#include <gui/utility.hpp>
#include <ctime>


namespace gui {
   

//! Construtor
chat_view::chat_view(color_t bg, color_t fg)
       : window(0,bg,fg,false)
{
}

//! Destructor
chat_view::~chat_view()
{
}

//When screen should be redrawed
bool chat_view::do_draw_screen( detail::window_driver_context& ctx )
{
    if(m_view) {
        int y,x;
        getmaxyx(ctx.win(),y,x);
        m_view->maxx(x);
    }
    if(m_view) {
        changed( changed()| m_view->changed() );
        m_view->displayed();
    }
    bool ret = changed();
    if( m_view && (changed()||m_view->changed()) ) {
        auto win = ctx.win();
        int maxx,maxy;
        getmaxyx(win,maxy,maxx);
        curs_set(0);
        //Calculate lines from the end
        const auto begin = std::make_reverse_iterator(m_view->end());
        const auto end = std::make_reverse_iterator(m_view->begin());
        wclear(win);
        int curr_y = maxy;
        for (auto i=begin;i!=end; ++i) {
            if(!i->second) {   //Is Sender
                setcolor(win,fgcolor(),bgcolor());
            } else {    //Im not sender
                setcolor(win,color_t::blue,bgcolor());
            }
            const auto real_lines = split_string(i->first,maxx);
            const auto rls = real_lines.size();
            const auto start_ln = curr_y - rls;
            for(int y=start_ln,n=0; n<rls ;++n,++y) {
                mvwaddstr(win,y,0,(real_lines[n]+"\n").c_str());
            }
            curr_y -= rls;
            if(curr_y<=0) break;
        }
        curs_set(2);
    }
    return ret;
}


//Assign view to the container
void chat_view::set_view(std::shared_ptr<chat_doc> view) 
{
    wclear(ctx().win());
    m_view = view;
    changed(true);
}

//Scroll up
void chat_view::scrolling(scroll_mode mode)
{
    auto win = ctx().win();
    int maxx,maxy;
    getmaxyx(win,maxy,maxx);
    maxy = maxy-1;
    m_view->rewind(mode==scroll_mode::up?-maxy:maxy);
    changed(true);
}


}