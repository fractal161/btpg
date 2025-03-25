import '../styles/style.css';
import { ExampleModel } from './models/example-model';
import { Parameters } from './params';
import { TetrisPreview } from './preview';
import { TetrisState } from './tetris';

const main = async () => {
    // @ts-ignore

    const state = new TetrisState();

    const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
    const preview = new TetrisPreview(board, state);

    const parameters = new Parameters(
        document.getElementById('game-param-config')! as HTMLDivElement,
        document.getElementById('model-param-config')! as HTMLDivElement,
        preview,
    );

    // TODO: disable button until model is fully loaded
    // also indicate that the model is being loaded lmao

    const model = await ExampleModel.create();

    // TODO: make this actually call the model lol
    const evalButton = document.querySelector<HTMLButtonElement>('#eval')!;
    evalButton.addEventListener('click', async () => {
        try {
            const result = await model.run(state, parameters);
            console.log(result);
        } catch (e) {
            console.error(e);
        }
    });

    // global event listeners

    const keyDown = (e: KeyboardEvent) => {
        preview.keyDown(e);
    };

    document.addEventListener('keydown', keyDown);
};
main();
