// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// see also advanced usage of importing ONNX Runtime Web:
// https://github.com/microsoft/onnxruntime-inference-examples/tree/main/js/importing_onnxruntime-web

import { InferenceSession, Tensor, env } from 'onnxruntime-web/all';
import onnxNormal from '../../agents/model-normal.onnx';
import onnxAggro from '../../agents/model-aggro.onnx';
import { Model } from '../model';
import { module, PIECE_NAMES, Placement, TetrisState } from '../tetris';
import { Parameters } from '../params';
import * as base64js from 'base64-js';

env.wasm.wasmPaths = '/btpg/';
env.wasm.numThreads = 0;

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
    private constructor(private sessions: Array<InferenceSession>, private _isGPU: Boolean) {}

    public get isGPU() {
        return this._isGPU;
    }

    public static create = async (): Promise<ExampleModel> => {
        try {
            const session0 = await InferenceSession.create(onnxNormal, {executionProviders: ['webgpu']});
            const session1 = await InferenceSession.create(onnxAggro, {executionProviders: ['webgpu']});
            return new ExampleModel([session0, session1], true);
        } catch (e) {}
        try {
            const session0 = await InferenceSession.create(onnxNormal, {executionProviders: ['wasm']});
            const session1 = await InferenceSession.create(onnxAggro, {executionProviders: ['wasm']});
            return new ExampleModel([session0, session1], false);
        } catch (e) {
            console.error('Cannot initialize model');
            throw e;
        }
    };

    public run = async (tetris: TetrisState, params_: Parameters) => {
        if (this.sessions === undefined) {
            throw Error("session isn't ready!");
        }
        const params = {
            board: tetris.board.copy(),
            piece: params_.piece,
            lines: params_.lines,
            tapSpeed: params_.tapSpeed,
            reactionTime: params_.reactionTime,
            aggression: params_.aggression,
            model: params_.model,
        }; // prevent changing params while running

        const query = {
            board: base64js.fromByteArray(params.board.toBytes()),
            piece: params.piece,
            lines: params.lines,
            tapSpeed: params.tapSpeed.value,
            reactionTime: params.reactionTime,
            aggression: params.aggression,
            model: params.model,
        };

        const result: Record<string, any> = {game_over: false, isGPU: this.isGPU, query: query};
        const startTime = performance.now();
        const state_pair = module.GetState(
            params.board,
            params.piece, // current piece
            -1,
            {r: 0, x: 0, y: 0}, // position
            params.lines,
            params.tapSpeed,
            params.reactionTime,
            params.aggression);
        const state = state_pair.state;
        if (state.board.length == 0) {
            const finishTime = performance.now();
            const elapsedTime = finishTime - startTime;
            result.elapsed_time = elapsedTime;
            result.game_over = true;
            return result;
        }

        // prepare feeds. use model input names as keys.
        const feeds = {
            board: createONNXTensor(state.board),
            meta: createONNXTensor(state.meta),
            moves: createONNXTensor(state.moves),
            move_meta: createONNXTensor(state.move_meta),
            meta_int: createONNXTensor(state.meta_int, 'int32'),
        };

        // feed inputs and run
        const results = await this.sessions[params.model].run(feeds);
        const pi = results.pi.data;
        const pi_rank = results.pi_rank.data;
        const v = results.v.data;
        const best = new Placement(Number(pi_rank[0]));
        result.eval = Array.from(v);

        const move_mode = state_pair.move_map[best.r][best.x][best.y];
        if (move_mode == 1) {
            const moves = [{prob: pi[Number(pi_rank[0])], position: best}];
            for (let i = 1; i < 5; i++) {
                const prob = pi[Number(pi_rank[i])];
                const pos = new Placement(Number(pi_rank[i]));
                if (prob < 0.001 || state_pair.move_map[pos.r][pos.x][pos.y] != 1) break;
                moves.push({prob: prob, position: pos});
            }
            result.adjustment = false;
            result.moves = moves;
        } else if (move_mode == 3) {
            const adj_state = module.GetStateAllNextPieces(
                params.board,
                params.piece,
                best,
                params.lines,
                params.tapSpeed,
                params.reactionTime,
                params.aggression);
            const adj_feeds = {
                board: createONNXTensor(adj_state.board),
                meta: createONNXTensor(adj_state.meta),
                moves: createONNXTensor(adj_state.moves),
                move_meta: createONNXTensor(adj_state.move_meta),
                meta_int: createONNXTensor(adj_state.meta_int, 'int32'),
            };
            const adj_results = await this.sessions[params.model].run(adj_feeds);
            const pi_rank = adj_results.pi_rank.data;
            const v = adj_results.v.data;
            const adj_best = PIECE_NAMES.map((_, i) => new Placement(Number(pi_rank[i * 800])));
            const adj_vals = PIECE_NAMES.map((_, i) => [v[i], v[i+7], v[i+14]]);

            const best_premove = module.GetBestAdjModes(
                params.board,
                params.piece,
                params.lines,
                params.tapSpeed,
                params.reactionTime,
                state_pair.moves,
                adj_best);
            result.adjustment = true;
            result.adj_best = adj_best;
            result.adj_vals = adj_vals;
            result.best_premove = best_premove;
        } else {
            throw Error("Invalid move mode");
        }
        params.board.delete();
        const finishTime = performance.now();
        const elapsedTime = finishTime - startTime;
        result.elapsed_time = elapsedTime;
        return result;
    };
}
