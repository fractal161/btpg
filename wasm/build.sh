#!/bin/bash
emcc -O2 -std=c++20 -fexceptions -sWASM_BIGINT -sENVIRONMENT=web -sEXPORT_ES6 \
    -o tetris.js --emit-tsd tetris.d.ts \
    tetris.cpp binding/state.cpp binding/calculate_moves.cpp tetris/frame_sequence.cpp -lembind
# -s'EXPORT_NAME="TetrisModule"'
