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
  int fd;
} med_state_t;

#define MED_KEY_TYPE_CHAR 0
#define MED_KEY_TYPE_SPEC 1

void med_state_init(med_state_t *state);
void med_state_open(med_state_t *state);
void med_state_termios(med_state_t *state);
void med_state_update_size(med_state_t *state);
void med_state_render(med_state_t *state);
void med_state_cursor_move(med_state_t *state, int x, int y);

void med_screen_init(med_screen_t *screen, int fd);
void med_screen_update_size(med_screen_t *screen, int fd);
void med_screen_set(med_screen_t *screen, int x, int y, med_cell_t *cell);
void med_screen_clear(med_screen_t *screen);

void med_buffer_init(med_buffer_t *buffer, int cap);
void med_buffer_clear(med_buffer_t *buffer);
void med_buffer_grow(med_buffer_t *buffer);
void med_buffer_puts(med_buffer_t *buffer, char * str);
int  med_buffer_to_unicode(med_buffer_t *buffer, uint32_t *ch);

void med_key_extract(med_buffer_t *buffer, uint32_t *key);

#define MED_KEY_CTRL_TILDE       0x00
#define MED_KEY_CTRL_2           0x00
#define MED_KEY_CTRL_A           0x01
#define MED_KEY_CTRL_B           0x02
#define MED_KEY_CTRL_C           0x03
#define MED_KEY_CTRL_D           0x04
#define MED_KEY_CTRL_E           0x05
#define MED_KEY_CTRL_F           0x06
#define MED_KEY_CTRL_G           0x07
#define MED_KEY_BACKSPACE        0x08
#define MED_KEY_CTRL_H           0x08
#define MED_KEY_TAB              0x09
#define MED_KEY_CTRL_I           0x09
#define MED_KEY_CTRL_J           0x0A
#define MED_KEY_CTRL_K           0x0B
#define MED_KEY_CTRL_L           0x0C
#define MED_KEY_ENTER            0x0D
#define MED_KEY_CTRL_M           0x0D
#define MED_KEY_CTRL_N           0x0E
#define MED_KEY_CTRL_O           0x0F
#define MED_KEY_CTRL_P           0x10
#define MED_KEY_CTRL_Q           0x11
#define MED_KEY_CTRL_R           0x12
#define MED_KEY_CTRL_S           0x13
#define MED_KEY_CTRL_T           0x14
#define MED_KEY_CTRL_U           0x15
#define MED_KEY_CTRL_V           0x16
#define MED_KEY_CTRL_W           0x17
#define MED_KEY_CTRL_X           0x18
#define MED_KEY_CTRL_Y           0x19
#define MED_KEY_CTRL_Z           0x1A
#define MED_KEY_ESC              0x1B
#define MED_KEY_CTRL_LSQ_BRACKET 0x1B
#define MED_KEY_CTRL_3           0x1B
#define MED_KEY_CTRL_4           0x1C
#define MED_KEY_CTRL_BACKSLASH   0x1C
#define MED_KEY_CTRL_5           0x1D
#define MED_KEY_CTRL_RSQ_BRACKET 0x1D
#define MED_KEY_CTRL_6           0x1E
#define MED_KEY_CTRL_7           0x1F
#define MED_KEY_CTRL_SLASH       0x1F
#define MED_KEY_CTRL_UNDERSCORE  0x1F
#define MED_KEY_SPACE            0x20
#define MED_KEY_BACKSPACE2       0x7F
#define MED_KEY_CTRL_8           0x7F

