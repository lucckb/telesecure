namespace gui 
{
    // Color base enumeration
    enum class color_t  {
        black, blue, green, cyan,
        red, magenta, yellow, white
    };

    namespace _internal {
        //Color to curs color
       short curs_color(color_t fg);
       // Pair creation
       int colornum(color_t fg, color_t bg);
    }
    //! Set font to specific color
    void setcolor(color_t fg, color_t bg);
    //! Unset font from specific color
    void unsetcolor(color_t fg, color_t bg);
}
