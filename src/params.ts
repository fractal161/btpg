import { TapSpeed } from '../wasm/tetris';
import { TetrisPreview } from './preview';
import { module, PIECE_NAMES } from './tetris';

class Select<T> {
  private select: HTMLSelectElement;
  get element(): HTMLSelectElement {
    return this.select;
  }
  private _value: T;
  get value(): T {
    return this._value;
  }
  public onchange: (e: Event, v: T) => void = (e, v) => {};

  set selectedIndex(i: number) {
    this.select.selectedIndex = i;
    this._value = this.keys[i];
  }
  get selectedIndex(): number {
    return this.select.selectedIndex;
  }

  constructor(
    id: string,
    private keys: Array<T>,
    texts: Array<string>,
    defaultOption: number = 0,
  ) {
    this.select = document.createElement('select');
    this.select.id = id;
    if (keys.length != texts.length) {
      throw new Error('keys and texts must have the same length');
    }
    for (let i = 0; i < keys.length; i++) {
      const option = document.createElement('option');
      option.value = i.toString();
      option.innerText = texts[i];
      this.select.appendChild(option);
    }
    this.select.selectedIndex = defaultOption;
    this._value = keys[defaultOption];
    this.select.addEventListener('change', (e: Event) => {
      this._value = keys[parseInt((e.target as HTMLSelectElement).value)];
      this.onchange(e, this._value);
    });
  }
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
  private pieceSelect: Select<number>;
  private lvlSelect: Select<number>;
  private hzSelect: Select<TapSpeed>;
  private reactionSelect: Select<number>;
  private aggressionSelect: Select<number>;
  get piece(): number {
    return this.pieceSelect.value;
  }
  get level(): number {
    return this.lvlSelect.value;
  }
  get tapSpeed(): TapSpeed {
    return this.hzSelect.value;
  }
  get reactionTime(): number {
    return this.reactionSelect.value;
  }
  get aggression(): number {
    return this.aggressionSelect.value;
  }

  private _lines: number = 0;
  get lines(): number {
    return this._lines;
  }

  constructor(
    private gameConfig: HTMLDivElement,
    private modelConfig: HTMLDivElement,
    private preview: TetrisPreview,
  ) {
    /// Game config
    this.pieceSelect = new Select(
      'current-piece-select',
      [...PIECE_NAMES.keys()],
      PIECE_NAMES,
      Math.floor(Math.random() * 7),
    );
    const pieceField = wrapSelectInField(this.pieceSelect.element, 'Current piece:');

    const LEVELS = [18, 19, 29, 39];
    this.lvlSelect = new Select(
        'lvl-select',
        LEVELS,
        LEVELS.map((lvl) => lvl.toString()),
        0,
    );
    const lvlField = wrapSelectInField(this.lvlSelect.element, 'Level speed:');

    const linesInput = createNumberSelector('lines-input', 0, 429, 2);
    const linesField = wrapSelectInField(linesInput, 'Lines:');
    linesInput.value = '30';
    this._lines = 30;

    this.gameConfig.appendChild(pieceField);
    this.gameConfig.appendChild(lvlField);
    this.gameConfig.appendChild(linesField);

    this.lvlSelect.onchange = (_: Event, level: number) => {
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

    const lineChange = (e: Event) => {  
      const target = e.target as HTMLInputElement;
      const value = parseInt(target.value);
      this._lines = value;
      let level = 19 + ~~((value - 130) / 10);
      if (value < 130) {
        this.lvlSelect.selectedIndex = 0;
        level = 18;
      } else if (value < 230) {
        this.lvlSelect.selectedIndex = 1;
      } else if (value < 330) {
        this.lvlSelect.selectedIndex = 2;
      } else {
        this.lvlSelect.selectedIndex = 3;
      }
      this.preview.setLevel(level);
    }
    linesInput.addEventListener('input', lineChange);

    /// Model config
    this.hzSelect = new Select(
        'hz-select',
        [module.TapSpeed.kTap10Hz, module.TapSpeed.kTap12Hz, module.TapSpeed.kTap15Hz,
         module.TapSpeed.kTap20Hz, module.TapSpeed.kTap24Hz, module.TapSpeed.kTap30Hz,
         module.TapSpeed.kSlow5],
        [ '10hz', '12hz', '15hz', '20hz', '24hz', '30hz', 'slow 5 tap'],
        4,
    );
    const hzField = wrapSelectInField(this.hzSelect.element, 'Tap speed:');

    const REACTION_TIMES = [0, 18, 21, 24, 30, 61];
    this.reactionSelect = new Select(
        'reaction-select',
        REACTION_TIMES,
        REACTION_TIMES.map((delay) =>
            delay == 61 ? 'No adj' : Math.round((delay * 1000) / 60).toString() + 'ms (' + delay + 'f)'),
        2,
    );
    const reactionField = wrapSelectInField(this.reactionSelect.element, 'Reaction time:');

    this.aggressionSelect = new Select(
        'aggro-select',
        [2, 1, 0],
        ['Low', 'Medium', 'High'],
        2,
    );
    const aggroField = wrapSelectInField(this.aggressionSelect.element, 'Aggression:');
    
    this.modelConfig.appendChild(hzField);
    this.modelConfig.appendChild(reactionField);
    this.modelConfig.appendChild(aggroField);
  }
};