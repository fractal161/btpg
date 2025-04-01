import '../styles/style.css';
import { Analysis } from './analysis';
import { ExampleModel } from './models/example-model';
import { Parameters } from './params';
import { TetrisPreview } from './preview';
import { TetrisState } from './tetris';


const state = new TetrisState();
const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
const preview = new TetrisPreview(board, state);
const parameters = new Parameters(
    document.getElementById('game-param-config')! as HTMLDivElement,
    document.getElementById('model-param-config')! as HTMLDivElement,
    preview,
);
const analysis = new Analysis(state, preview);
const evalButton = document.getElementById('eval') as HTMLButtonElement;
const loadingDiv = document.getElementById('loading')! as HTMLDivElement;
let model: ExampleModel | undefined = undefined;

const Sleep = async (ms: number) => {
    return new Promise((resolve) => setTimeout(resolve, ms));
};

const evaluate = async () => {
    if (model === undefined) return;
    try {
        evalButton.disabled = true;
        loadingDiv.classList.remove('hidden');
        await Sleep(1); // make UI change visible
        const result = await model.run(state, parameters);
        await Sleep(1); // prevent double-click
        analysis.displayResult(result);
    } catch (e) {
        console.error(e);
    } finally {
        evalButton.disabled = false;
        loadingDiv.classList.add('hidden');
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

    preview.onChange = (state, isRelease, lineIncrement) => {
        analysis.hideAll();
        if (lineIncrement !== undefined) {
            parameters.generateRandomPiece();
            parameters.addLines(lineIncrement);
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
