import { TetrisPreview } from './preview';
import { PIECE_NAMES } from './tetris';

function createSelectFromNumberArray(
  id: string,
  values: Array<number>,
  displayFn: (v: number) => string,
): HTMLSelectElement {
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

function createSelectFromList(
  id: string,
  values: Array<Array<string>>,
): HTMLSelectElement {
  const select = document.createElement('select');
  select.id = id;
  for (const [value, display] of values) {
      const option = document.createElement('option');
      option.value = value.toString();
      option.innerText = display.toString();
      select.appendChild(option);
  }
  return select;
}

function createNumberSelector(
  id: string,
  min: number,
  max: number,
  step: number = 1,
): HTMLInputElement {
  const input = document.createElement('input');
  input.type = 'number';
  input.id = id;
  input.min = min.toString();
  input.max = max.toString();
  input.step = step.toString();
  return input;
}

function wrapSelectInField(
  select: HTMLSelectElement | HTMLInputElement,
  labelText: string,
): HTMLDivElement {
  const label = document.createElement('label');
  label.textContent = labelText;
  label.htmlFor = select.id;
  const div = document.createElement('div');
  div.classList.add('field');
  div.appendChild(label);
  div.appendChild(select);
  return div;
}


export class Parameters {
  private _lines: number = 32;
  get lines(): number {
    return this._lines;
  }

  private _piece: number = 0;
  get piece(): number {
    return this._piece;
  }

  private createGameConfigMenu() {
    const pieceSelect = createSelectFromList(
      'current-piece-select',
      PIECE_NAMES.map((name, i) => [i.toString(), name]),
    );
    pieceSelect.selectedIndex = Math.floor(Math.random() * 7);
    const pieceField = wrapSelectInField(pieceSelect, 'Current piece:');

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

    this.gameConfig.appendChild(pieceField);
    this.gameConfig.appendChild(lvlField);
    this.gameConfig.appendChild(linesField);

    const pieceChange = (e: Event) => {
      const target = e.target as HTMLSelectElement;
      this._piece = parseInt(target.value);
    };
    pieceSelect.addEventListener('change', pieceChange);

    const levelChange = (e: Event) => {
      const target = e.target as HTMLSelectElement;
      const level = parseInt(target.value);
      if (level == 18) {
        this._lines = 30;
        this.preview.setLevel(18);
      } else if (level == 19) {
        this._lines = 160;
        this.preview.setLevel(22);
      } else if (level == 29) {
        this._lines = 260;
        this.preview.setLevel(32);
      } else if (level == 39) {
        this._lines = 360;
        this.preview.setLevel(42);
      }
      linesInput.value = this._lines.toString();
    }
    lvlSelect.addEventListener('change', levelChange);

    const lineChange = (e: Event) => {  
      const target = e.target as HTMLInputElement;
      const value = parseInt(target.value);
      this._lines = value;
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
      this.preview.setLevel(level);
    }
    linesInput.addEventListener('input', lineChange);
  };

  private createModelConfigMenu() {
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
    this.modelConfig.appendChild(hzField);
    this.modelConfig.appendChild(reactionField);
    this.modelConfig.appendChild(aggroField);
  }

  constructor(
    private gameConfig: HTMLDivElement,
    private modelConfig: HTMLDivElement,
    private preview: TetrisPreview,
  ) {
    this.createGameConfigMenu();
    this.createModelConfigMenu();
  }
};