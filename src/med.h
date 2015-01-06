#include <stdio.h>
#include <termios.h>

typedef struct {
  int x;
  int y;
  int hide;
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
  int len;
  int cap;
  char *buf;
} med_buffer_t;

typedef struct {
  med_cell_t *cells;
  int width;
  int height;
} med_screen_t;

typedef struct {
  med_cursor_t cursor;
  med_mode_t mode;
  med_history_t history;
  med_screen_t screen;
  med_buffer_t buffer;
  int tty_fd;
} med_state_t;

void med_state_init(med_state_t *state);
void med_state_open(med_state_t *state);
void med_state_termios(med_state_t *state);
void med_state_update_size(med_state_t *state);
void med_state_render(med_state_t *state);

void med_screen_init(med_screen_t *screen, int fd);
void med_screen_update_size(med_screen_t *screen, int fd);
void med_screen_set(med_screen_t *screen, int x, int y, med_cell_t *cell);

void med_cells_init(med_cell_t *cells, int width, int height);
void med_cells_clear(med_cell_t *cells, int width, int height);

void med_buffer_init(med_buffer_t *buffer, int cap);
void med_buffer_clear(med_buffer_t *buffer);
void med_buffer_grow(med_buffer_t *buffer);
void med_buffer_puts(med_buffer_t *buffer, char * str);

#define MED_KEY_F1     "\033OP"
#define MED_KEY_F2     "\033OQ"
#define MED_KEY_F3     "\033OR"
#define MED_KEY_F4     "\033OS"
#define MED_KEY_F5     "\033[15~"
#define MED_KEY_F6     "\033[17~"
#define MED_KEY_F7     "\033[18~"
#define MED_KEY_F8     "\033[19~"
#define MED_KEY_F9     "\033[20~"
#define MED_KEY_F10    "\033[21~"
#define MED_KEY_F11    "\033[23~"
#define MED_KEY_F12    "\033[24~"
#define MED_KEY_INSERT "\033[2~"
#define MED_KEY_DELETE "\033[3~"
#define MED_KEY_HOME   "\033OH"
#define MED_KEY_END    "\033OF"
#define MED_KEY_PGUP   "\033[5~"
#define MED_KEY_PGDN   "\033[6~"
#define MED_KEY_UP     "\033OA"
#define MED_KEY_DOWN   "\033OB"
#define MED_KEY_LEFT   "\033OD"
#define MED_KEY_RIGHT  "\033OC"

#define MED_FUNC_ENTER_CA     "\033[?1049h"
#define MED_FUNC_EXIT_CA      "\033[?12l\033[?25h"
#define MED_FUNC_SHOW_CURSOR  "\033[?12l\033[?25h"
#define MED_FUNC_HIDE_CURSOR  "\033[?25l"
#define MED_FUNC_CLEAR_SCREEN "\033[H\033[2J"
#define MED_FUNC_SGR0         "\033(B\033[m"
#define MED_FUNC_UNDERLINE    "\033[4m"
#define MED_FUNC_BOLD         "\033[1m"
#define MED_FUNC_BLINK        "\033[5m"
#define MED_FUNC_REVERSE      "\033[7m"
#define MED_FUNC_ENTER_KEYPAD "\033[?1h\033="
#define MED_FUNC_EXIT_KEYPAD  "\033[?1l\033>"
