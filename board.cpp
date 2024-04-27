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

Board::Board()
    : size(0), cells({}), red_count(0), blue_count(0), created_visited(false),
      created_moves(false) {}

Board::Board(const Board &other) {
  size = other.size;
  cells = other.cells;
  red_count = other.red_count;
  blue_count = other.blue_count;
}

void Board::create_moves() {
  if (created_moves) {
    return;
  }
  created_moves = true;
  int len = size * size;
  for (int i = 0; i < len; i++) {
    if (cells[i] != NONE) {
      continue;
    }
    if (sensible_move(i)) {
      sensible_moves.push_back(i);
    } else {
      useless_moves.push_back(i);
    }
  }
  for (int id : sensible_moves) {
    useless_moves.push_back(id);
  }
}

void Board::create_visited() {
  if (created_visited) {
    return;
  }
  base_visited.resize(size * size, false);
  created_visited = true;

  red_connected = is_player_connected_with_visited(RED, base_visited);
  blue_connected = is_player_connected_with_visited(BLUE, base_visited);
}

void Board::reset() {
  red_count = 0;
  blue_count = 0;
  created_moves = false;
  sensible_moves.clear();
  useless_moves.clear();

  created_visited = false;
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
      Player p = NONE;

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
  create_visited();
  return player == RED ? red_connected : blue_connected;
}

bool Board::is_player_connected_with_visited(const Player player,
                                             std::vector<bool> &visited) {
  assert(player != NONE);
  if (size == 1) {
    return cells[0] == player;
  }
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

    if (is_player_connected_from_start(player, id, visited, id_stack))
      return true;
  }
  return false;
}

bool Board::is_player_connected_from_start(const Player player, const int id,
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

      visited[n] = true;
      id_stack.push_back(n);
    }
  }

  return false;
}
bool Board::is_player_connected_partial(const Player player, const int id,
                                        std::vector<bool> &visited,
                                        std::vector<bool> &visited_copy,
                                        std::vector<int> &id_stack) {
  // any visited cell is connected to start
  // because we saved visited cells after inital check
  // so, if id does not have a neighbor pawn that is visited
  // it is not connected to start
  // so we return, and do not mark it as visited
  // so it can be visited again in the future

  bool has_visited_neighbor = is_id_start_side(id, player);

  int adj[6];
  int n_count = neighbors(id, adj);
  for (int i = 0; i < n_count; i++) {
    int n = adj[i];
    has_visited_neighbor =
        has_visited_neighbor || (visited[n] && cells[n] == player);

    if (has_visited_neighbor) {
      break;
    }
  }
  if (!has_visited_neighbor) {
    return false;
  }
  int len = size * size;
  visited_copy.reserve(len);
  for (int i = 0; i < size; i++) {
    visited_copy[i] = visited[i];
  }

  return is_player_connected_from_start(player, id, visited, id_stack);
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

  std::vector<bool> visited(size * size, false);

  for (int row = 0; row < size; row++) {
    for (int col = 0; col < size; col++) {
      int id = row * size + col;
      if (cells[id] != player) {
        continue;
      }
      cells[id] = NONE;

      visited.assign(visited.size(), false);
      bool is_connected = is_player_connected_with_visited(player, visited);

      cells[id] = player;

      if (!is_connected) {
        player_count++;
        return true;
      }
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
bool Board::sensible_move(const int id) { return has_neighbor(id); }

int &Board::curr_player_count() {
  return curr_turn() == RED ? red_count : blue_count;
}

bool Board::can_player_win_in_one_move_p_turn(std::vector<bool> &visited,
                                              const Player player) {
  std::vector<int> id_stack;

  for (int id : sensible_moves) {
    if (cells[id] != NONE) {
      continue;
    }

    cells[id] = player;

    std::vector<bool> visited_copy;

    bool player_won = is_player_connected_partial(player, id, visited,
                                                  visited_copy, id_stack);

    cells[id] = NONE;

    if (player_won) {
      return true;
    }
  }
  return false;
}
bool Board::can_player_win_in_one_move_op_turn(std::vector<bool> &visited,
                                               const Player player,
                                               bool perfect_op) {
  std::vector<int> id_stack;

  Player opponent = opposite_player(player);
  std::vector<int> &op_move_positions =
      perfect_op ? sensible_moves : useless_moves;

  for (int id : op_move_positions) {
    if (cells[id] != NONE) {
      continue;
    }
    cells[id] = opponent;

    std::vector<bool> visited_copy;

    bool opponent_won = is_player_connected_partial(opponent, id, visited,
                                                    visited_copy, id_stack);

    if (!opponent_won && visited_copy.empty()) {
      visited_copy = visited;
    }
    bool player_wins = !opponent_won &&
                       can_player_win_in_one_move_p_turn(visited_copy, player);

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

  bool player_won = is_player_connected(player);
  bool op_won = is_player_connected(opponent);

  if (all_occupied || player_won || op_won) {
    return false;
  }

  create_moves();

  int &curr_turn_count = curr_player_count();
  curr_turn_count++;
  bool can_win;
  if (turn == player) {
    can_win = can_player_win_in_one_move_p_turn(base_visited, player);
  } else {
    can_win =
        can_player_win_in_one_move_op_turn(base_visited, player, perfect_op);
  }
  curr_turn_count--;

  return can_win;
}
bool Board::can_player_win_in_two_moves_p_turn(std::vector<bool> &visited,
                                               const Player player,
                                               bool perfect_op) {
  std::vector<int> id_stack;

  for (int id : sensible_moves) {
    if (cells[id] != NONE) {
      continue;
    }
    cells[id] = player;

    std::vector<bool> visited_copy;

    bool won = is_player_connected_partial(player, id, visited, visited_copy,
                                           id_stack);
    if (!won && visited_copy.empty())
      visited_copy = visited;

    bool can_win = !won && can_player_win_in_one_move_op_turn(
                               visited_copy, player, perfect_op);

    cells[id] = NONE;

    if (can_win) {
      return true;
    }
  }
  return false;
}
bool Board::can_player_win_in_two_moves_op_turn(std::vector<bool> &visited,
                                                const Player player,
                                                bool perfect_op) {
  Player opponent = opposite_player(player);
  std::vector<int> id_stack;
  std::vector<int> &op_move_positions =
      perfect_op ? sensible_moves : useless_moves;

  for (int id : op_move_positions) {
    if (cells[id] != NONE) {
      continue;
    }
    cells[id] = opponent;

    std::vector<bool> visited_copy;

    bool opponent_won = is_player_connected_partial(opponent, id, visited,
                                                    visited_copy, id_stack);
    if (!opponent_won && visited_copy.empty())
      visited_copy = visited;

    bool player_can_win =
        !opponent_won &&
        can_player_win_in_two_moves_p_turn(visited_copy, player, perfect_op);

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

  bool player_won = is_player_connected(player);
  bool op_won = is_player_connected(opponent);

  if (all_occupied || player_won || op_won) {
    return false;
  }

  create_moves();

  int &curr_turn_count = curr_player_count();
  curr_turn_count++;
  bool can_win;

  if (turn == player) {
    can_win =
        can_player_win_in_two_moves_p_turn(base_visited, player, perfect_op);
  } else {
    can_win =
        can_player_win_in_two_moves_op_turn(base_visited, player, perfect_op);
  }
  curr_turn_count--;

  return can_win;
}
