# btpg

A frontend for using the [BetaTetris](https://github.com/adrien1018/beta-tetris/blob/left-well/README.md) AI in the browser, through the magic of the [ONNX Runtime](https://onnxruntime.ai/).

## Setup

Clone the repository, then run `npm install` in the root directory. To compile the wasm modules, navigate to the `wasm` directory and run `./build.sh`. This will require Emscripten, which can be installed following the instructions [here](https://emscripten.org/docs/getting_started/downloads.html).

From here, simply run the server in dev mode with `npm run dev`.

To use any of the python scripts, run `pip install -r requirements.txt`. I recommend you do this in a virtual environment.

## Acknowledgements

Adrien Wu for creating BetaTetris.
