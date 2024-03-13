import { BOARD_HEIGHT, BOARD_WIDTH, TetrisState } from './tetris';

export class TetrisPreview {
    private ctx: CanvasRenderingContext2D;
    public drawMode: 'cell' | 'erase' | 'column';
    private drawing: boolean = false;
    private cursor: { x: number; y: number } | undefined = undefined;

    constructor(
        private canvas: HTMLCanvasElement,
        private tetris: TetrisState,
    ) {
        this.drawMode = 'cell';
        this.ctx = canvas.getContext('2d')!;

        // listeners for manipulating canvas state
        // TODO: this can be greatly improved to handle cases where the mouse
        // leaves/enters the board but it's good enough for now
        this.canvas.addEventListener('mousedown', this.mouseDown.bind(this));
        this.canvas.addEventListener('mouseup', this.mouseUp.bind(this));
        this.canvas.addEventListener('mousemove', this.mouseMove.bind(this));
        this.canvas.addEventListener('mouseleave', this.mouseLeave.bind(this));

        this.canvas.addEventListener('keydown', this.keyDown.bind(this));
        document.addEventListener('keydown', this.keyDown.bind(this));
        document.addEventListener('keyup', this.keyUp.bind(this));
    }

    private clear(): void {
        this.ctx.fillStyle = 'black';
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
    }

    // provided for convenience, but in theory this shouldn't be needed
    public refresh(): void {
        this.clear();
        this.ctx.fillStyle = 'rgb(0, 176, 162)';
        // draw existing board state
        const cellWidth = this.canvas.width / BOARD_WIDTH;
        const cellHeight = this.canvas.height / BOARD_HEIGHT;
        for (let i = 0; i < BOARD_HEIGHT; i++) {
            for (let j = 0; j < BOARD_WIDTH; j++) {
                if (this.tetris.board[i][j]) {
                    this.ctx.fillRect(
                        cellWidth * (j + 1 / 8),
                        cellHeight * (i + 1 / 8),
                        (cellWidth * 6) / 8,
                        (cellHeight * 6) / 8,
                    );
                }
            }
        }
        // draw preview on top
        if (this.cursor) {
            let fillStyle = 'rgba(0, 176, 162, 0.5)';
            if (this.drawing) {
                if (this.drawMode === 'erase') {
                    fillStyle = 'black';
                } else {
                    fillStyle = 'rgb(0, 176, 162)';
                }
            }
            if (this.drawMode == 'column') {
                for (let i = this.cursor.y; i < BOARD_HEIGHT; i++) {
                    this.ctx.fillStyle = 'black';
                    this.ctx.fillRect(
                        cellWidth * (this.cursor.x + 1 / 8),
                        cellHeight * (i + 1 / 8),
                        (cellWidth * 6) / 8,
                        (cellHeight * 6) / 8,
                    );
                    this.ctx.fillStyle = fillStyle;
                    this.ctx.fillRect(
                        cellWidth * (this.cursor.x + 1 / 8),
                        cellHeight * (i + 1 / 8),
                        (cellWidth * 6) / 8,
                        (cellHeight * 6) / 8,
                    );
                }
            } else {
                this.ctx.fillStyle = 'black';
                this.ctx.fillRect(
                    cellWidth * (this.cursor.x + 1 / 8),
                    cellHeight * (this.cursor.y + 1 / 8),
                    (cellWidth * 6) / 8,
                    (cellHeight * 6) / 8,
                );
                this.ctx.fillStyle = fillStyle;
                this.ctx.fillRect(
                    cellWidth * (this.cursor.x + 1 / 8),
                    cellHeight * (this.cursor.y + 1 / 8),
                    (cellWidth * 6) / 8,
                    (cellHeight * 6) / 8,
                );
            }
        }
    }

    private draw(cursorX: number, cursorY: number) {
        if (this.drawMode === undefined) {
            // attempt to look up cell to figure out what to do
            try {
                const cell = this.tetris.getCell(cursorY, cursorX);
                this.drawMode = cell ? 'erase' : 'cell';
            } catch {
                return;
            }
        }
        if (this.drawMode === 'cell') {
            this.tetris.setCell(cursorY, cursorX, true);
        } else if (this.drawMode === 'erase') {
            this.tetris.setCell(cursorY, cursorX, false);
        } else if (this.drawMode === 'column') {
            for (let i = 0; i < cursorY; i++) {
                this.tetris.setCell(i, cursorX, false);
            }
            for (let i = cursorY; i < BOARD_HEIGHT; i++) {
                this.tetris.setCell(i, cursorX, true);
            }
        }
    }

    private getCell(mouseX: number, mouseY: number): { x: number; y: number } {
        const cellWidth = this.canvas.width / BOARD_WIDTH;
        const cellHeight = this.canvas.height / BOARD_HEIGHT;
        const cursorX = Math.floor(mouseX / cellWidth);
        const cursorY = Math.floor(mouseY / cellHeight);
        return { x: cursorX, y: cursorY };
    }

    private mouseDown(e: MouseEvent): void {
        this.drawing = true;
        // partly handle case where another window is focus and shift is pressed
        this.drawMode = e.shiftKey ? 'column' : 'cell';
        if (this.cursor) {
            if (this.drawMode == 'column') {
                this.draw(this.cursor.x, this.cursor.y);
            } else {
                // use first cell to decide if erasing or drawing
                try {
                    const cell = this.tetris.getCell(
                        this.cursor.y,
                        this.cursor.x,
                    );
                    this.drawMode = cell ? 'erase' : 'cell';
                    this.draw(this.cursor.x, this.cursor.y);
                } catch (e) {
                    console.error(e);
                }
            }
        }
        this.refresh();
    }

    private mouseUp(): void {
        this.drawing = false;
        this.refresh();
    }

    private mouseMove(e: MouseEvent): void {
        const newCursor = this.getCell(e.offsetX, e.offsetY);
        if (this.drawing) {
            if (
                this.cursor === undefined ||
                this.cursor.x !== newCursor.x ||
                this.cursor.y !== newCursor.y
            ) {
                this.draw(newCursor.x, newCursor.y);
                this.cursor = newCursor;
            }
        } else {
            this.cursor = newCursor;
        }
        this.refresh();
    }

    private mouseLeave(): void {
        this.cursor = undefined;
        // TODO: if column, should clear column????
        this.refresh();
    }

    public keyDown(e: KeyboardEvent): void {
        if (this.drawing) return;
        if (e.key === 'Shift') {
            this.drawMode = 'column';
        }
        this.refresh();
    }

    public keyUp(e: KeyboardEvent): void {
        if (this.drawing) return;
        if (e.key === 'Shift') {
            this.drawMode = 'cell';
        }
        this.refresh();
    }
}
