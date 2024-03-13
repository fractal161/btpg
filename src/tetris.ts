export const BOARD_HEIGHT = 20;
export const BOARD_WIDTH = 10;

export enum Piece {
    T = 'T',
    J = 'J',
    Z = 'Z',
    O = 'O',
    S = 'S',
    L = 'L',
    I = 'I',
}

// coordinate system is right+down, i.e. lower-indexed rows are higher up
export class TetrisState {
    constructor(
        public readonly board: Array<Array<boolean>> = [
            ...Array(BOARD_HEIGHT),
        ].map((_) => Array(BOARD_WIDTH).fill(false)),
        public current: Piece = Piece.T,
        public next: Piece = Piece.T,
    ) {}

    public getCell(row: number, col: number): boolean {
        if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH) {
            throw Error('coordinates out of bounds');
        }
        return this.board[row][col];
    }

    public setCell(row: number, col: number, state: boolean) {
        if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH) {
            throw Error('coordinates out of bounds');
        }
        this.board[row][col] = state;
    }

    public toggleCell(row: number, col: number) {
        if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH) {
            throw Error('coordinates out of bounds');
        }
        this.board[row][col] = !this.board[row][col];
    }

    public clearBoard() {
        for (let i = 0; i < BOARD_HEIGHT; i++) {
            for (let j = 0; j < BOARD_WIDTH; j++) {
                this.board[i][j] = false;
            }
        }
    }
}

export interface Placement {
    x: number;
    y: number;
    rot: number;
}
