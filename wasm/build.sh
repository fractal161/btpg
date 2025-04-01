#!/bin/bash
emcc -O2 -std=c++20 -fexceptions -sALLOW_MEMORY_GROWTH -sWASM_BIGINT -sENVIRONMENT=web -sEXPORT_ES6 \
    -o tetris.js --emit-tsd tetris.d.ts --emit-symbol-map \
    tetris.cpp binding/*.cpp tetris/frame_sequence.cpp -lembind

# emcc -O2 -std=c++20 -fexceptions -sALLOW_MEMORY_GROWTH -sWASM_BIGINT -sENVIRONMENT=web -sSINGLE_FILE \
#     -o tetris-single.js \
#     tetris.cpp binding/*.cpp tetris/frame_sequence.cpp -lembind

# -s'EXPORT_NAME="TetrisModule"'
