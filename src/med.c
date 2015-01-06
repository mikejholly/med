#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "med.h"

void med_state_init(med_state_t *state) {
  med_state_open(state);
  med_state_termios(state);

  med_screen_init(&state->screen, state->tty_fd);

  med_buffer_init(&state->buffer, 2048);
  med_buffer_puts(&state->buffer, MED_FUNC_ENTER_CA);
  med_buffer_puts(&state->buffer, MED_FUNC_ENTER_KEYPAD);
  med_buffer_puts(&state->buffer, MED_FUNC_HIDE_CURSOR);

  med_state_render(state);
}

void med_state_open(med_state_t *state) {
  state->tty_fd = open("/dev/tty", O_RDWR);
}

void med_state_termios(med_state_t *state) {
  struct termios orig_tios;
  struct termios tios;

  tcgetattr(state->tty_fd, &orig_tios);

  memcpy(&tios, &orig_tios, sizeof(tios));

  tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tios.c_oflag &= ~OPOST;
  tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tios.c_cflag &= ~(CSIZE | PARENB);
  tios.c_cflag |= CS8;
  tios.c_cc[VMIN] = 0;
  tios.c_cc[VTIME] = 0;

  tcsetattr(state->tty_fd, TCSAFLUSH, &tios);
}

void med_cells_init(med_cell_t *cells, int x, int y) {
  int n = x * y;
  cells = (med_cell_t *)malloc(n * sizeof(med_cell_t));
}

void med_cells_clear(med_cell_t *cells, int width, int height) {
  int i;
  int n = width * height;
  for (i = 0; i < n; i++) {
    cells[i].ch = ' ';
    cells[i].fg = 0;
    cells[i].bg = 0;
  }
}

void med_state_render(med_state_t *state) {
  int i;
  int n = state->screen.width * state->screen.height;
  med_buffer_clear(&state->buffer);
  med_buffer_puts(&state->buffer, MED_FUNC_CLEAR_SCREEN);
}

void med_buffer_init(med_buffer_t *buffer, int cap) {
  buffer->len = 0;
  buffer->cap = cap;
  buffer->buf = (char *)malloc(cap * sizeof(char *));
}

void med_buffer_grow(med_buffer_t *buffer) {
  int len = buffer->cap * 2;
  buffer->buf = (char *)realloc(buffer->buf, len * sizeof(char *));
  buffer->cap = len;
}

void med_buffer_clear(med_buffer_t *buffer) {
  buffer->len = 0;
}

void med_buffer_puts(med_buffer_t *buffer, char *str) {
  int extra = strlen(str);
  if (buffer->len + extra) {
    med_buffer_grow(buffer);
  }
}

void med_screen_init(med_screen_t *screen, int fd) {
  med_screen_update_size(screen, fd);
  med_cells_init(screen->cells, screen->width, screen->height);
}

void med_screen_update_size(med_screen_t *screen, int fd) {
  struct winsize sz;
  ioctl(fd, TIOCGWINSZ, &sz);
  screen->height = sz.ws_col;
  screen->width = sz.ws_row;
}

void med_screen_set(med_screen_t *screen, int x, int y, med_cell_t *cell) {
  int n = (screen->width * y + x);
  screen->cells[n] = *cell;
}

int main(int argc, char **argv) {
  med_state_t state;
  med_state_init(&state);
  med_state_render(&state);
  return 0;
}
