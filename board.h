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
  void reset();
  void parse_from_stdin();

  int player_count(const Player player);
  Player curr_turn();

  bool is_board_correct();
  Player winner();
  bool is_board_possible();

  bool is_pos_valid(const int row, const int col);
  int neighbors(const int id, int (&arr_out)[6]);

  int is_id_dest_side(const int id, const Player player);
  int is_id_start_side(const int id, const Player player);

  int player_connected_count(const Player player);
  bool player_connected_at(const Player player, const int id,
                           std::vector<bool> &visited,
                           std::vector<int> &id_stack);

  bool check_connected_cycle(const Player player);
  bool check_connected_cycle_at_id(const Player player, const int id,
                                   const int parent, bool connected,
                                   std::vector<bool> &visited);
};
