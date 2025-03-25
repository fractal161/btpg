#pragma once

#include "../tetris/frame_sequence.h"
#include "state.h"

struct AdjItem {
  std::string modes;
  std::vector<std::string> frame_seq;
  Position position;
};

std::vector<AdjItem> GetBestAdjModes(
    const Board& board, int now_piece,
    int lines, TapSpeed tap_speed, int adj_delay,
    const PossibleMoves& moves, const std::vector<Position>& adjs);
