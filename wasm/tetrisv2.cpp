#include "binding/state.h"

#include <emscripten/bind.h>
#include "emarray.h"

EMSCRIPTEN_BINDINGS(my_module) {
  DeclareArray<ByteBoard>("ByteBoard");
  DeclareArray<ByteBoard::value_type>("ByteBoard_value");
  
  DeclareArray<decltype(State::board)>("State_board");
  DeclareArray<decltype(State::board)::value_type>("State_board_value");
  DeclareArray<decltype(State::board)::value_type::value_type>("State_board_value_value");
  DeclareArray<decltype(State::moves)>("State_moves");
  DeclareArray<decltype(State::meta)>("State_meta");
  //DeclareArray<decltype(State::move_meta)>("State_move_meta");
  DeclareArray<decltype(State::meta_int)>("State_meta_int");

  // std::vector<*> is automatically bound as JS list in emarray.h

  emscripten::value_object<MultiState>("MultiState")
      .field("board", &MultiState::board)
      .field("moves", &MultiState::moves)
      .field("meta", &MultiState::meta)
      .field("move_meta", &MultiState::move_meta)
      .field("meta_int", &MultiState::meta_int)
      ;

  emscripten::value_object<Position>("Position")
      .field("r", &Position::r)
      .field("x", &Position::x)
      .field("y", &Position::y)
      ;
  emscripten::enum_<TapSpeed>("TapSpeed")
      .value("kTap12Hz", kTap12Hz)
      .value("kTap15Hz", kTap15Hz)
      .value("kTap20Hz", kTap20Hz)
      .value("kTap24Hz", kTap24Hz)
      .value("kTap30Hz", kTap30Hz)
      ;

  emscripten::function("GetState", &GetState);
  emscripten::function("GetStateAllNextPieces", &GetStateAllNextPieces);
}
