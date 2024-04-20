#pragma once

#include <vector>
#define FIRST RED
#define SECOND BLUE
#define MAX_SIZE 11

enum Player {
  NONE,
  RED,
  BLUE,
};

struct Position {
  int depth, offset;

  bool is_valid(const int board_size);
  int as_index(const int board_size);
  // writes all 6 cell neighbors, regardless if they are valid
  void neighbors(Position (&arr_out)[6]);

  bool is_dest_side(const int board_size, const Player player);
  bool is_start_side(const int board_size, const Player player);
};

// SIZE * size hexagonal board
//
// example 3x3 board:
//   r 0 b
//  r 1 2 b
// | 3 4 5 |
//  b 6 7 r
//   b 8 r
//
struct Board {
  int size;
  std::vector<Player> cells;
  int red_count;
  int blue_count;

  // we just assume every board is of max size
  // after, parsing the input, we will shrink it
  Board();

  // deep copy
  Board(const Board &other);
  void resize(int new_size);
  void parse_from_stdin();

  int player_count(const Player player);
  Player curr_turn();

  bool is_board_correct();
  Player winner();
  bool is_board_possible();

  void dfs_populate_start_side(const Player player, std::vector<bool> &visited,
                               std::vector<Position> &pos_stack);
  bool is_player_connected(const Player player);
  int player_connection_count(const Player player);
};
