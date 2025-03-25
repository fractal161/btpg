// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// see also advanced usage of importing ONNX Runtime Web:
// https://github.com/microsoft/onnxruntime-inference-examples/tree/main/js/importing_onnxruntime-web

import { InferenceSession, Tensor, env } from 'onnxruntime-web/all';
import onnxFile from '../../agents/model.onnx';
import { Model } from '../model';
import { module, Placement, TetrisState } from '../tetris';
import { Parameters } from '../params';

env.wasm.wasmPaths = '/btpg/';

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

export class ExampleModel implements Model {
    private constructor(private session: InferenceSession) {}

    public static create = async (): Promise<ExampleModel> => {
        try {
            const session = await InferenceSession.create(onnxFile, {executionProviders: ['webgpu']});
            return new ExampleModel(session);
        } catch (e) {
            // TODO: change to display a message in the UI
            console.info('WebGPU not supported; falling back to WebAssembly');
        }
        try {
            const session = await InferenceSession.create(onnxFile, {executionProviders: ['wasm']});
            return new ExampleModel(session);
        } catch (e) {
            console.error('Cannot initialize model');
            throw e;
        }
    };

    public run = async (tetris: TetrisState, params: Parameters): Promise<Placement> => {
        if (this.session === undefined) {
            throw Error("session isn't ready!");
        }

        console.log(tetris.board.toString(false, true, true));
        const state = module.GetState(
            tetris.board,
            params.piece, // current piece
            -1,
            {r: 0, x: 0, y: 0}, // position
            params.lines,
            module.TapSpeed.kTap30Hz,
            18,
            0);

        // prepare feeds. use model input names as keys.
        const feeds = {
            board: createONNXTensor(state.board),
            meta: createONNXTensor(state.meta),
            moves: createONNXTensor(state.moves),
            move_meta: createONNXTensor(state.move_meta),
            meta_int: createONNXTensor(state.meta_int, 'int32'),
        };
        console.log(feeds);

        // feed inputs and run
        const results = await this.session.run(feeds);
        const pi = results.pi.data;
        const pi_rank = results.pi_rank.data;
        const v = results.v.data;
        console.log(pi, pi_rank, v);
        const best = Number(pi_rank[0]);

        return { rot: ~~(best / 200), x: ~~(best / 10) % 20, y: best % 10 };
    };
}
