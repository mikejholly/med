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

  med_screen_init(&state->screen, state->fd);

  med_buffer_init(&state->buffer, 2048);
  med_buffer_puts(&state->buffer, MED_FUNC_ENTER_CA);
  med_buffer_puts(&state->buffer, MED_FUNC_ENTER_KEYPAD);
  med_buffer_puts(&state->buffer, MED_FUNC_HIDE_CURSOR);
  med_buffer_puts(&state->buffer, MED_FUNC_CLEAR_SCREEN);

  med_state_render(state);
}

void med_state_open(med_state_t *state) {
  state->fd = open("/dev/tty", O_RDWR);
}

void med_state_termios(med_state_t *state) {
  struct termios tios;

  tcgetattr(state->fd, &tios);

  tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tios.c_oflag &= ~OPOST;
  tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tios.c_cflag &= ~(CSIZE | PARENB);
  tios.c_cflag |= CS8;
  tios.c_cc[VMIN] = 0;
  tios.c_cc[VTIME] = 0;

  tcsetattr(state->fd, TCSAFLUSH, &tios);
}

void med_screen_clear(med_screen_t *screen) {
  int i;
  med_cell_t *cell;
  int n = screen->width * screen->height;
  for (i = 0; i < n; i++) {
    cell = screen->cells + i;
    cell->ch = ' ';
    cell->fg = 0;
    cell->bg = 0;
  }
}

void med_state_render(med_state_t *state) {
  int i;
  med_cell_t *cell;
  int n = state->screen.width * state->screen.height;

  med_buffer_clear(&state->buffer);
  med_buffer_puts(&state->buffer, MED_FUNC_CLEAR_SCREEN);

  for (i = 0; i < n; i++) {
    char buf[12];
    memset(buf, '\0', 12);
    cell = state->screen.cells + i;
    *buf = cell->ch;
    med_buffer_puts(&state->buffer, buf);
  }

  char cbuf[32];
  memset(cbuf, '\0', 32);
  sprintf(cbuf, "\033[%d;%dH", state->cursor.y + 1, state->cursor.x + 1);
  med_buffer_puts(&state->buffer, cbuf);

  write(state->fd, state->buffer.buf, state->buffer.len);
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
  int len = strlen(str);
  if (buffer->len + len > buffer->cap) {
    med_buffer_grow(buffer);
  }
  memcpy(buffer->buf + buffer->len, str, len);
  buffer->len += len;
}

void med_screen_init(med_screen_t *screen, int fd) {
  med_screen_update_size(screen, fd);
  int n = screen->width * screen->height;
  screen->cells = (med_cell_t *)malloc(n * sizeof(med_cell_t));
  med_screen_clear(screen);
}

void med_screen_update_size(med_screen_t *screen, int fd) {
  struct winsize sz;
  ioctl(fd, TIOCGWINSZ, &sz);
  screen->width = sz.ws_col;
  screen->height = sz.ws_row;
}

void med_screen_set(med_screen_t *screen, int x, int y, med_cell_t *cell) {
  int n = screen->width * y + 1 + x;
  screen->cells[n] = *cell;
}

void med_state_handle_input(med_state_t *state, med_buffer_t *buffer) {
  uint32_t ch = (uint32_t)buffer->buf[0];
  med_cell_t cell = {ch, 0, 0};
  med_screen_set(&state->screen, 3, 3, &cell);
  med_state_render(state);
}

int main(int argc, char **argv) {
  med_state_t state;
  med_state_init(&state);

  state.cursor.x = 1;
  state.cursor.y = 1;

  med_state_render(&state);

  int c;
  med_buffer_t input;
  med_buffer_init(&input, 128);

  while (1) {
    med_buffer_clear(&input);
    c = read(state.fd, input.buf, 32);
    if (c > 0) {
      input.len = c;
      med_state_handle_input(&state, &input);
    }
  }

  return 0;
}
