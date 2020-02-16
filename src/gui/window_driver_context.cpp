#include <gui/window_driver_context.hpp>

namespace gui {
namespace detail {

// Window private driver context
auto window_driver_context::window(int a,int b,int c, int d, bool border) ->WINDOW*
{
    return border?newwin(a,b,c,d):newwin(a,b,c,d);
}

auto window_driver_context::mwindow(int a,int b,int c, int d, bool border, WINDOW *wnd) -> WINDOW*
{
    auto dw = derwin(wnd,a-2,b-2,1,1);
    return border?dw:nullptr;
}
window_driver_context::window_driver_context(const detail::curses_coord& c, bool border)
     : mwin(window(c.nlines,c.ncols,c.y0,c.x0,border),&delwin)
    , xwin(mwindow(c.nlines,c.ncols,c.y0,c.x0,border,mwin.get()),&delwin)
{
}

auto window_driver_context::win() noexcept -> WINDOW* {
    return xwin?xwin.get():mwin.get();
}

auto window_driver_context::winm() noexcept -> WINDOW* {
    return mwin.get();
}


}}