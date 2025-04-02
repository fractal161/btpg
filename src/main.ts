import '../styles/style.css';
import { Analysis } from './analysis';
import { NNModel } from './models/nn-model';
import { Parameters } from './params';
import { ChangeMode, TetrisPreview } from './preview';
import { TetrisState } from './tetris';

const state = new TetrisState();
const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
const preview = new TetrisPreview(board, state);
const parameters = new Parameters(preview);
const analysis = new Analysis(state, preview);
const evalButton = document.getElementById('eval') as HTMLButtonElement;
const undoButton = document.getElementById('undo') as HTMLButtonElement;
const loadingDiv = document.getElementById('loading')! as HTMLDivElement;
let model: NNModel | undefined = undefined;
let evaluating: boolean = false;

const evaluate = async () => {
    if (model === undefined || evaluating) return;
    evaluating = true;
    const beforePaint = () => new Promise(requestAnimationFrame);
    try {
        await beforePaint();
        evalButton.disabled = true;
        loadingDiv.classList.remove('hidden');
        await beforePaint();
        const result = await model.run(state, parameters);
        fetch("https://betatetris.adrien.csie.org/query", {
            method: "POST",
            body: JSON.stringify(result),
            headers: {
                "Content-type": "application/json; charset=UTF-8"
            }
        });
        await beforePaint();
        analysis.displayResult(result);
    } catch (e) {
        console.error(e);
    } finally {
        evalButton.disabled = false;
        loadingDiv.classList.add('hidden');
        evaluating = false;
    }
};

const initModel = async () => {
    const loadingText = document.getElementById('loading-text')! as HTMLSpanElement;

    model = await NNModel.create();
    if (!model.isGPU) {
        document.getElementById('message-no-webgpu')!.classList.remove('hidden');
    }
    evalButton.disabled = false;
    loadingDiv.classList.add('hidden');
    loadingText.innerText = 'Evaluating...';
    evalButton.addEventListener('click', evaluate);
};

const main = () => {
    const errorFull = document.getElementById('message-full-line')! as HTMLDivElement;
    const warnOdd = document.getElementById('message-odd-cells')! as HTMLDivElement;

    evalButton.disabled = true;
    loadingDiv.classList.remove('hidden');

    undoButton.addEventListener('click', () => {
        preview.undo();
    });

    preview.onChange = (state, changeMode, placementInfor) => {
        analysis.hideAll();
        if (changeMode == ChangeMode.PLACEMENT) {
            if (placementInfor === undefined) {
                throw new Error('placementInfor is undefined');
            }
            if (placementInfor.piece !== undefined) {
                parameters.setPiece(placementInfor.piece);
            } else {
                parameters.generateRandomPiece();
            }
            parameters.addLines(placementInfor.lineIncrement);
            if (parameters.autoEval) evaluate();
        }
        if (changeMode == ChangeMode.DRAG) return;
        const count = state.board.count();
        if (count % 2 != 0) {
            warnOdd.classList.remove('hidden');
        } else {
            warnOdd.classList.add('hidden');
            parameters.changeLineMin(count % 4 != 0);
        }
        if (state.board.numFullLines() > 0) {
            errorFull.classList.remove('hidden');
            evalButton.disabled = true;
        } else {
            errorFull.classList.add('hidden');
            if (changeMode != ChangeMode.LOAD) evalButton.disabled = false;
        }
        undoButton.disabled = preview.historySize == 1;
    };
    preview.onChange(state, ChangeMode.LOAD);
    initModel();
};
main();
