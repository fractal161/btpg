#include "board.h"

std::unique_ptr<Board> CreateBoard() {
  return std::make_unique<Board>(Board::Ones);
}

std::unique_ptr<Board> BoardFromBytes(const std::vector<uint8_t>& buf) {
  return std::make_unique<Board>(buf.data());
}

Board BoardCopy(const Board& b) {
  return Board(b);
}
