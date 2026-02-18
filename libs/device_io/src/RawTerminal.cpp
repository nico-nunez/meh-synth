#include "device_io/RawTerminal.h"

#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

namespace device_io {
static struct termios orig_termios;

void enableRawTerminal() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawTerminal);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~((unsigned long)BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~((unsigned long)OPOST);
  raw.c_lflag &= ~((unsigned long)ECHO | ICANON | IEXTEN);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

  printf("Raw mode enabled. Type anything ('q' to quit):\r\n");
}

void disableRawTerminal() {
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &orig_termios);
}

void captureKeyboardInputs() {
  bool running = true;
  char c;

  while (running) {
    c = '\0';
    if (read(STDIN_FILENO, &c, 1) == 1) {
      if (c == 'Q' || c == 'q') {
        disableRawTerminal();
        running = false;
        printf("Raw mode disable\r\n");
      } else {
        // TODO(nico): this will be for params
      }
    }
  }
}

} // namespace device_io
