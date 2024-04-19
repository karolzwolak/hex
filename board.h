#pragma once

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
  // assumes the index is valid
  int as_index(const int board_size);
  // writes all 6 cell neighbors, regardless if they are valid
  void neighbors(Position (&arr_out)[6]);
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
  Player *cells;
  int red_count;
  int blue_count;

  // we just assume every board is of max size
  // after, parsing the input, we will shrink it
  Board();

  ~Board();

  void resize(int new_size);
  void parse_from_stdin();
};
