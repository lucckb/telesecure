#include <gui/chat_view.hpp>

namespace gui {

//! Construtor
chat_view::chat_view( color_t bg, color_t fg )
       : window(0,0,1,0,bg,fg,false)
{

}
//! Destructor
chat_view::~chat_view()
{

}

void chat_view::do_draw_screen( detail::window_driver_context& ctx )
{

}

}