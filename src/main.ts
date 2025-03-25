import '../styles/style.css';
import { ExampleModel } from './models/example-model';
import { TetrisPreview } from './preview';
import { PIECE_NAMES, TetrisState } from './tetris';

const main = async () => {
    // @ts-ignore

    const state = new TetrisState();

    const board = document.querySelector<HTMLDivElement>('#board-wrapper')!;
    const preview = new TetrisPreview(board, state);

    // intialize the config menus
    const createSelectFromNumberArray = (
        id: string,
        values: Array<number>,
        displayFn: (v: number) => string,
    ): HTMLSelectElement => {
        const select = document.createElement('select');
        select.id = id;
        for (const value of values) {
            const option = document.createElement('option');
            option.value = value.toString();
            option.innerText = displayFn(value);
            select.appendChild(option);
        }
        return select;
    };

    const createSelectFromList = (
        id: string,
        values: Array<Array<string>>,
    ): HTMLSelectElement => {
        const select = document.createElement('select');
        select.id = id;
        for (const [value, display] of values) {
            const option = document.createElement('option');
            option.value = value.toString();
            option.textContent = display.toString();
            select.appendChild(option);
        }
        return select;
    };

    const createNumberSelector = (
        id: string,
        min: number,
        max: number,
        step: number = 1,
    ): HTMLInputElement => {
        const input = document.createElement('input');
        input.type = 'number';
        input.id = id;
        input.min = min.toString();
        input.max = max.toString();
        input.step = step.toString();
        return input;
    }

    const wrapSelectInField = (
        select: HTMLSelectElement | HTMLInputElement,
        labelText: string,
    ): HTMLDivElement => {
        const label = document.createElement('label');
        label.textContent = labelText;
        label.htmlFor = select.id;
        const div = document.createElement('div');
        div.classList.add('field');
        div.appendChild(label);
        div.appendChild(select);
        return div;
    };

    // TODO: right now we assume all models have the same config, but in general
    // this might not be true
    const createGameConfigMenu = () => {
        const currentSelect = createSelectFromList(
            'current-piece-select',
            PIECE_NAMES.map((name, i) => [i.toString(), name]),
        );
        currentSelect.selectedIndex = Math.floor(Math.random() * 7);
        const currentField = wrapSelectInField(currentSelect, 'Current piece:');

        // TODO: betatetris uses the current line count as a factor in making
        // decisions. this is hidden from the user for simplicity, but it might
        // be good to support this functionality later
        // TODO: maybe this belongs in the game state menu??
        const LEVELS = [18, 19, 29, 39];
        const lvlSelect = createSelectFromNumberArray(
            'lvl-select',
            LEVELS,
            (lvl) => lvl.toString(),
        );
        const lvlField = wrapSelectInField(lvlSelect, 'Level speed:');

        const linesInput = createNumberSelector('lines-input', 0, 429, 2);
        const linesField = wrapSelectInField(linesInput, 'Lines:');
        linesInput.value = '32';

        const gameConfig =
            document.querySelector<HTMLDivElement>('#game-param-config')!;
        gameConfig.appendChild(currentField);
        gameConfig.appendChild(lvlField);
        gameConfig.appendChild(linesField);

        const levelChange = (e: Event) => {
            const target = e.target as HTMLSelectElement;
            const level = parseInt(target.value);
            if (level == 18) {
                linesInput.value = '30';
                preview.setLevel(18);
            } else if (level == 19) {
                linesInput.value = '160';
                preview.setLevel(22);
            } else if (level == 29) {
                linesInput.value = '260';
                preview.setLevel(32);
            } else if (level == 39) {
                linesInput.value = '360';
                preview.setLevel(42);
            }
        }
        lvlSelect.addEventListener('change', levelChange);

        const lineChange = (e: Event) => {  
            const target = e.target as HTMLInputElement;
            const value = parseInt(target.value);
            let level = 19 + ~~((value - 130) / 10);
            if (value < 130) {
                lvlSelect.selectedIndex = 0;
                level = 18;
            } else if (value < 230) {
                lvlSelect.selectedIndex = 1;
            } else if (value < 330) {
                lvlSelect.selectedIndex = 2;
            } else {
                lvlSelect.selectedIndex = 3;
            }
            preview.setLevel(level);
        }
        linesInput.addEventListener('input', lineChange);
    };

    const createModelConfigMenu = () => {
        const HZ_VALUES = ['10', '12', '15', '20', '24', '30', 'slow5'];
        const HZ_NAMES = [ '10hz', '12hz', '15hz', '20hz', '24hz', '30hz', 'slow 5 tap'];
        const hzSelect = createSelectFromList(
            'hz-select',
            HZ_VALUES.map((elem, i) => [elem, HZ_NAMES[i]]),
        );
        hzSelect.selectedIndex = 4;
        const hzField = wrapSelectInField(hzSelect, 'Tap speed:');

        const REACTION_TIMES = [0, 18, 21, 24, 30, 61];
        const reactionSelect = createSelectFromNumberArray(
            'reaction-select',
            REACTION_TIMES,
            (delay) => delay == 61 ? 'No adj' : Math.round((delay * 1000) / 60).toString() + 'ms (' + delay + 'f)',
        );
        reactionSelect.selectedIndex = 2;
        const reactionField = wrapSelectInField(
            reactionSelect,
            'Reaction time:',
        );

        const AGGRESSION_LEVELS = [0, 1, 2];
        const aggroNames = ['Low', 'Medium', 'High'];
        const aggroOptions = aggroNames.map((elem, i) => [
            AGGRESSION_LEVELS[i].toString(),
            elem,
        ]);
        const aggroSelect = createSelectFromList('aggro-select', aggroOptions);
        const aggroField = wrapSelectInField(aggroSelect, 'Aggression:');

        // add everything to the menu
        const modelConfig = document.querySelector<HTMLDivElement>(
            '#model-param-config',
        )!;
        modelConfig.appendChild(hzField);
        modelConfig.appendChild(reactionField);
        modelConfig.appendChild(aggroField);
    };

    createGameConfigMenu();
    createModelConfigMenu();

    // TODO: disable button until model is fully loaded
    // also indicate that the model is being loaded lmao

    const model = await ExampleModel.create();

    // TODO: make this actually call the model lol
    const evalButton = document.querySelector<HTMLButtonElement>('#eval')!;
    evalButton.addEventListener('click', async () => {
        try {
            const result = await model.run(state);
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
