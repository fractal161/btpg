#include "board.h"

std::unique_ptr<Board> CreateBoard() {
  return std::make_unique<Board>(Board::Ones);
}