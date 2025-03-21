#pragma once

#include "../tetris/game.h"
#include "../tetris/board.h"
#include "../tetris/position.h"

using MoveMap = std::array<ByteBoard, 4>;
constexpr uint8_t kNoAdj = 1;
constexpr uint8_t kHasAdjReduced = 2;
constexpr uint8_t kHasAdjNonReduced = 3;

MoveMap CalculateMoves(
    const Board& b, int now_piece, Level level, int adj_frame, const std::array<int, 10>& taps, const Position& premove);
