#include "state.h"

namespace {

StateDetail GetState_(
    const Board& board, int now_piece, int next_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_delay, int aggression_level) {
  State state = {};
  memset(state.board.data(), 0, sizeof(state.board));

  ByteBoard byte_board = board.ToByteBoard();
  int level = GetLevelByLines(lines);
  int count = board.Count();
  int pieces = (lines * 10 + count) / 4;
  Level level_speed = GetLevelSpeed(level);
  bool is_adj = next_piece != -1;
  // board: shape (6, 20, 10) [board, one, initial_move(4)]
  // meta: shape (32,) [now_piece(7), next_piece(7), is_adj(1), hz(7), adj_delay(6), aggro(3), pad(1)]
  // meta_int: shape (2,) [entry, now_piece]
  // moves: shape (14, 20, 10) [board, one, moves(4), adj_moves(4), initial_move(4), nonreduce_moves(4)]
  // move_meta: shape (28,) [speed(4), to_transition(21), (level-18)*0.1, lines*0.01, pieces*0.004]
  auto [moves, move_map] = CalculateMoves(
    board, now_piece, level_speed, adj_delay, kTapTables[static_cast<int>(tap_speed)],
    is_adj ? premove : Position::Invalid);
  {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 10; j++) state.board[0][i][j] = byte_board[i][j];
      for (int j = 0; j < 10; j++) state.board[1][i][j] = 1;
      for (int j = 0; j < 10; j++) state.moves[0][i][j] = byte_board[i][j];
      for (int j = 0; j < 10; j++) state.moves[1][i][j] = 1;
    }
    for (int r = 0; r < 4; r++) {
      for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
          state.moves[2 + r][i][j] = move_map[r][i][j] ? 1 : 0;
          state.moves[6 + r][i][j] = move_map[r][i][j] >= 2;
          state.moves[14 + r][i][j] = move_map[r][i][j] && move_map[r][i][j] != 2 ? 1 : 0;
        }
      }
      memset(state.board.data() + (2 + r), 0, sizeof(state.board[0]));
      memset(state.moves.data() + (10 + r), 0, sizeof(state.moves[0]));
    }
  }
  if (is_adj) {
    auto& pos = premove;
    state.board[2 + pos.r][pos.x][pos.y] = 1;
    state.moves[10 + pos.r][pos.x][pos.y] = 1;
  }

  memset(state.meta.data(), 0, sizeof(state.meta));
  state.meta[0 + now_piece] = 1;
  if (is_adj) {
    state.meta[7 + next_piece] = 1;
    state.meta[14] = 1;
  }

  int state_lines = lines;
  int state_level = GetLevelByLines(state_lines);
  int state_speed = static_cast<int>(GetLevelSpeed(state_level));

  int tap_4 = kTapTables[tap_speed][3];
  int tap_5 = kTapTables[tap_speed][4];
  if (state_speed == 2 && adj_delay >= 20) adj_delay = 61;
  if (state_speed == 3 && adj_delay >= 10) adj_delay = 61;
  if (tap_5 <= 8) { // 30hz
    state.meta[15] = 1;
  } else if (tap_5 <= 11) { // 24hz
    state.meta[16] = 1;
  } else if (tap_5 <= 13) { // 20hz
    state.meta[17] = 1;
  } else if (tap_5 <= 16) { // 15hz
    state.meta[18] = 1;
  } else if (tap_4 <= 9) { // slow 5-tap
    state.meta[19] = 1;
  } else if (tap_5 <= 21) { // 12hz
    state.meta[20] = 1;
  } else { // 10hz
    state.meta[21] = 1;
  }
  if (adj_delay <= 4) {
    state.meta[22] = 1;
  } else if (adj_delay <= 19) {
    state.meta[23] = 1;
  } else if (adj_delay <= 22) {
    state.meta[24] = 1;
  } else if (adj_delay <= 25) {
    state.meta[25] = 1;
  } else if (adj_delay <= 32) {
    state.meta[26] = 1;
  } else {
    state.meta[27] = 1;
  }
  state.meta[28 + aggression_level] = 1;

  state.meta_int[0] = state_lines / 2;
  state.meta_int[1] = now_piece;

  memset(state.move_meta.data(), 0, sizeof(state.move_meta));
  int to_transition = 0;
  state.move_meta[state_speed] = 1;
  to_transition = std::max(1, kLevelSpeedLines[state_speed + 1] - state_lines);
  if (to_transition <= 10) { // 4..13
    state.move_meta[4 + (to_transition - 1)] = 1;
  } else if (to_transition <= 22) { // 14..17
    state.move_meta[14 + (to_transition - 11) / 3] = 1;
  } else if (to_transition <= 40) { // 18..20
    state.move_meta[18 + (to_transition - 22) / 6] = 1;
  } else if (to_transition <= 60) { // 21,22
    state.move_meta[21 + (to_transition - 40) / 10] = 1;
  } else {
    state.move_meta[23] = 1;
  }
  state.move_meta[24] = to_transition * 0.01;
  state.move_meta[25] = (state_level - 18) * 0.1;
  state.move_meta[26] = state_lines * 0.01;
  state.move_meta[27] = pieces * 0.004;

  MultiState ret;
  ret.from_state(std::move(state));
  return {ret, moves, move_map};
}

} // namespace

StateDetail GetState(
    const Board& board, int now_piece, int next_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_delay, int aggression_level) {
  try {
    return GetState_(
        board, now_piece, next_piece, premove,
        lines, tap_speed, adj_delay, aggression_level);
  } catch (std::runtime_error&) {
    return {};
  }
}

MultiState GetStateAllNextPieces(
    const Board& board, int now_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_delay, int aggression_level) {
  MultiState ret = GetState(board, now_piece, 0, premove, lines, tap_speed, adj_delay, aggression_level).state;
  ret.resize(7);
  for (int i = 1; i < 7; i++) {
    memcpy(ret.board[i].data(), ret.board[0].data(), sizeof(State::board));
    memcpy(ret.meta[i].data(), ret.meta[0].data(), sizeof(State::meta));
    memcpy(ret.moves[i].data(), ret.moves[0].data(), sizeof(State::moves));
    memcpy(ret.move_meta[i].data(), ret.move_meta[0].data(), sizeof(State::move_meta));
    memcpy(ret.meta_int[i].data(), ret.meta_int[0].data(), sizeof(State::meta_int));
    ret.meta[i][7] = 0;
    ret.meta[i][7 + i] = 1;
  }
  return ret;
}
