#include "board.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define MAX_LINE_LEN 70

int Position::as_index(const int board_size) {
  int sum;
  int offset = this->offset;
  if (depth >= board_size) {
    int depth = this->depth % board_size;
    sum = -((float)depth / 2) * (depth + 1);
    offset += board_size * board_size - 1;
  } else {
    sum = ((float)depth / 2) * (depth + 1);
  }

  return sum + offset;
}

bool Position::is_valid(int board_size) {
  bool depth_correct = 0 <= depth && depth <= 2 * (board_size - 1) + 1;
  bool offset_correct = 0 <= offset && offset <= depth % board_size;
  return depth_correct && offset_correct;
}
void Position::neighbors(Position (&arr_out)[6]) {
  arr_out[0] = Position{depth - 1, offset - 1};
  arr_out[1] = Position{depth - 1, offset};

  arr_out[2] = Position{depth, offset - 1};
  arr_out[3] = Position{depth, offset + 1};

  arr_out[4] = Position{depth + 1, offset};
  arr_out[5] = Position{depth + 1, offset + 1};
}

Board::Board() : size(MAX_SIZE), cells(nullptr), red_count(0), blue_count(0) {
  // cells = (Player *)calloc(size * size, sizeof(Player));
  cells = (Player *)malloc(size * size * sizeof(Player));
  assert(cells);
}
Board::~Board() { free(cells); }

void Board::resize(int new_size) {
  size = new_size;
  // cells = (Player *)realloc(cells, size * size * sizeof(Player));
  // assert(cells);
}

// assumes first line "---" is consumed
void Board::parse_from_stdin() {
  char buffer[MAX_LINE_LEN];
  int next_id = 0;

  std::cin.getline(buffer, sizeof(buffer));
  while (strcmp(buffer, " ---") != 0) {
    int len = strlen(buffer);
    int cell_count = 0;

    for (int i = 0; i < len; i++) {
      if (buffer[i] != '<') {
        continue;
      }
      // skip "< "
      i += 2;
      Player p;

      switch (buffer[i]) {
      case 'r':
        p = RED;
        red_count++;
        break;
      case 'b':
        p = BLUE;
        blue_count++;
        break;
      case ' ':
        p = NONE;
        break;
      default:
        std::cout << "parse error: wrong player '" << p << "'\n";
        exit(1);
        break;
      }
      cells[next_id] = p;
      next_id++;
      cell_count++;
    }

    std::cin.getline(buffer, sizeof(buffer));
    if (next_id > 1 && cell_count == 1) {
      break;
    }
  }

  resize(sqrt(next_id));
}
