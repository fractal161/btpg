#!/bin/bash
emcc -lembind embind.cpp -o tetris.js --embind-emit-tsd ../src/tetris.d.ts -o tetris.html -sWASM_BIGINT -o tetris.html
# -s 'EXPORT_NAME="Example"'
