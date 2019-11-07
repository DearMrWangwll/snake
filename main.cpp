#include "console_library/console_library.h"

constexpr int W = 50;
constexpr int H = 50;
int x = 0;
int y = 0;
int old_x, old_y;
int old_old_x, old_old_y;

bool keyA, keyD, keyW, keyS;

void display() {
  console::draw::clear();
  console::draw::point(old_old_x, old_old_y, console::Color::DARK_WHITE);
  console::draw::point(old_x, old_y, console::Color::DARK_RED);
  console::draw::point(x, y, console::Color::RED);
  console::draw::flush();
}

void timer() {
  old_old_x = old_x;
  old_old_y = old_y;
  old_x = x;
  old_y = y;

  using console::keyboard::press_key;

  if (press_key('A')) x += -1;
  if (press_key('D')) x += +1;
  if (x < 0) x = W - 1;
  if (x >= W) x = 0;

  if (press_key('W')) y += -1;
  if (press_key('S')) y += +1;
  if (y < 0) y = H - 1;
  if (y >= H) y = 0;

  console::draw::post_redisplay();
}

int main(int argc, char* argv[]) {
  if (!console::init(W, H)) return 1;

  console::set_display_func(display);
  console::set_timer_func(timer, 50);

  console::main_loop();

  return 0;
}