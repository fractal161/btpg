#!/bin/bash
emcc -lembind embind.cpp -o tetris.js --embind-emit-tsd tetris.d.ts -sWASM_BIGINT -sMODULARIZE -sENVIRONMENT=web -sEXPORT_ES6
# -s'EXPORT_NAME="TetrisModule"'
