#!/bin/bash
emcc -O2 -lembind embind.cpp -o tetris.js --emit-tsd tetris.d.ts -sWASM_BIGINT -sMODULARIZE -sENVIRONMENT=web -sEXPORT_ES6

emcc -O2 -std=c++20 -lembind tetrisv2.cpp -o tetrisv2.js --emit-tsd tetrisv2.d.ts -sWASM_BIGINT -sENVIRONMENT=web -sSINGLE_FILE
# -s'EXPORT_NAME="TetrisModule"'
