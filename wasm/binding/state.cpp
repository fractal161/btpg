#include "state.h"

#include "calculate_moves.h"

MultiState GetState(
    const ByteBoard& byte_board, int now_piece, int next_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_frame) {
  State state = {};
  memset(state.board.data(), 0, sizeof(state.board));

  Board board(byte_board);
  int level = GetLevelByLines(lines);
  int count = board.Count();
  int pieces = (lines * 10 + count) / 4;
  Level level_speed = GetLevelSpeed(level);
  bool is_adj = next_piece != -1;
  // board: shape (6, 20, 10) [board, one, initial_move(4)]
  // meta: shape (28,) [group(5), now_piece(7), next_piece(7), is_adj(1), hz(4), adj(4)]
  // meta_int: shape (2,) [entry, now_piece]
  // moves: shape (14, 20, 10) [board, one, moves(4), adj_moves(4), initial_move(4)]
  // move_meta: shape (28,) [speed(4), to_transition(21), (level-18)*0.1, lines*0.01, pieces*0.004]
  {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 10; j++) state.board[0][i][j] = byte_board[i][j];
      for (int j = 0; j < 10; j++) state.board[1][i][j] = 1;
      for (int j = 0; j < 10; j++) state.moves[0][i][j] = byte_board[i][j];
      for (int j = 0; j < 10; j++) state.moves[1][i][j] = 1;
    }
    auto move_map = CalculateMoves(
        board, now_piece, level_speed, adj_frame, kTapTables[static_cast<int>(tap_speed)],
        is_adj ? premove : Position::Invalid);
    for (int r = 0; r < 4; r++) {
      for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) state.moves[2 + r][i][j] = move_map[r][i][j] ? 1 : 0;
        for (int j = 0; j < 10; j++) state.moves[6 + r][i][j] = move_map[r][i][j] == 2;
      }
    }
  }
  if (is_adj) {
    state.board[2 + premove.r][premove.x][premove.y] = 1;
    state.moves[10 + premove.r][premove.x][premove.y] = 1;
  }

  state.meta[0 + board.Count() / 2 % 5] = 1;
  state.meta[5 + now_piece] = 1;
  if (is_adj) {
    state.meta[12 + next_piece] = 1;
    state.meta[19] = 1;
  }
  state.meta[20] = 1; // hardcode now; modify if extended
  state.meta[24] = 1;

  state.meta_int[0] = lines / 2;
  state.meta_int[1] = now_piece;

  int to_transition = 0;
  state.move_meta[level_speed] = 1;
  to_transition = std::max(1, kLevelSpeedLines[level_speed + 1] - lines);
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
  state.move_meta[25] = (level - 18) * 0.1;
  state.move_meta[26] = lines * 0.01;
  state.move_meta[27] = pieces * 0.004;

  MultiState ret;
  ret.from_state(std::move(state));
  return ret;
}

MultiState GetStateAllNextPieces(
    const ByteBoard& byte_board, int now_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_frame) {
  MultiState ret = GetState(byte_board, now_piece, 0, premove, lines, tap_speed, adj_frame);
  ret.resize(7);
  for (int i = 1; i < 7; i++) {
    memcpy(ret.board[i].data(), ret.board[0].data(), sizeof(State::board));
    memcpy(ret.meta[i].data(), ret.meta[0].data(), sizeof(State::meta));
    memcpy(ret.moves[i].data(), ret.moves[0].data(), sizeof(State::moves));
    memcpy(ret.move_meta[i].data(), ret.move_meta[0].data(), sizeof(State::move_meta));
    memcpy(ret.meta_int[i].data(), ret.meta_int[0].data(), sizeof(State::meta_int));
    ret.meta[i][12] = 0;
    ret.meta[i][12 + i] = 1;
  }
  return ret;
}
