import '../styles/style.css';
import { Piece } from './tetris';

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
    values: Array<[string, string]>,
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

const wrapSelectInField = (
    select: HTMLSelectElement,
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

const createGameConfigMenu = () => {
    const currentSelect = createSelectFromList(
        'current-piece-select',
        Object.entries(Piece),
    );
    currentSelect.selectedIndex = Math.floor(Math.random() * 7);
    const currentField = wrapSelectInField(currentSelect, 'Current piece:');

    const nextSelect = createSelectFromList(
        'current-piece-select',
        Object.entries(Piece),
    );
    nextSelect.selectedIndex = Math.floor(Math.random() * 7);
    const nextField = wrapSelectInField(nextSelect, 'Next piece:');

    // TODO: betatetris uses the current line count as a factor in making
    // decisions. this is hidden from the user for simplicity, but it might
    // be good to support this functionality later
    // TODO: maybe this belongs in the game state menu??
    const LEVELS = [18, 19, 29];
    const lvlSelect = createSelectFromNumberArray('lvl-select', LEVELS, (lvl) =>
        lvl.toString(),
    );
    const lvlField = wrapSelectInField(lvlSelect, 'Level:');

    const gameConfig =
        document.querySelector<HTMLDivElement>('#game-param-config')!;
    gameConfig.appendChild(currentField);
    gameConfig.appendChild(nextField);
    gameConfig.appendChild(lvlField);
};

const createModelConfigMenu = () => {
    const HZ_VALUES = [12, 13.5, 15, 20, 30];
    const hzSelect = createSelectFromNumberArray(
        'hz-select',
        HZ_VALUES,
        (hz) => hz.toString() + 'hz',
    );
    hzSelect.selectedIndex = 4;
    const hzField = wrapSelectInField(hzSelect, 'Speed:');

    const REACTION_TIMES = [8, 16, 21, 25, 61];
    const reactionSelect = createSelectFromNumberArray(
        'reaction-select',
        REACTION_TIMES,
        (delay) => Math.round((delay * 1000) / 60).toString() + 'ms',
    );
    reactionSelect.selectedIndex = 2;
    const reactionField = wrapSelectInField(reactionSelect, 'Reaction time:');

    const AGGRESSION_LEVELS = [2000, 360, 100];
    const aggroNames = ['Low', 'Medium', 'High'];
    const aggroOptions: Array<[string, string]> = aggroNames.map((elem, i) => [
        AGGRESSION_LEVELS[i].toString(),
        elem,
    ]);
    const aggroSelect = createSelectFromList('aggro-select', aggroOptions);
    const aggroField = wrapSelectInField(aggroSelect, 'Aggression:');

    const droughtOptions: Array<[string, string]> = [
        ['yes', 'Yes'],
        ['no', 'No'],
    ];
    const droughtSelect = createSelectFromList(
        'drought-select',
        droughtOptions,
    );
    droughtSelect.selectedIndex = 1;
    const droughtField = wrapSelectInField(droughtSelect, 'Drought mode?');

    // add everything to the menu
    const modelConfig = document.querySelector<HTMLDivElement>(
        '#model-param-config',
    )!;
    modelConfig.appendChild(hzField);
    modelConfig.appendChild(reactionField);
    modelConfig.appendChild(aggroField);
    modelConfig.appendChild(droughtField);
};

createGameConfigMenu();
createModelConfigMenu();

const board = document.querySelector<HTMLCanvasElement>('#board')!;
const boardCtx = board.getContext('2d')!;
boardCtx.fillStyle = 'black';
boardCtx.fillRect(0, 0, board.width, board.height);

// TODO: make this actually call the model lol
const evalButton = document.querySelector<HTMLButtonElement>('#eval')!;
evalButton.addEventListener('click', () => console.log('eval!'));
