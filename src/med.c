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
  med_state_get_size(state);
  med_state_init_cells(state);

  med_state_write(state, funcs[MED_FUNC_ENTER_CA]);
  med_state_write(state, funcs[MED_FUNC_ENTER_KEYPAD]);
  med_state_write(state, funcs[MED_FUNC_HIDE_CURSOR]);

  med_state_clear(state);
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

void med_state_get_size(med_state_t *state) {
  struct winsize sz;

  ioctl(state->tty_fd, TIOCGWINSZ, &sz);

  state->height = sz.ws_col;
  state->width = sz.ws_row;
}

void med_state_init_cells(med_state_t *state) {
  int n = state->width * state->height;
  state->cells = (med_cell_t*)malloc(n * sizeof(med_cell_t));
}

void med_state_set_cell(med_state_t *state, int x, int y, med_cell_t *cell) {
  int n = (state->width * y + x);
  state->cells[n] = *cell;
}

void med_state_clear(med_state_t *state) {
  int i;
  int n = state->width * state->height;
  for (i = 0; i < n; i++) {
    state->cells[i].ch = ' ';
    state->cells[i].fg = 0;
    state->cells[i].bg = 0;
  }
}

void med_state_render(med_state_t *state) {
  int i;
  int n = state->width * state->height;
  med_state_buffer
  med_state_write(state, funcs[MED_FUNC_CLEAR_SCREEN]);
  med_state_write(state, str);
}

void med_state_write(med_state_t *state, char *str) {
  write(state->tty_fd, str, strlen(str));
}

int main(int argc, char **argv) {
  med_state_t state;
  med_state_init(&state);
  med_state_render(&state);
  return 0;
}
