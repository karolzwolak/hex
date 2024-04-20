#include "board.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#define MAX_LINE_LEN 70

#include <cassert>
int Position::as_index(const int board_size) {
  if (!is_valid(board_size)) {
    return -1;
  }
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
bool Position::is_dest_side(const int board_size, const Player player) {
  int expected_offset = player == RED ? depth % board_size : 0;
  return depth >= board_size && depth < 2 * (board_size - 1) &&
         offset == expected_offset;
}

bool Position::is_start_side(const int board_size, const Player player) {
  int expected_offset = player == RED ? 0 : depth;
  return depth > 0 && depth + 1 < board_size && offset == expected_offset;
}

Board::Board() : size(MAX_SIZE), cells({}), red_count(0), blue_count(0) {}

Board::Board(const Board &other) {
  size = other.size;
  cells = other.cells;
  red_count = other.red_count;
  blue_count = other.blue_count;
}

void Board::resize(int new_size) { size = new_size; }

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
      cells.push_back(p);
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

int Board::player_count(const Player player) {
  assert(player != NONE);
  return player == RED ? red_count : blue_count;
}

bool Board::is_player_connected(const Player player) {
  assert(player != NONE);
  std::vector<bool> visited(size * size, false);
  std::vector<Position> pos_stack;
  pos_stack.reserve(player_count(player));

  for (int d = 1; d < size; d++) {
    int offset = player == RED ? 0 : d;
    Position pos = Position{d, offset};
    int id = pos.as_index(size);

    if (cells[id] != player) {
      continue;
    }

    visited[id] = true;
    pos_stack.push_back(pos);
  }

  while (!pos_stack.empty()) {
    Position pos = pos_stack.back();
    pos_stack.pop_back();

    Position adj[6];
    pos.neighbors(adj);
    for (int i = 0; i < 6; i++) {
      int id = adj[i].as_index(size);
      if (id == -1 || visited[id] || cells[id] != player) {
        continue;
      }
      if (pos.is_dest_side(size, player)) {
        return true;
      }

      visited[id] = true;
      pos_stack.push_back(adj[i]);
    }
  }

  return false;
}
int Board::player_connection_count(const Player player) {
  assert(player != NONE);
  std::vector<bool> visited(size * size, false);
  std::vector<Position> pos_stack;
  pos_stack.reserve(player_count(player));

  int connections = 0;

  for (int d = 1; d < size; d++) {
    int offset = player == RED ? 0 : d;
    Position pos = Position{d, offset};
    int id = pos.as_index(size);

    if (cells[id] != player) {
      continue;
    }

    visited[id] = true;
    pos_stack.push_back(pos);
  }

  bool new_con = true;
  while (!pos_stack.empty()) {
    Position pos = pos_stack.back();
    pos_stack.pop_back();

    if (pos.is_start_side(size, player)) {
      new_con = true;
    }

    Position adj[6];
    pos.neighbors(adj);
    for (int i = 0; i < 6; i++) {
      int id = adj[i].as_index(size);
      if (id == -1 || visited[id] || cells[id] != player) {
        continue;
      }
      if (pos.is_dest_side(size, player) && new_con) {
        connections++;
        new_con = false;
      }

      visited[id] = true;
      pos_stack.push_back(adj[i]);
    }
  }

  return connections;
}
