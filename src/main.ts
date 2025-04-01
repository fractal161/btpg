import '../styles/style.css';
import { Analysis } from './analysis';
import { ExampleModel } from './models/example-model';
import { Parameters } from './params';
import { TetrisPreview } from './preview';
import { TetrisState } from './tetris';


const state = new TetrisState();
const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
const preview = new TetrisPreview(board, state);
const parameters = new Parameters(preview);
const analysis = new Analysis(state, preview);
const evalButton = document.getElementById('eval') as HTMLButtonElement;
const loadingDiv = document.getElementById('loading')! as HTMLDivElement;
let model: ExampleModel | undefined = undefined;
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

    model = await ExampleModel.create();
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

    preview.onChange = (state, isRelease, placementInfor) => {
        analysis.hideAll();
        if (placementInfor !== undefined) {
            if (placementInfor.piece !== undefined) {
                parameters.setPiece(placementInfor.piece);
            } else {
                parameters.generateRandomPiece();
            }
            parameters.addLines(placementInfor.lineIncrement);
            if (parameters.autoEval) evaluate();
        }
        if (!isRelease) return;
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
            evalButton.disabled = false;
        }
    };
    initModel();
};
main();
