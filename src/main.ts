import '../styles/style.css';
import { ExampleModel } from './models/example-model';
import { Parameters } from './params';
import { TetrisPreview } from './preview';
import { TetrisState } from './tetris';

const main = async () => {
    const state = new TetrisState();

    const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
    const preview = new TetrisPreview(board, state);

    const parameters = new Parameters(
        document.getElementById('game-param-config')! as HTMLDivElement,
        document.getElementById('model-param-config')! as HTMLDivElement,
        preview,
    );

    const Sleep = async (ms: number) => {
        return new Promise((resolve) => setTimeout(resolve, ms));
    };

    const evalButton = document.getElementById('eval') as HTMLButtonElement;
    const loadingDiv = document.getElementById('loading')! as HTMLDivElement;
    const loadingText = document.getElementById('loading-text')! as HTMLSpanElement;
    evalButton.disabled = true;
    loadingDiv.classList.remove('hidden');
    const model = await ExampleModel.create();
    evalButton.disabled = false;
    loadingDiv.classList.add('hidden');
    loadingText.innerText = 'Evaluating...';
    evalButton.addEventListener('click', async () => {
        try {
            evalButton.disabled = true;
            loadingDiv.classList.remove('hidden');
            await Sleep(1); // make UI change visible
            const result = await model.run(state, parameters);
            await Sleep(1); // prevent double-click
            console.log(result);
        } catch (e) {
            console.error(e);
        } finally {
            evalButton.disabled = false;
            loadingDiv.classList.add('hidden');
        }
    });
};
main();
