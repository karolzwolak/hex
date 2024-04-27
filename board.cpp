#include "board.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#define MAX_LINE_LEN 70

Player opposite_player(Player player) {
  if (player == RED) {
    return BLUE;
  }
  if (player == BLUE) {
    return RED;
  }
  return NONE;
}

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
bool Board::is_player_connected(const Player player) {
  assert(player != NONE);
  if (size == 1) {
    return cells[0] == player;
  }
  std::vector<bool> visited(size * size, false);
  std::vector<int> id_stack;
  id_stack.reserve(player_count(player));

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

    if (is_player_connected_at(player, id, visited, id_stack))
      return true;
  }
  return false;
}

bool Board::is_player_connected_at(const Player player, const int id,
                                   std::vector<bool> &visited,
                                   std::vector<int> &id_stack) {
  visited[id] = true;
  id_stack.push_back(id);

  while (!id_stack.empty()) {
    int id = id_stack.back();
    id_stack.pop_back();

    if (is_id_dest_side(id, player)) {
      return true;
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
  bool red_won = is_player_connected(RED);
  bool blue_won = is_player_connected(BLUE);

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

bool Board::is_victory_legal(const Player player) {
  assert(player != NONE);
  int &player_count = player == RED ? red_count : blue_count;
  player_count--;

  for (int row = 0; row < size; row++) {
    for (int col = 0; col < size; col++) {
      int id = row * size + col;
      if (cells[id] != player) {
        continue;
      }
      cells[id] = NONE;

      if (!is_player_connected(player)) {
        cells[id] = player;
        player_count++;
        return true;
      }

      cells[id] = player;
    }
  }
  player_count++;
  return false;
}

bool Board::is_board_possible() {
  if (!is_board_correct()) {
    return false;
  }
  if (red_count + blue_count == 1 && red_count != 1) {
    return false;
  }
  bool red_won = is_player_connected(RED);
  bool blue_won = is_player_connected(BLUE);

  if (!red_won && !blue_won) {
    return true;
  }

  if (red_won && blue_won)
    return false;

  if (red_won && !is_victory_legal(RED))
    return false;
  if (blue_won && !is_victory_legal(BLUE))
    return false;

  if (red_won && red_count != blue_count + 1) {
    return false;
  }
  if (blue_won && red_count != blue_count) {
    return false;
  }

  return true;
}
bool Board::has_neighbor(const int id) {
  int row = id / size;
  int col = id % size;
  bool is_edge = row == 0 || col == 0 || row == size - 1 || col == size - 1;
  if (is_edge) {
    return true;
  }

  int neighbor_arr[6];
  int n_count = neighbors(id, neighbor_arr);
  for (int i = 0; i < n_count; i++) {
    int n_id = neighbor_arr[i];

    if (cells[n_id] != NONE) {
      return true;
    }
  }

  return false;
}
bool Board::skip_move(const int id) {
  return cells[id] != NONE || !has_neighbor(id);
}
int &Board::curr_player_count() {
  return curr_turn() == RED ? red_count : blue_count;
}

void Board::move_positions(std::vector<int> &positions) {
  int len = size * size;

  for (int i = 0; i < len; i++) {
    if (skip_move(i)) {
      continue;
    }
    positions.push_back(i);
  }
}

bool Board::can_player_win_in_one_move_p_turn(std::vector<int> &positions,
                                              const Player player) {
  for (int id : positions) {
    if (cells[id] != NONE) {
      continue;
    }

    cells[id] = player;

    bool player_won = is_player_connected(player);

    cells[id] = NONE;

    if (player_won) {
      return true;
    }
  }
  return false;
}
bool Board::can_player_win_in_one_move_op_turn(std::vector<int> &positions,
                                               const Player player,
                                               bool perfect_op) {
  Player opponent = opposite_player(player);

  for (int id : positions) {
    if (cells[id] != NONE) {
      continue;
    }
    cells[id] = opponent;

    bool opponent_won = is_player_connected(opponent);
    bool player_wins =
        !opponent_won && can_player_win_in_one_move_p_turn(positions, player);

    cells[id] = NONE;

    if (perfect_op && !player_wins) {
      return false;
    }
    if (!perfect_op && player_wins) {
      return true;
    }
  }
  if (perfect_op) {
    return true;
  }
  return false;
}

bool Board::can_player_win_in_one_move(const Player player, bool perfect_op) {
  assert(player != NONE);
  Player turn = curr_turn();

  bool all_occupied = red_count + blue_count == size * size;
  Player opponent = opposite_player(player);

  if (all_occupied || is_player_connected(opponent) ||
      is_player_connected(player)) {
    return false;
  }

  std::vector<int> positions;
  move_positions(positions);

  int &curr_turn_count = curr_player_count();
  curr_turn_count++;
  bool can_win;
  if (turn == player) {
    can_win = can_player_win_in_one_move_p_turn(positions, player);
  } else {
    can_win = can_player_win_in_one_move_op_turn(positions, player, perfect_op);
  }
  curr_turn_count--;

  return can_win;
}
bool Board::can_player_win_in_two_moves_p_turn(std::vector<int> &positions,
                                               const Player player,
                                               bool perfect_op) {
  for (int id : positions) {
    if (cells[id] != NONE) {
      continue;
    }
    cells[id] = player;

    bool can_win =
        !is_player_connected(player) &&
        can_player_win_in_one_move_op_turn(positions, player, perfect_op);

    cells[id] = NONE;

    if (can_win) {
      return true;
    }
  }
  return false;
}
bool Board::can_player_win_in_two_moves_op_turn(std::vector<int> &positions,
                                                const Player player,
                                                bool perfect_op) {
  Player opponent = opposite_player(player);

  for (int id : positions) {
    if (cells[id] != NONE) {
      continue;
    }
    cells[id] = opponent;

    bool opponent_won = is_player_connected(opponent);
    bool player_can_win =
        !opponent_won && !is_player_connected(player) &&
        can_player_win_in_two_moves_p_turn(positions, player, perfect_op);

    cells[id] = NONE;

    if (perfect_op && !player_can_win) {
      return false;
    }
    if (!perfect_op && player_can_win) {
      return true;
    }
  }
  if (perfect_op) {
    return true;
  }
  return false;
}
bool Board::can_player_win_in_two_moves(const Player player, bool perfect_op) {
  assert(player != NONE);
  Player turn = curr_turn();

  bool all_occupied = red_count + blue_count == size * size;
  Player opponent = opposite_player(player);

  if (all_occupied || is_player_connected(opponent) ||
      is_player_connected(player)) {
    return false;
  }

  std::vector<int> positions;
  move_positions(positions);

  int &curr_turn_count = curr_player_count();
  curr_turn_count++;
  bool can_win;
  if (turn == player) {
    can_win = can_player_win_in_two_moves_p_turn(positions, player, perfect_op);
  } else {
    can_win =
        can_player_win_in_two_moves_op_turn(positions, player, perfect_op);
  }
  curr_turn_count--;

  return can_win;
}
