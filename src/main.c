#include <stdio.h>
#include "termbox.h"

typedef struct {
  int x;
  int y;
} cursor_t;

void set_cursor(cursor_t *cursor) {
  tb_set_cursor(cursor->x, cursor->y);
  tb_present();
}

void handle_key_press(struct tb_event *ev, cursor_t *cursor) {
  int w = tb_width();
  int h = tb_height();
  switch (ev->key) {
    case TB_KEY_ARROW_UP:
      if (cursor->y == 0) return;
      cursor->y--;
      set_cursor(cursor);
      break;
    case TB_KEY_ARROW_DOWN:
      if (cursor->y == h) return;
      cursor->y++;
      set_cursor(cursor);
      break;
    case TB_KEY_ARROW_RIGHT:
      if (cursor->x == w) return;
      cursor->x++;
      set_cursor(cursor);
      break;
    case TB_KEY_ARROW_LEFT:
      if (cursor->x == 0) return;
      cursor->x--;
      set_cursor(cursor);
      break;
  }
}

int main(int argc, char **argv) {
  int ret;
  
  ret = tb_init();
  if (ret) {
    fprintf(stderr, "tb_init failed with error code %d\n", ret);
    return 1;
  }

  tb_clear();
  tb_set_cursor(0, 0);
  tb_present();

  tb_select_input_mode(TB_INPUT_ESC);
  struct tb_event ev;
  cursor_t cursor = {0, 0};

  while (tb_poll_event(&ev)) {
    switch (ev.type) {
      case TB_EVENT_KEY:
        if (ev.key == TB_KEY_CTRL_Q) {
          tb_shutdown();
          return 0;
        } else {
          handle_key_press(&ev, &cursor);
        }
        break;
      case TB_EVENT_RESIZE:
        break;
      default:
        break;
    }
  }
  tb_shutdown();
  return 0;
}
