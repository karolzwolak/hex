#include "board.h"
#include <cstdio>
#include <cstring>
#include <iostream>

#define MAX_LINE_LEN 70
#define DEBUG false

void print_bool(bool val) {
  if (val)
    std::cout << "YES";
  else
    std::cout << "NO";
}

bool string_startswith(const char *str, const char *prefix) {
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

void command(Board &board, char *cmd) {
  if (cmd[0] == '\0') {
    return;
  }
  if (DEBUG) {
    std::cout << "c: " << cmd << ": ";
  }

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
  } else if (string_startswith(cmd, "CAN_")) {
    Player player;
    char player_str[MAX_LINE_LEN];
    char opponent_str[MAX_LINE_LEN];

    bool one_move = sscanf(cmd, "CAN_%[^_]_WIN_IN_1_MOVE_WITH_%[^_]_OPPONENT",
                           player_str, opponent_str) == 2;
    bool two_move = sscanf(cmd, "CAN_%[^_]_WIN_IN_2_MOVES_WITH_%[^_]_OPPONENT",
                           player_str, opponent_str) == 2;
    if (strcmp(player_str, "RED") == 0) {
      player = RED;
    } else if (strcmp(player_str, "BLUE") == 0) {
      player = BLUE;
    } else {
      std::cerr << "Invalid player: " << player_str << "\n";
      return;
    }

    bool perfect_op = strcmp(opponent_str, "PERFECT") == 0;

    bool res = false;
    if (one_move) {
      res = board.can_player_win_in_one_move(player, perfect_op);
    } else if (two_move) {
      res = board.can_player_win_in_two_moves(player, perfect_op);
    } else {
      std::cerr << "Invalid command: " << cmd << "\n";
      return;
    }
    if (res) {
      std::cout << "YES";
    } else {
      std::cout << "NO";
    }
  }
  std::cout << "\n";
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
