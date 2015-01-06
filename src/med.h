#include <stdio.h>
#include <termios.h>

typedef struct {
  int x;
  int y;
} med_cursor_t;

typedef enum {
  MED_MODE_EDIT,
  MED_MODE_SELECT,
} med_mode_t;

typedef enum {
  MED_OP_ADD,
  MED_OP_REMOVE,
} med_op_t;

typedef struct {
  int offset;
  med_op_t op;
} med_history_item_t;

typedef struct {
  med_history_item_t *items;
} med_history_t;

typedef struct {
  uint32_t ch;
  uint16_t fg;
  uint16_t bg;
} med_cell_t;

typedef struct {
  med_cursor_t cursor;
  med_mode_t mode;
  med_history_t history;
  med_cell_t *cells;
  int tty_fd;
  int width;
  int height;
} med_state_t;

void med_state_init(med_state_t *state);
void med_state_open(med_state_t *state);
void med_state_termios(med_state_t *state);
void med_state_get_size(med_state_t *state);
void med_state_init_cells(med_state_t *state);
void med_set_cell(med_state_t *state, med_cell_t *cell);
void med_clear_cells(med_state_t *state);
void med_state_set_cell(med_state_t *state, int x, int y, med_cell_t *cell);
void med_state_clear(med_state_t *state);
void med_state_render(med_state_t *state);
void med_state_write(med_state_t *state, char *str);

char *keys[] = {
  "\033OP",
  "\033OQ",
  "\033OR",
  "\033OS",
  "\033[15~",
  "\033[17~",
  "\033[18~",
  "\033[19~",
  "\033[20~",
  "\033[21~",
  "\033[23~",
  "\033[24~",
  "\033[2~",
  "\033[3~",
  "\033OH",
  "\033OF",
  "\033[5~",
  "\033[6~",
  "\033OA",
  "\033OB",
  "\033OD",
  "\033OC",
  0,
};

enum {
  MED_FUNC_ENTER_CA,
  MED_FUNC_EXIT_CA,
  MED_FUNC_SHOW_CURSOR,
  MED_FUNC_HIDE_CURSOR,
  MED_FUNC_CLEAR_SCREEN,
  MED_FUNC_SGR0,
  MED_FUNC_UNDERLINE,
  MED_FUNC_BOLD,
  MED_FUNC_BLINK,
  MED_FUNC_REVERSE,
  MED_FUNC_ENTER_KEYPAD,
  MED_FUNC_EXIT_KEYPAD,
};

char *funcs[] = {
  "\033[?1049h",
  "\033[?1049l",
  "\033[?12l\033[?25h",
  "\033[?25l",
  "\033[H\033[2J",
  "\033(B\033[m",
  "\033[4m",
  "\033[1m",
  "\033[5m",
  "\033[7m",
  "\033[?1h\033=",
  "\033[?1l\033>",
};
