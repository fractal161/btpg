#include "binding/state.h"
#include "binding/board.h"
#include "binding/frame_sequence.h"

#include <emscripten/bind.h>
#include "emarray.h"

EMSCRIPTEN_BINDINGS(my_module) {
  // board
  emscripten::class_<Board>("Board")
    .constructor(&CreateBoard, emscripten::allow_raw_pointers())
    .class_function("fromBytes", &BoardFromBytes)
    .function("copy", &BoardCopy)
    .function("getArray", &Board::ToByteBoard)
    .function("place", &Board::PlaceInplace)
    .function("clearLines", &Board::ClearLinesInplace)
    .function("numFullLines", &Board::NumFullLines)
    .function("count", &Board::Count)
    .function("setCellFilled", &Board::SetCellFilled)
    .function("setCellEmpty", &Board::SetCellEmpty)
    .function("isCellFilled", &Board::IsCellFilled)
    .function("placementNotation", &Board::PlacementNotation)
    .function("toBytes", &Board::ToByteVector)
    .function("toString", &Board::ToString);

  // state
  DeclareArray<ByteBoard>("ByteBoard");
  DeclareArray<ByteBoard::value_type>("ByteBoard_value");
  DeclareArray<MoveMap>("MoveMap");
  
  DeclareArray<decltype(State::board)>("State_board");
  DeclareArray<decltype(State::board)::value_type>("State_board_value");
  DeclareArray<decltype(State::board)::value_type::value_type>("State_board_value_value");
  DeclareArray<decltype(State::moves)>("State_moves");
  DeclareArray<decltype(State::meta)>("State_meta");
  if constexpr (decltype(State::move_meta)().size() != decltype(State::meta)().size()) {
    DeclareArray<decltype(State::move_meta)>("State_move_meta");
  }
  DeclareArray<decltype(State::meta_int)>("State_meta_int");

  // std::vector<*> is automatically bound as JS list in emarray.h
  emscripten::value_object<MultiState>("MultiState")
    .field("board", &MultiState::board)
    .field("moves", &MultiState::moves)
    .field("meta", &MultiState::meta)
    .field("move_meta", &MultiState::move_meta)
    .field("meta_int", &MultiState::meta_int)
    ;

  emscripten::class_<PossibleMoves>("PossibleMoves");
  emscripten::value_object<StateDetail>("StateDetail")
    .field("state", &StateDetail::state)
    .field("moves", &StateDetail::moves)
    .field("move_map", &StateDetail::move_map)
    ;

  emscripten::value_object<Position>("Position")
    .field("r", &Position::r)
    .field("x", &Position::x)
    .field("y", &Position::y)
    ;
  emscripten::enum_<TapSpeed>("TapSpeed")
    .value("kTap10Hz", kTap10Hz)
    .value("kTap12Hz", kTap12Hz)
    .value("kTap15Hz", kTap15Hz)
    .value("kTap20Hz", kTap20Hz)
    .value("kTap24Hz", kTap24Hz)
    .value("kTap30Hz", kTap30Hz)
    .value("kSlow5", kSlow5)
    ;

  emscripten::function("GetState", &GetState);
  emscripten::function("GetStateAllNextPieces", &GetStateAllNextPieces);

  // frame sequence
  emscripten::value_object<AdjItem>("AdjItem")
    .field("position", &AdjItem::position)
    .field("modes", &AdjItem::modes)
    .field("frame_seq", &AdjItem::frame_seq)
    ;
  emscripten::function("GetBestAdjModes", &GetBestAdjModes);
}
