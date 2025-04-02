import { TapSpeed } from '../wasm/tetris';
import { TetrisPreview } from './preview';
import { module, PIECE_NAMES, TRANSITION_PROBS } from './tetris';
import { Select, Checkbox, wrapSelectInField, createNumberSelector } from './components';

export class Parameters {
    private pieceSelect: Select<number>;
    private lvlSelect: Select<number>;
    private linesInput: HTMLInputElement;
    private modelSelect: Select<number>;
    private hzSelect: Select<TapSpeed>;
    private reactionSelect: Select<number>;
    private aggressionSelect: Select<number>;
    private autoEvalCheckbox: Checkbox;
    get piece(): number {
        return this.pieceSelect.value;
    }
    get level(): number {
        return this.lvlSelect.value;
    }
    get model(): number {
        return this.modelSelect.value;
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
    get autoEval(): boolean {
        return this.autoEvalCheckbox.value;
    }

    private setLines(value: number, checkParity: Boolean = true) {
        if (checkParity) {
            value = (value & 0xfffffffe) + (this.linesInput.min == '1' ? 1 : 0);
        }
        this.linesInput.value = value.toString();
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
        localStorage.setItem('field-lines-input', value.toString());
        this.lvlSelect.saveValue();
        this.preview.setLevel(level);
    }

    private _lines: number = 0;
    get lines(): number {
        return this._lines;
    }
    set lines(value: number) {
        this.setLines(value);
    }

    constructor(
        private preview: TetrisPreview,
    ) {
        const gameConfig = document.getElementById('game-param-config')! as HTMLDivElement;
        const modelConfig = document.getElementById('model-param-config')! as HTMLDivElement;
        const websiteConfig = document.getElementById('website-config')! as HTMLDivElement;

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
            false,
        );
        const lvlField = wrapSelectInField(this.lvlSelect.element, 'Level speed:');

        this.linesInput = createNumberSelector('lines-input', 0, 429, 2, 30);
        const linesField = wrapSelectInField(this.linesInput, 'Lines:');

        gameConfig.appendChild(pieceField);
        gameConfig.appendChild(lvlField);
        gameConfig.appendChild(linesField);

        /// Model config
        this.lvlSelect.onchange = (_: Event, level: number) => {
            if (level == 18) {
                this.lines = 30;
            } else if (level == 19) {
                this.lines = 160;
            } else if (level == 29) {
                this.lines = 260;
            } else if (level == 39) {
                this.lines = 360;
            }
        }

        const lineInput = (_: Event) => {
            this.setLines(parseInt(this.linesInput.value), false);
        }
        const lineChange = (_: Event) => {
            this.setLines(parseInt(this.linesInput.value), true);
        }
        this.linesInput.addEventListener('input', lineInput);
        this.linesInput.addEventListener('change', lineChange);
        this.setLines(parseInt(this.linesInput.value), true);

        /// Model config
        this.modelSelect = new Select(
            'model-select',
            [0, 1],
            ['Normal', 'Aggro'],
            0,
        );
        const modelField = wrapSelectInField(this.modelSelect.element, 'Model:');

        this.hzSelect = new Select(
            'hz-select',
            [module.TapSpeed.kTap10Hz, module.TapSpeed.kTap12Hz, module.TapSpeed.kTap15Hz,
                module.TapSpeed.kTap20Hz, module.TapSpeed.kTap24Hz, module.TapSpeed.kTap30Hz,
                module.TapSpeed.kSlow5],
            ['10hz', '12hz', '15hz', '20hz', '24hz', '30hz', 'slow 5 tap'],
            4,
        );
        const hzField = wrapSelectInField(this.hzSelect.element, 'Tap speed:');

        const REACTION_TIMES = [0, 18, 21, 24, 30, 61];
        this.reactionSelect = new Select(
            'reaction-select',
            REACTION_TIMES,
            REACTION_TIMES.map((delay) => delay == 61 ? 'No adj' :
                Math.round((delay * 1000) / 60).toString() + 'ms (' + delay + 'f)'),
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

        modelConfig.appendChild(modelField);
        modelConfig.appendChild(hzField);
        modelConfig.appendChild(reactionField);
        modelConfig.appendChild(aggroField);

        this.autoEvalCheckbox = new Checkbox('auto-eval', 'Auto evaluate:');
        websiteConfig.appendChild(this.autoEvalCheckbox.wrapper);
    }

    public setPiece(piece: number) {
        this.pieceSelect.selectedIndex = piece;
    }

    public generateRandomPiece() {
        const transition_probs = TRANSITION_PROBS[this.piece];
        const random = Math.random() * 32;
        let sum = 0;
        for (let i = 0; i < transition_probs.length; i++) {
            sum += transition_probs[i];
            if (random < sum) {
                this.setPiece(i);
                return;
            }
        }
        this.setPiece(0);
    }

    public changeLineMin(isOdd: Boolean) {
        this.linesInput.min = isOdd ? '1' : '0';
        this.linesInput.value = ((this.lines & 0xfffffffe) + (isOdd ? 1 : 0)).toString();
    }

    public addLines(lines: number) {
        this.linesInput.min = (this.lines + lines) % 2 ? '1' : '0';
        this.lines = Math.min(429, this.lines + lines);
    }
};