import Module, { Board } from '../wasm/tetris.js';

export const module = await Module();

export const BOARD_HEIGHT = 20;
export const BOARD_WIDTH = 10;

export enum Piece {
    T = 0,
    J = 1,
    Z = 2,
    O = 3,
    S = 4,
    L = 5,
    I = 6,
};
export const PIECE_NAMES = ['T', 'J', 'Z', 'O', 'S', 'L', 'I'];
export const TRANSITION_PROBS = [
    [1, 5, 6, 5, 5, 5, 5],
    [6, 1, 5, 5, 5, 5, 5],
    [5, 6, 1, 5, 5, 5, 5],
    [5, 5, 5, 2, 5, 5, 5],
    [5, 5, 5, 5, 2, 5, 5],
    [6, 5, 5, 5, 5, 1, 5],
    [5, 5, 5, 5, 6, 5, 1],
];

// coordinate system is right+down, i.e. lower-indexed rows are higher up
export class TetrisState {
    constructor(
        public readonly board: Board = new module.Board,
    ) {}

    public setCell(x: number, y: number, value: boolean) {
        value ? this.board.setCellFilled(x, y) : this.board.setCellEmpty(x, y);
    }

    public getCell(x: number, y: number): boolean {
        return this.board.isCellFilled(x, y);
    }
}

export class Placement {
    public r: number;
    public x: number;
    public y: number;

    constructor(
        r: number,
        x?: number,
        y?: number,
    ) {
        if (x === undefined || y === undefined) {
            this.r = ~~(r / 200);
            this.x = ~~(r / 10) % 20;
            this.y = r % 10;
        } else {
            this.r = r;
            this.x = x;
            this.y = y;
        }
    }
}
