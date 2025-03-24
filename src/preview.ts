import { BOARD_HEIGHT, BOARD_WIDTH, TetrisState } from './tetris';

export class TetrisPreview {
    public drawMode: 'cell' | 'erase' | 'column' = 'cell';
    private drawing: boolean = false;
    private cells: HTMLTableCellElement[][] = [];
    private cursor: { x: number; y: number } | undefined = undefined;
    private board: HTMLTableElement;

    constructor(
        wrapper: HTMLDivElement,
        private tetris: TetrisState,
    ) {
        const createCell = (x: number, y: number) => {
            const cell = document.createElement('td');
            cell.classList.add('cell');
            cell.setAttribute('data-x', x.toString());
            cell.setAttribute('data-y', y.toString());
            const block = document.createElement('div');
            block.classList.add('block');
            cell.appendChild(block);
            return cell;
        };
        const createRow = (x: number): [HTMLTableRowElement, HTMLTableCellElement[]] => {
            const row = document.createElement('tr');
            row.classList.add('row');
            const rowMark = document.createElement('td');
            rowMark.classList.add('row-mark');
            rowMark.innerText = (BOARD_HEIGHT - x).toString();
            row.appendChild(rowMark);
            const cells: HTMLTableCellElement[] = [];
            for (let y = 0; y < BOARD_WIDTH; y++) {
                const cell = createCell(x, y);
                cells.push(cell);
                row.appendChild(cell);
            }
            return [row, cells];
        };
        const createColumnMarks = () => {
            const columnMarks = document.createElement('tr');
            columnMarks.classList.add('row-column-marks');
            columnMarks.appendChild(document.createElement('td'));
            for (let i = 0; i < BOARD_WIDTH; i++) {
                const columnMark = document.createElement('td');
                columnMark.classList.add('column-mark');
                columnMark.setAttribute('data-x', BOARD_HEIGHT.toString());
                columnMark.setAttribute('data-y', i.toString());
                columnMark.innerText = ((i + 1) % 10).toString();
                columnMarks.appendChild(columnMark);
            }
            return columnMarks;
        }
        const board = document.createElement('table');
        this.board = board;
        board.classList.add('board');
        board.classList.add('level-8');
        for (let i = 0; i < BOARD_HEIGHT; i++) {
            const [row, cells] = createRow(i);
            this.cells.push(cells);
            board.appendChild(row);
        }
        board.appendChild(createColumnMarks());
        wrapper.appendChild(board);

        board.addEventListener('mousedown', this.mouseDown.bind(this));
        board.addEventListener('mouseup', this.mouseUp.bind(this));
        board.addEventListener('mouseleave', this.mouseLeave.bind(this));
        for (let i of board.querySelectorAll('td')) {
            i.addEventListener('mousemove', this.mouseMove.bind(this));
        }
        document.addEventListener('keydown', this.keyDown.bind(this));
        document.addEventListener('keyup', this.keyUp.bind(this));
    }

    private draw(cursorX: number, cursorY: number) {
        const setCell = (x: number, y: number, value: boolean) => {
            if (x < 0 || x >= BOARD_HEIGHT || y < 0 || y >= BOARD_WIDTH) return;
            const cell = this.cells[x][y];
            if (value) {
                // random block 1-3
                cell.classList = 'cell block-' + Math.floor(Math.random() * 3 + 1);
            } else {
                cell.classList = 'cell';
            }
            this.tetris.setCell(x, y, value);
        }
        if (this.drawMode === 'cell') {
            setCell(cursorX, cursorY, true);
        } else if (this.drawMode === 'erase') {
            setCell(cursorX, cursorY, false);
        } else if (this.drawMode === 'column') {
            for (let i = 0; i < cursorX; i++) {
                setCell(i, cursorY, false);
            }
            for (let i = cursorX; i < BOARD_HEIGHT; i++) {
                setCell(i, cursorY, true);
            }
        }
    }

    private mouseDown(e: MouseEvent): void {
        this.drawing = true;
        // partly handle case where another window is focus and shift is pressed
        this.drawMode = e.shiftKey ? 'column' : 'cell';
        if (this.cursor) {
            if (this.drawMode == 'column') {
                this.draw(this.cursor.x, this.cursor.y);
            } else if (this.cursor.x < BOARD_HEIGHT) {
                // use first cell to decide if erasing or drawing
                const cell = this.tetris.getCell(this.cursor.x, this.cursor.y);
                this.drawMode = cell ? 'erase' : 'cell';
                this.draw(this.cursor.x, this.cursor.y);
            }
        }
        this.resetHover();
    }

    private mouseUp(): void {
        this.drawing = false;
        this.resetHover();
    }

    private bresenham(x0: number, y0: number, x1: number, y1: number) {
        const dx = Math.abs(x1 - x0);
        const dy = Math.abs(y1 - y0);
        const sx = x0 < x1 ? 1 : -1;
        const sy = y0 < y1 ? 1 : -1;
        let err = dx - dy;
        while (true) {
            this.draw(x0, y0);
            if (x0 === x1 && y0 === y1) break;
            const err2 = err * 2;
            if (err2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (err2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    private mouseMove(e: MouseEvent): void {
        const x = (e.currentTarget as HTMLTableCellElement).getAttribute('data-x');
        const y = (e.currentTarget as HTMLTableCellElement).getAttribute('data-y');
        if (x === null || y === null) {
            this.cursor = undefined;
            return;
        }
        const newCursor = { x: parseInt(x), y: parseInt(y) };
        if (this.drawing) {
            if (this.cursor === undefined) {
                this.draw(newCursor.x, newCursor.y);
            } else if (this.cursor.x !== newCursor.x || this.cursor.y !== newCursor.y) {
                this.bresenham(this.cursor.x, this.cursor.y, newCursor.x, newCursor.y);
            }
            this.cursor = newCursor;
        } else {
            this.cursor = newCursor;
            this.resetHover();
        }
    }

    private mouseLeave(): void {
        this.cursor = undefined;
        this.resetHover();
    }

    private resetHover(): void {
        this.board.querySelectorAll('.hover').forEach((elem) => {
            elem.classList.remove('hover');
        });
        if (this.cursor && !this.drawing && this.drawMode === 'column') {
            for (let i = this.cursor.x; i < BOARD_HEIGHT; i++) {            
                this.cells[i][this.cursor.y].classList.add('hover');
            }
        }
    }

    public keyDown(e: KeyboardEvent): void {
        if (this.drawing) return;
        if (e.key === 'Shift') {
            this.drawMode = 'column';
        }
        this.resetHover();
    }

    public keyUp(e: KeyboardEvent): void {
        if (this.drawing) return;
        if (e.key === 'Shift') {
            this.drawMode = 'cell';
        }
        this.resetHover();
    }

    public setLevel(level: number) {
        const levelClass = level % 10;
        this.board.classList = 'board level-' + levelClass;
    }
}
