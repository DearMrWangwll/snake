#include "console_library.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdio>
#include <cstdlib>

#include <cmath>

// In visual studio, sprintf will cause C4996.
#if defined(_MSC_VER)
#define FORMAT_STRING sprintf_s
#else
#define FORMAT_STRING sprintf
#endif

namespace console {

// This is a structure for storing information about console settings.
// It is defined in the source file, not the header file, to ensure that it will
// not be used by the user
static struct _Info {
  // console handle
  HANDLE handle{NULL};

  // console size
  int width{-1};
  int height{-1};

  // console buffer
  using BUFFER = Color[kCONSOLE_MAX_HEIGHT][kCONSOLE_MAX_WIDTH];
  BUFFER buffer{Color::BLACK};

  // display
  using DISPLAY_FUNC = void (*)(void);
  DISPLAY_FUNC display_func{nullptr};
  bool redisplay_flag{false};

  // timer
  using TIMER_FUNC = void (*)(void);
  TIMER_FUNC timer_func{nullptr};
  unsigned int interval{0};
  unsigned int sleep_time{0};

} _info;

bool init(int width, int height) {
  // Used to prompt for an error message
  const char caption[] = "[Warning] console::init";

  // [Warning] cannot initialize more than once
  static bool s_initialized = false;
  if (s_initialized) {
    prompt_box(caption, "Cannot initialize more than once.");
    return false;
  }

  // [Warning] should not exceed the maximum size
  if (width > kCONSOLE_MAX_WIDTH) {
    char text[255];
    FORMAT_STRING(text, "Exceed the maximum width = %d : width = %d.",
                  kCONSOLE_MAX_WIDTH, width);
    prompt_box(caption, text);
    return false;
  }
  if (height > kCONSOLE_MAX_HEIGHT) {
    char text[255];
    FORMAT_STRING(text, "Exceed the maximum height = %d : height = %d.",
                  kCONSOLE_MAX_HEIGHT, height);
    prompt_box(caption, text);
    return false;
  }

  // store information
  _info.handle = GetStdHandle(STD_OUTPUT_HANDLE);
  _info.width = width;
  _info.height = height;

  // resize console
  // note that the aspect ratio of the character is 1:2
  char resize_command[255];
  FORMAT_STRING(resize_command, "mode con cols=%d lines=%d", 2 * width,
                height + 1);
  system(resize_command);

  // hide cursor
  CONSOLE_CURSOR_INFO cursor_info;
  cursor_info.dwSize = 1;
  cursor_info.bVisible = FALSE;
  SetConsoleCursorInfo(_info.handle, &cursor_info);

  // clear console
  system("cls");

  return true;
}

void main_loop(void) {
  // [Error] no display function was specified
  if (!_info.display_func) {
    prompt_box("[Error] console::main_loop",
               "No display function was specified!");
    exit(EXIT_FAILURE);
  }

  // first time display
  _info.display_func();

  // enter loop
  while (true) {
    // timer
    if (_info.timer_func) {
      Sleep(10);
      _info.sleep_time += 10;
      if (_info.sleep_time >= _info.interval) {
        _info.sleep_time = 0;
        _info.timer_func();
      }
    }

    // redisplay
    if (_info.redisplay_flag) {
      _info.redisplay_flag = false;
      _info.display_func();
    }
  }
}

void prompt_box(const char* caption, const char* text) {
  MessageBoxA(NULL, text, caption, MB_OK);
}

void set_display_func(void (*display_func)(void)) {
  _info.display_func = display_func;
}

void set_timer_func(void (*timer_func)(void), unsigned int interval) {
  _info.timer_func = timer_func;
  _info.interval = interval;
}

}  // namespace console

namespace console {
namespace draw {

void clear(void) {
  memset(&_info.buffer, 0,
         sizeof(Color) * kCONSOLE_MAX_HEIGHT * kCONSOLE_MAX_WIDTH);
}

void post_redisplay(void) { _info.redisplay_flag = true; }

void flush(void) {
  // move to (0, 0)
  SetConsoleCursorPosition(_info.handle, {0, 0});

  // flush buffer
  for (int i = 0; i < _info.height; ++i) {
    for (int j = 0; j < _info.width; ++j) {
      auto color = static_cast<WORD>(_info.buffer[i][j]);
      SetConsoleTextAttribute(_info.handle, color);
      printf("  ");
    }
  }

  // reset color to black
  SetConsoleTextAttribute(_info.handle, 0);
}

void point(int x, int y, Color c) {
  if (x < 0 || x > _info.width - 1) return;
  if (y < 0 || y > _info.height - 1) return;
  _info.buffer[y][x] = c;
}

void line(int x1, int y1, int x2, int y2, Color c) {
  if (x1 < 0 || x1 > _info.width - 1) return;
  if (y1 < 0 || y1 > _info.height - 1) return;
  if (x2 < 0 || x2 > _info.width - 1) return;
  if (y2 < 0 || y2 > _info.height - 1) return;

  double dx = double(x2) - x1;
  double dy = double(y2) - y1;
  int n = int(max(abs(dx), abs(dy)));
  if (n == 0) {
    _info.buffer[y1][x1] = c;
    return;
  }
  for (int i = 0; i <= n; ++i) {
    int x = int(round(x1 + i * dx / n));
    int y = int(round(y1 + i * dy / n));
    _info.buffer[y][x] = c;
  }
}
void rect(int x, int y, int w, int h, Color c) {
  if (w <= 0 || h <= 0) return;
  if (x < 0 || x + w > _info.width) return;
  if (y < 0 || y + h > _info.height) return;

  for (int i = y; i < y + h; ++i) {
    for (int j = x; j < x + w; ++j) {
      _info.buffer[i][j] = c;
    }
  }
}
void circle(int x, int y, int r, Color c) {
  if (r <= 0) return;
  if (x - r < 0 || x + r > _info.width - 1) return;
  if (y - r < 0 || y + r > _info.height - 1) return;

  for (int i = -r; i < +r; ++i) {
    for (int j = -r; j < +r; ++j) {
      if ((i * i + j * j) <= r * r) {
        _info.buffer[y + i][x + j] = c;
      }
    }
  }
}
}  // namespace draw
}  // namespace console

namespace console {
namespace keyboard {
void console::keyboard::poll(int (&keyboard_state)[256]) {
  for (int vkey = 0; vkey < 256; ++vkey) {
    keyboard_state[vkey] = GetAsyncKeyState(vkey) & 0x8000 ? 1 : 0;
  }
}

bool press_key(int vKey) { return (GetAsyncKeyState(vKey) & 0x8000) ? 1 : 0; }

bool release_key(int vKey) { return (GetAsyncKeyState(vKey) & 0x8000) ? 0 : 1; }
}  // namespace keyboard
}  // namespace console
