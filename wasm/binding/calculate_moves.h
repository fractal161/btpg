#pragma once

#include "../tetris/game.h"
#include "../tetris/board.h"
#include "../tetris/position.h"

using MoveMap = std::array<ByteBoard, 4>;
static constexpr uint8_t kNoAdj = 1;
static constexpr uint8_t kHasAdj = 2;

MoveMap CalculateMoves(
    const Board& b, int now_piece, Level level, int adj_frame, const std::array<int, 10>& taps, const Position& premove);
