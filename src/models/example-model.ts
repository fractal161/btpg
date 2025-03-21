// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// see also advanced usage of importing ONNX Runtime Web:
// https://github.com/microsoft/onnxruntime-inference-examples/tree/main/js/importing_onnxruntime-web

import { InferenceSession, Tensor } from 'onnxruntime-web';
import onnxFile from '../../agents/model.onnx';
import { Model } from '../model';
import { Placement } from '../tetris';
import Module from '../../wasm/tetris.js';

function calculateShape(nestedArray: Array<any>) {
    const shape = [];
    let currentArray = nestedArray;
    // Traverse the nested array structure to determine its shape
    while (Array.isArray(currentArray)) {
        shape.push(currentArray.length);
        currentArray = currentArray[0];  // Move one level deeper
    }
    return shape;
}

function createONNXTensor(nestedArray: Array<any>, dataType = 'float32') {
    const flattenedArray = nestedArray.flat(100);
    const shape = calculateShape(nestedArray);
    return new Tensor(dataType as any, flattenedArray, shape);
}

function argmax(a) {
    return a.reduce((iMax, x, i, arr) => x > arr[iMax] ? i : iMax, 0);
}

export class ExampleModel implements Model {
    private constructor(private session: InferenceSession) {}

    public static create = async (): Promise<ExampleModel> => {
        const session = await InferenceSession.create(onnxFile);
        return new ExampleModel(session);
    };

    public run = async (): Promise<Placement> => {
        if (this.session === undefined) {
            throw Error("session isn't ready!");
        }

        const module = await Module();
        const board = Array.from({length:20}, () => Array(10).fill(1));
        const state = module.GetState(
            board,
            0,
            -1,
            {r: 0, x: 0, y: 0}, // position
            0, // lines
            module.TapSpeed.kTap30Hz,
            18,
            0);

        // prepare feeds. use model input names as keys.
        console.log(state.moves);
        const feeds = {
            [this.session.inputNames[0]]: createONNXTensor(state.board),
            [this.session.inputNames[1]]: createONNXTensor(state.meta),
            [this.session.inputNames[2]]: createONNXTensor(state.moves),
            [this.session.inputNames[3]]: createONNXTensor(state.move_meta),
            [this.session.inputNames[4]]: createONNXTensor(state.meta_int, 'int32'),
        };
        console.log(feeds);

        // feed inputs and run
        const results = await this.session.run(feeds);
        const pi = results[this.session.outputNames[0]].data;
        const v = results[this.session.outputNames[1]].data;
        const best = argmax(pi);
        console.log(best, v);

        // step 1: create typed arrays for the input
        // step 2: create tensors
        return { rot: ~~(best / 200), x: ~~(best / 10) % 20, y: best % 10 };
    };
}
