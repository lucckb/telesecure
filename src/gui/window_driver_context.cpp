#include <gui/window_driver_context.hpp>
#include <gui/utility.hpp>

namespace gui {
namespace detail {



window_driver_context::window_driver_context(const rect& c)
     : mwin(newwin(c.cy,c.cx,c.y,c.x),&delwin)
{
}

auto window_driver_context::win() noexcept -> WINDOW* {
    return mwin.get();
}



}}