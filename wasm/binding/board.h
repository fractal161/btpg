#pragma once

#include "../tetris/board.h"

std::unique_ptr<Board> CreateBoard();
std::unique_ptr<Board> BoardFromBytes(const std::vector<uint8_t>& buf);
Board BoardCopy(const Board& b);
