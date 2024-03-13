// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// see also advanced usage of importing ONNX Runtime Web:
// https://github.com/microsoft/onnxruntime-inference-examples/tree/main/js/importing_onnxruntime-web

import { InferenceSession, Tensor, env } from 'onnxruntime-web';
import exampleModel from '../../agents/example.onnx';
import { Model } from '../model';
import { Placement } from '../tetris';

export class ExampleModel implements Model {
    private session: InferenceSession | undefined = undefined;

    constructor() {
        env.wasm.wasmPaths = 'btpg/';
        InferenceSession.create(exampleModel).then((s) => {
            this.session = s;
        });
    }

    public run = async (): Promise<Placement> => {
        if (this.session === undefined) {
            throw Error("session isn't ready!");
        }
        const dataA = Float32Array.from([
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        ]);
        const dataB = Float32Array.from([
            10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120,
        ]);
        const tensorA = new Tensor('float32', dataA, [3, 4]);
        const tensorB = new Tensor('float32', dataB, [4, 3]);

        // prepare feeds. use model input names as keys.
        const feeds = { a: tensorA, b: tensorB };

        // feed inputs and run
        const results = await this.session.run(feeds);
        console.log(results.c.data);

        // step 1: create typed arrays for the input
        // step 2: create tensors
        return { x: 3, y: 5, rot: 0 };
    };
}
