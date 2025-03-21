#pragma once

#include "../tetris/board.h"
#include "../tetris/position.h"

enum TapSpeed {
  kTap10Hz,
  kTap12Hz,
  kTap15Hz,
  kTap20Hz,
  kTap24Hz,
  kTap30Hz,
  kSlow5
};
constexpr std::array<int, 10> kTapTables[] = { // match TapSpeed
  {0, 6, 12, 18, 24, 30, 36, 42, 48, 54},
  {0, 5, 10, 15, 20, 25, 30, 35, 40, 45},
  {0, 4, 8, 12, 16, 20, 24, 28, 32, 36},
  {0, 3, 6, 9, 12, 15, 18, 21, 24, 27},
  {0, 3, 5, 8, 10, 13, 15, 18, 20, 23},
  {0, 2, 4, 6, 8, 10, 12, 14, 16, 18},
  {0, 2, 4, 6, 18, 20, 22, 24, 36, 38}
};

struct State {
  std::array<std::array<std::array<float, 10>, 20>, 6> board;
  std::array<float, 32> meta;
  std::array<std::array<std::array<float, 10>, 20>, 18> moves;
  std::array<float, 28> move_meta;
  std::array<int, 2> meta_int;
};

struct MultiState {
  std::vector<decltype(State::board)> board;
  std::vector<decltype(State::meta)> meta;
  std::vector<decltype(State::moves)> moves;
  std::vector<decltype(State::move_meta)> move_meta;
  std::vector<decltype(State::meta_int)> meta_int;

  void resize(size_t size) {
    board.resize(size);
    meta.resize(size);
    moves.resize(size);
    move_meta.resize(size);
    meta_int.resize(size);
  }

  void from_state(State&& state) {
    board.push_back(std::move(state.board));
    meta.push_back(std::move(state.meta));
    moves.push_back(std::move(state.moves));
    move_meta.push_back(std::move(state.move_meta));
    meta_int.push_back(std::move(state.meta_int));
  }
};

// next_piece == -1 for pre-adj
MultiState GetState(
    const ByteBoard& byte_board, int now_piece, int next_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_delay, int aggression_level);

MultiState GetStateAllNextPieces(
    const ByteBoard& byte_board, int now_piece, const Position& premove,
    int lines, TapSpeed tap_speed, int adj_delay, int aggression_level);
