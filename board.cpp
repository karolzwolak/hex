#include "board.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#define MAX_LINE_LEN 70

Board::Board() : size(0), cells({}), red_count(0), blue_count(0) {}

Board::Board(const Board &other) {
  size = other.size;
  cells = other.cells;
  red_count = other.red_count;
  blue_count = other.blue_count;
}

void Board::reset() {
  red_count = 0;
  blue_count = 0;
}

// assumes first line "---" is consumed
void Board::parse_from_stdin() {
  reset();

  std::vector<Player> new_cells;
  char buffer[MAX_LINE_LEN];

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
      new_cells.push_back(p);
      cell_count++;
    }

    std::cin.getline(buffer, sizeof(buffer));
    if (new_cells.size() > 1 && cell_count == 1) {
      break;
    }
  }

  cells.resize(new_cells.size());
  size = sqrt(new_cells.size());

  int max_depth = 2 * (size - 1);
  int id = 0;
  for (int depth = 0; depth <= max_depth; depth++) {
    int max_offset = depth + 1 >= size ? max_depth - depth : depth;

    int flat_id;
    if (depth >= size) {
      flat_id = size * (size - 1) + (depth - size + 1);
    } else {
      flat_id = depth * size;
    }

    for (int offset = 0; offset <= max_offset; offset++) {
      cells[flat_id] = new_cells[id];

      flat_id -= size - 1;
      id++;
    }
  }
}

int Board::player_count(const Player player) {
  assert(player != NONE);
  return player == RED ? red_count : blue_count;
}
int Board::is_id_dest_side(const int id, const Player player) {
  int col = id % size;
  int row = id / size;

  if (player == RED) {
    return col == size - 1;
  }
  return row == size - 1;
}
int Board::is_id_start_side(const int id, const Player player) {
  int col = id % size;
  int row = id / size;
  if (player == RED) {
    return col == 0;
  }
  return row == 0;
}
bool Board::is_pos_valid(const int row, const int col) {
  return row >= 0 && row < size && col >= 0 && col < size;
}
int Board::neighbors(const int id, int (&arr_out)[6]) {
  int count = 0;
  int col = id % size;
  int row = id / size;
  if (is_pos_valid(row, col + 1)) {
    arr_out[count] = id + 1;
    count++;
  }
  if (is_pos_valid(row, col - 1)) {
    arr_out[count] = id - 1;
    count++;
  }
  if (is_pos_valid(row + 1, col)) {
    arr_out[count] = id + size;
    count++;
  }
  if (is_pos_valid(row - 1, col)) {
    arr_out[count] = id - size;
    count++;
  }
  if (is_pos_valid(row + 1, col + 1)) {
    arr_out[count] = id + size + 1;
    count++;
  }
  if (is_pos_valid(row - 1, col - 1)) {
    arr_out[count] = id - size - 1;
    count++;
  }
  return count;
}
int Board::player_connected_count(const Player player) {
  assert(player != NONE);
  if (size == 1) {
    return cells[0] == player;
  }
  std::vector<bool> visited(size * size, false);
  std::vector<int> id_stack;
  id_stack.reserve(player_count(player));

  int connections = 0;

  for (int i = 0; i < size; i++) {
    int row, col;
    if (player == RED) {
      col = 0;
      row = i;
    } else {
      col = i;
      row = 0;
    }
    int id = row * size + col;

    if (cells[id] != player) {
      continue;
    }

    connections += player_connected_at(player, id, visited, id_stack);
  }
  return connections;
}

bool Board::player_connected_at(const Player player, const int id,
                                std::vector<bool> &visited,
                                std::vector<int> &id_stack) {
  visited[id] = true;
  id_stack.push_back(id);
  bool connected = false;

  while (!id_stack.empty()) {
    int id = id_stack.back();
    id_stack.pop_back();

    if (is_id_dest_side(id, player)) {
      connected = true;
    }

    int adj[6];
    int n_count = neighbors(id, adj);
    for (int i = 0; i < n_count; i++) {
      int n = adj[i];
      if (visited[n] || cells[n] != player) {
        continue;
      }

      visited[id] = true;
      id_stack.push_back(n);
    }
  }

  return connected;
}
bool Board::check_connected_cycle_at_id(const Player player, const int id,
                                        const int parent, bool connected,
                                        std::vector<bool> &visited) {
  visited[id] = true;

  if (is_id_dest_side(id, player)) {
    connected = true;
  }

  int adj[6];
  int n_count = neighbors(id, adj);
  for (int i = 0; i < n_count; i++) {
    int n = adj[i];
    if (cells[n] != player) {
      continue;
    }
    bool res = false;
    if (!visited[n]) {
      res = check_connected_cycle_at_id(player, n, id, connected, visited);
    } else if (n != parent && connected && is_id_start_side(n, player)) {
      res = true;
    }
    if (res) {
      return true;
    }
  }
  return false;
}
bool Board::check_connected_cycle(const Player player) {
  assert(player != NONE);
  if (size == 1) {
    return cells[0] == player;
  }

  std::vector<bool> visited(size * size, false);
  std::vector<int> pos_stack;

  pos_stack.reserve(player_count(player));
  assert(size > 1);

  for (int i = 0; i < size; i++) {
    int row, col;
    if (player == RED) {
      col = 0;
      row = i;
    } else {
      col = i;
      row = 0;
    }
    int id = row * size + col;

    if (cells[id] != player || visited[id]) {
      continue;
    }

    if (check_connected_cycle_at_id(player, id, -1, false, visited)) {
      return true;
    }
  }
  return false;
}

Player Board::curr_turn() {
  if (red_count == blue_count || blue_count > red_count) {
    return RED;
  }
  return BLUE;
}

bool Board::is_board_correct() {
  // because red starts, blue can't have more pawns than red
  return red_count == blue_count || blue_count + 1 == red_count;
}

Player Board::winner() {
  if (!is_board_correct()) {
    return NONE;
  }
  bool red_won = player_connected_count(RED) > 0;
  bool blue_won = player_connected_count(BLUE) > 0;

  if (red_won && blue_won) {
    return NONE;
  }
  if (red_won) {
    return RED;
  }
  if (blue_won) {
    return BLUE;
  }
  return NONE;
}

bool Board::is_board_possible() {
  if (!is_board_correct()) {
    return false;
  }
  bool red_cycle = check_connected_cycle(RED);
  bool blue_cycle = check_connected_cycle(BLUE);

  int red_con = player_connected_count(RED);
  int blue_con = player_connected_count(BLUE);
  /* std::cout << "red_con: " << red_con << " blue_con: " << blue_con << "\n";
   */
  /* std::cout << "red_cycle: " << red_cycle << " blue_cycle: " << blue_cycle */
  /*           << "\n"; */

  return red_con + blue_con <= 1 && !red_cycle && !blue_cycle;
}
