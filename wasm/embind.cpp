#include <emscripten/bind.h>
#include "emarray.h"

#include "tetris.cpp"

using namespace emscripten;

using FieldRow = std::array<bool, Tetris::kM>;

using StateRow = std::array<float, Tetris::kM>;
using StateBoard = std::array<StateRow, Tetris::kN>;

EMSCRIPTEN_BINDINGS(tetris) {
  value_object<std::pair<int, int>>("pii")
    .field("first", &std::pair<int, int>::first)
    .field("second", &std::pair<int, int>::second)
    ;
  //register_vector<int>("vector<int>");
  //register_array<bool, 20>("array<bool, 20>");
  DeclareArray<FieldRow>("FieldRow");
  DeclareArray<Tetris::Field>("Field");
  DeclareArray<StateRow>("StateRow");
  DeclareArray<StateBoard>("StateBoard");
  DeclareArray<Tetris::State>("State");

  class_<Tetris>("Tetris")
    .constructor<unsigned int>()
    //.property("x", &MyClass::x)
    //.property("x", &MyClass::getX, &MyClass::setX)
    //.property("x_readonly", &MyClass::getX)
    //.function("Reseed", &Tetris::Reseed)
    //.function("ResetGame", &Tetris::ResetGame)
    //.function("ResetRandom", &Tetris::ResetRandom)
    //.function("IsOver", &Tetris::IsOver)
    //.function("GetScore", &Tetris::GetScore)
    //.function("GetLines", &Tetris::GetLines)
    //.function("GetPlaceStage", &Tetris::GetPlaceStage)
    //.function("GetTetrisStat", &Tetris::GetTetrisStat)
    //.function("GetNextPieceDistribution", &Tetris::GetNextPieceDistribution, allow_raw_pointer<int>())
    .function("GetState", &Tetris::GetState)
    //.function("GetPlannedSequence", &Tetris::GetPlannedSequence)
    //.function("GetMicroadjSequence", &Tetris::GetMicroadjSequence)
    //.function("InputPlacement", &Tetris::InputPlacement)
    //.function("TrainingSetPlacement", &Tetris::TrainingSetPlacement)
    //.function("SetNowPiece", &Tetris::SetNowPiece)
    //.function("SetNextPiece", &Tetris::SetNextPiece)
    //.function("SetPreviousPlacement", &Tetris::SetPreviousPlacement)
    .function("SetState", &Tetris::SetState)
    //.function("GetPlacements", &Tetris::GetPlacements)
    //.class_function("IsAB", &Tetris::IsAB)
    //.class_function("PlaceField", &Tetris::PlaceField)
    //.class_function("GetMoveSequence", &Tetris::GetMoveSequence)
    ;
  //enum_<Tetris::MoveType>("MoveType")
  //  .value("A", Tetris::MoveType::kA)
  //  .value("B", Tetris::MoveType::kB)
  //  .value("L", Tetris::MoveType::kL)
  //  .value("R", Tetris::MoveType::kR)
  //  ;
  //value_object<Tetris::Move>("Move")
  //  .field("height_start", &Tetris::Move::height_start)
  //  .field("height_end", &Tetris::Move::height_end)
  //  .field("type", &Tetris::Move::type)
  //  ;
  //value_object<Tetris::MoveSequence>("MoveSequence")
  //  .field("valid", &Tetris::MoveSequence::valid)
  //  .field("moves", &Tetris::MoveSequence::moves)
  //  ;
  //value_object<Tetris::FrameInput>("FrameInput")
  //  .field("l", &Tetris::FrameInput::l)
  //  .field("r", &Tetris::FrameInput::r)
  //  .field("a", &Tetris::FrameInput::a)
  //  .field("b", &Tetris::FrameInput::b)
  //  ;
  //value_object<Tetris::FrameSequence>("FrameSequence")
  //  .field("seq", &Tetris::FrameSequence::seq)
  //  ;
  value_object<Tetris::Position>("Position")
    .field("rotate", &Tetris::Position::rotate)
    .field("x", &Tetris::Position::x)
    .field("y", &Tetris::Position::y)
    ;
}
