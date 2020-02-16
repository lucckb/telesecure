#pragma once

namespace gui {
namespace detail {
    struct curses_coord {
        int nlines;
        int ncols;
        int x0;
        int y0;
    };
}}