#include "board.h"
#include <cstring>
#include <iostream>

#define MAX_LINE_LEN 70

void command(Board &board, char *cmd) {
  if (cmd[0] == '\0') {
    return;
  }
  std::cout << "cmd: " << cmd << "\n";

  if (strcmp(cmd, "BOARD_SIZE") == 0) {
    std::cout << board.size << "\n";
  } else if (strcmp(cmd, "IS_BOARD_CORRECT") == 0) {
  }
}

void run_once(Board &board) {
  board.parse_from_stdin();

  char buffer[MAX_LINE_LEN];
  std::cin.getline(buffer, sizeof(buffer));

  while (!std::cin.eof() && buffer[0] != '-' && buffer[0] != ' ') {
    command(board, buffer);

    std::cin.getline(buffer, sizeof(buffer));
  }
}

int main() {
  Board board;

  // skip ---
  char buffer[MAX_LINE_LEN];
  std::cin.getline(buffer, sizeof(buffer));

  while (!std::cin.eof()) {
    run_once(board);
  }

  return 0;
}
