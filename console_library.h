#pragma once

namespace console {

// Due to the limitations of the console itself,
// the size cannot be set too large
constexpr int kCONSOLE_MAX_WIDTH = 100, kCONSOLE_MAX_HEIGHT = 50;

/**********************************************************************
 * Initialize the console with given width and height.                *
 * [Warning] Cannot initialize more than once                         *
 * [Warning] Should not exceed the maximum size                       *
 *                                                                    *
 * Parameters:                                                        *
 *    width:  The width of console, smaller than kCONSOLE_MAX_WIDTH.  *
 *    height: The height of console, smaller than kCONSOLE_MAX_HEIGHT.*
 *                                                                    *
 * Return:                                                            *
 *    bool: Initialization success or not.                            *
 **********************************************************************/
bool init(int width, int height);

/**********************************************************************
 * Start main loop.                                                   *
 *                                                                    *
 * Parameters:                                                        *
 *    void                                                            *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void main_loop(void);

/**********************************************************************
 * Pop up a dialog box with caption, text, and only one 'OK' button.  *
 *                                                                    *
 * Parameters:                                                        *
 *    caption: The caption of the dialog box.                         *
 *    text:    The content of the dialog box.                         *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void prompt_box(const char* caption, const char* text);

/**********************************************************************
 * Specify display function.                                          *
 *                                                                    *
 * Parameters:                                                        *
 *    display_func: a function pointer to `void display(void)`        *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void set_display_func(void (*display_func)(void));

/**********************************************************************
 * Specify timer function.                                            *
 *                                                                    *
 * Parameters:                                                        *
 *    display_func: a function pointer to `void timer(void)`          *
 *    interval: The interval of the timer callback, in milliseconds.  * *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void set_timer_func(void (*timer_func)(void), unsigned int interval);

// Define some color constants
enum class Color {
  BLACK = 0X0000,
  DARK_BLUE = 0X0010,
  DARK_GREEN = 0X0020,
  DARK_CYAN = DARK_BLUE | DARK_GREEN,
  DARK_RED = 0X0040,
  DARK_MAGENTA = DARK_BLUE | DARK_RED,
  DARK_YELLOW = DARK_GREEN | DARK_RED,
  DARK_WHITE = DARK_BLUE | DARK_GREEN | DARK_RED,
  GRAY = 0X0080 | BLACK,
  BLUE = 0X0080 | DARK_BLUE,
  GREEN = 0X0080 | DARK_GREEN,
  CYAN = 0X0080 | DARK_CYAN,
  RED = 0X0080 | DARK_RED,
  MAGENTA = 0X0080 | DARK_MAGENTA,
  YELLOW = 0X0080 | DARK_YELLOW,
  WHITE = 0X0080 | DARK_WHITE,
};

enum class State {
	RUNNING, FAIL, FOOD, BITE
};

namespace draw {
/**********************************************************************
 * Clear the console buffer immediately.                              *
 *                                                                    *
 * Parameters:                                                        *
 *    void                                                            *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void clear(void);

/**********************************************************************
 * post a redisplay request, not necessarily redisplay immediately.   *
 *                                                                    *
 * Parameters:                                                        *
 *    void                                                            *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void post_redisplay(void);

/**********************************************************************
 * Flush the console buffer immediately.                              *
 *                                                                    *
 * Parameters:                                                        *
 *    void                                                            *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void flush(void);

/**********************************************************************
 * Draw a single point in the console.                                *
 * [Warning] If the parameter is out of bounds, there will be no      *
 *           action. You need to make sure that the position of       *
 *           the point is valid and reasonable.                       *
 *                                                                    *
 * Parameters:                                                        *
 *    x: X-coordinate of point                                        *
 *    y: Y-coordinate of point                                        *
 *    c: Color of rectangle.                                          *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void point(int x, int y, Color c);

/**********************************************************************
 * Draw a solid line in the console.                                  *
 * [Warning] If the parameter is out of bounds, there will be no      *
 *           action. You need to make sure that the position of       *
 *           the line is valid and reasonable.                        *
 *                                                                    *
 * Parameters:                                                        *
 *    x1: X-coordinate of the start point                             *
 *    y1: Y-coordinate of the start point                             *
 *    x2: X-coordinate of the stop point                              *
 *    y2: Y-coordinate of the stop point                              *
 *    c: Color of rectangle.                                          *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void line(int x1, int y1, int x2, int y2, Color c);

/**********************************************************************
 * Draw a solid rectangle in the console.                             *
 * [Warning] If the parameter is out of bounds, there will be no      *
 *           action. You need to make sure that the position of       *
 *           the rectangle is valid and reasonable.                   *
 *                                                                    *
 * Parameters:                                                        *
 *    x: X-coordinate of the upper left corner                        *
 *    y: Y-coordinate of the upper left corner                        *
 *    w: Width of rectangle.                                          *
 *    h: Height of rectangle.                                         *
 *    c: Color of rectangle.                                          *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void rect(int x, int y, int w, int h, Color c);

/**********************************************************************
 * Draw a solid circle in the console.                                *
 * [Warning] If the parameter is out of bounds, there will be no      *
 *           action. You need to make sure that the position of       *
 *           the circle is valid and reasonable.                      *
 *                                                                    *
 * Parameters:                                                        *
 *    x: X-coordinate of the center of circle                         *
 *    y: Y-coordinate of the center of circle                         *
 *    r: Radius of circle                                             *
 *    c: Color of rectangle.                                          *
 *                                                                    *
 * Return:                                                            *
 *    void                                                            *
 **********************************************************************/
void circle(int x, int y, int r, Color c);
}  // namespace draw

namespace keyboard {

void poll(int (&keyboard_state)[256]);

bool press_key(int vKey);

bool release_key(int vKey);

}  // namespace keyboard

}  // namespace console
