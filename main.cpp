#include "board.h"
#include <cstring>
#include <iostream>

#define MAX_LINE_LEN 70

void print_bool(bool val) {
  if (val)
    std::cout << "YES";
  else
    std::cout << "NO";
}

void command(Board &board, char *cmd) {
  if (cmd[0] == '\0') {
    return;
  }
  // std::cout << "cmd: " << cmd << "\n";

  if (strcmp(cmd, "BOARD_SIZE") == 0) {
    std::cout << board.size;
  } else if (strcmp(cmd, "PAWNS_NUMBER") == 0) {
    std::cout << board.red_count + board.blue_count;
  } else if (strcmp(cmd, "IS_BOARD_CORRECT") == 0) {
    print_bool(board.is_board_correct());
  } else if (strcmp(cmd, "IS_GAME_OVER") == 0) {
    Player winner = board.winner();
    switch (winner) {
    case NONE:
      std::cout << "NO";
      break;
    case RED:
      std::cout << "YES RED";
      break;
    case BLUE:
      std::cout << "YES BLUE";
      break;
    }
  } else if (strcmp(cmd, "IS_BOARD_POSSIBLE") == 0) {
    print_bool(board.is_board_possible());
  }
  std::cout << '\n';
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