// Custom values to represent escape codes (outside expected range)
#define MED_KEY_F1     0xFFFF - 1
#define MED_KEY_F2     0xFFFF - 2
#define MED_KEY_F3     0xFFFF - 3
#define MED_KEY_F4     0xFFFF - 4
#define MED_KEY_F5     0xFFFF - 5
#define MED_KEY_F6     0xFFFF - 6
#define MED_KEY_F7     0xFFFF - 7
#define MED_KEY_F8     0xFFFF - 8
#define MED_KEY_F9     0xFFFF - 9
#define MED_KEY_F10    0xFFFF - 10
#define MED_KEY_F11    0xFFFF - 11
#define MED_KEY_F12    0xFFFF - 12
#define MED_KEY_INSERT 0xFFFF - 13
#define MED_KEY_DELETE 0xFFFF - 14
#define MED_KEY_HOME   0xFFFF - 15
#define MED_KEY_END    0xFFFF - 16
#define MED_KEY_PGUP   0xFFFF - 17
#define MED_KEY_PGDN   0xFFFF - 18
#define MED_KEY_UP     0xFFFF - 19
#define MED_KEY_DOWN   0xFFFF - 20
#define MED_KEY_LEFT   0xFFFF - 21
#define MED_KEY_RIGHT  0xFFFF - 22

// TODO use term-specific values
#define MED_KEY_F1_ESC     "\033OP"
#define MED_KEY_F2_ESC     "\033OQ"
#define MED_KEY_F3_ESC     "\033OR"
#define MED_KEY_F4_ESC     "\033OS"
#define MED_KEY_F5_ESC     "\033[15~"
#define MED_KEY_F6_ESC     "\033[17~"
#define MED_KEY_F7_ESC     "\033[18~"
#define MED_KEY_F8_ESC     "\033[19~"
#define MED_KEY_F9_ESC     "\033[20~"
#define MED_KEY_F10_ESC    "\033[21~"
#define MED_KEY_F11_ESC    "\033[23~"
#define MED_KEY_F12_ESC    "\033[24~"
#define MED_KEY_INSERT_ESC "\033[2~"
#define MED_KEY_DELETE_ESC "\033[3~"
#define MED_KEY_HOME_ESC   "\033[H"
#define MED_KEY_END_ESC    "\033[F"
#define MED_KEY_PGUP_ESC   "\033[5~"
#define MED_KEY_PGDN_ESC   "\033[6~"
#define MED_KEY_UP_ESC     "\033[A"
#define MED_KEY_DOWN_ESC   "\033[B"
#define MED_KEY_LEFT_ESC   "\033[D"
#define MED_KEY_RIGHT_ESC  "\033[C"

static const char * MED_KEYS_ESC[] = {
  MED_KEY_F1_ESC,
  MED_KEY_F2_ESC,
  MED_KEY_F3_ESC,
  MED_KEY_F4_ESC,
  MED_KEY_F5_ESC,
  MED_KEY_F6_ESC,
  MED_KEY_F7_ESC,
  MED_KEY_F8_ESC,
  MED_KEY_F9_ESC,
  MED_KEY_F10_ESC,
  MED_KEY_F11_ESC,
  MED_KEY_F12_ESC,
  MED_KEY_INSERT_ESC,
  MED_KEY_DELETE_ESC,
  MED_KEY_HOME_ESC,
  MED_KEY_END_ESC,
  MED_KEY_PGUP_ESC,
  MED_KEY_PGDN_ESC,
  MED_KEY_UP_ESC,
  MED_KEY_DOWN_ESC,
  MED_KEY_LEFT_ESC,
  MED_KEY_RIGHT_ESC,
};

static const int MED_KEYS[] = {
  MED_KEY_F1,
  MED_KEY_F2,
  MED_KEY_F3,
  MED_KEY_F4,
  MED_KEY_F5,
  MED_KEY_F6,
  MED_KEY_F7,
  MED_KEY_F8,
  MED_KEY_F9,
  MED_KEY_F10,
  MED_KEY_F11,
  MED_KEY_F12,
  MED_KEY_INSERT,
  MED_KEY_DELETE,
  MED_KEY_HOME,
  MED_KEY_END,
  MED_KEY_PGUP,
  MED_KEY_PGDN,
  MED_KEY_UP,
  MED_KEY_DOWN,
  MED_KEY_LEFT,
  MED_KEY_RIGHT,
};

// TODO use term-specific values
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
