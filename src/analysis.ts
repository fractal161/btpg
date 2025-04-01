import { TapSpeed } from '../wasm/tetris';
import { TetrisPreview } from './preview';
import { module, PIECE_NAMES, TetrisState } from './tetris';

const TRANSITION_PROBS = [
    [1, 5, 6, 5, 5, 5, 5],
    [6, 1, 5, 5, 5, 5, 5],
    [5, 6, 1, 5, 5, 5, 5],
    [5, 5, 5, 2, 5, 5, 5],
    [5, 5, 5, 5, 2, 5, 5],
    [6, 5, 5, 5, 5, 1, 5],
    [5, 5, 5, 5, 6, 5, 1],
];

function scoreText(avg: number, dev: number) {
    const avgInt = Math.round(avg * 100);
    const devInt = Math.round(dev * 100);
    return `${avgInt}k ± ${devInt}k`;
}

export class Analysis {
    private scoreSection: HTMLDivElement;
    private scoreCell: HTMLTableCellElement;
    private hoverSection: HTMLDivElement;
    private hoverTable: HTMLTableElement;
    private hoverTitle: HTMLDivElement;
    private adjustmentSection: HTMLDivElement;
    private adjustmentRows: Array<Array<HTMLTableCellElement>>;
    private placementSection: HTMLDivElement;
    private placementTable: HTMLTableElement;
    private elapsedSection: HTMLDivElement;
    private elapsedNumber: HTMLSpanElement;

    constructor(
        private tetris: TetrisState,
        private preview: TetrisPreview,
    ) {
        this.scoreSection = document.getElementById('analysis-score')! as HTMLDivElement;
        this.scoreCell = document.getElementById('analysis-score-table')!.querySelector('td')!;
        this.hoverSection = document.getElementById('analysis-hover')! as HTMLDivElement;
        this.hoverTable = document.getElementById('analysis-hover-table')! as HTMLTableElement;
        this.hoverTitle = document.getElementById('analysis-hover-title')! as HTMLDivElement;
        this.adjustmentSection = document.getElementById('analysis-adjustment')! as HTMLDivElement;
        this.placementSection = document.getElementById('analysis-placement')! as HTMLDivElement;
        this.placementTable = document.getElementById('analysis-placement-table')! as HTMLTableElement;
        this.elapsedSection = document.getElementById('analysis-elapsed')! as HTMLDivElement;
        this.elapsedNumber = document.getElementById('analysis-elapsed-number')! as HTMLSpanElement;

        const adjustmentTable = document.getElementById('analysis-adjustment-table')! as HTMLTableElement;
        this.adjustmentRows = [];
        for (let i = 0; i < 7; i++) {
            const piece = document.createElement('td');
            piece.classList.add('cell-piece');
            piece.innerText = PIECE_NAMES[i];
            const prob = document.createElement('td');
            prob.classList.add('cell-probability');
            const placement = document.createElement('td');
            placement.classList.add('cell-placement');
            const score = document.createElement('td');
            score.classList.add('cell-score');
            const row = document.createElement('tr');
            row.appendChild(piece);
            row.appendChild(prob);
            row.appendChild(placement);
            row.appendChild(score);
            adjustmentTable.appendChild(row);
            this.adjustmentRows.push([piece, prob, placement, score]);
        }
    }

    public displayResult(result: Record<string, any>) {
        const piece = result.query.piece;

        this.scoreSection.classList.remove('hidden');
        this.scoreCell.innerText = scoreText(result.eval[1], result.eval[2]);
        this.elapsedSection.classList.remove('hidden');
        this.elapsedNumber.innerText = `${Math.round(result.elapsed_time)}`;
        if (result.adjustment) {
            this.placementSection.classList.add('hidden');
            if (result.query.reactionTime == 0) {
                this.hoverSection.classList.add('hidden');
            } else {
                this.hoverSection.classList.remove('hidden');
                this.hoverTable.innerHTML = '';
                if (result.best_premove.length == 1) {
                    this.hoverTitle.innerText = 'Best hover position';
                    const i = result.best_premove[0];
                    const placement = document.createElement('td');
                    placement.classList.add('cell-placement');
                    placement.innerText = this.tetris.board.placementNotation(
                        piece, i.position.r, i.position.x, i.position.y);
                    const row = document.createElement('tr');
                    row.appendChild(placement);
                    this.hoverTable.appendChild(row);
                } else {
                    this.hoverTitle.innerText = 'Good hover positions';
                    for (let i of result.best_premove) {
                        const modes = document.createElement('td');
                        modes.classList.add('cell-hover-mode');
                        modes.innerText = i.modes;
                        const placement = document.createElement('td');
                        placement.classList.add('cell-placement');
                        placement.innerText = this.tetris.board.placementNotation(
                            piece, i.position.r, i.position.x, i.position.y);
                        const row = document.createElement('tr');
                        row.appendChild(modes);
                        row.appendChild(placement);
                        this.hoverTable.appendChild(row);
                    }
                }
            }
            this.adjustmentSection.classList.remove('hidden');
            for (let i = 0; i < 7; i++) {
                const position = result.adj_best[i];
                this.adjustmentRows[i][1].innerText = `${TRANSITION_PROBS[piece][i]}/32`;
                this.adjustmentRows[i][2].innerText = this.tetris.board.placementNotation(
                    piece, position.r, position.x, position.y);
                this.adjustmentRows[i][3].innerText = scoreText(result.adj_vals[i][1], result.adj_vals[i][2]);
            }
        } else {
            this.hoverSection.classList.add('hidden');
            this.adjustmentSection.classList.add('hidden');
            this.placementSection.classList.remove('hidden');
            this.placementTable.innerHTML = '';
            for (let i = 0; i < result.moves.length; i++) {
                const position = result.moves[i].position;
                const rank = document.createElement('td');
                rank.classList.add('cell-rank');
                rank.innerText = `${i + 1}`;
                const placement = document.createElement('td');
                placement.classList.add('cell-placement');
                placement.innerText = this.tetris.board.placementNotation(
                    piece, position.r, position.x, position.y);
                const prob = document.createElement('td');
                prob.classList.add('cell-probability');
                prob.innerText = `${(result.moves[i].prob * 100).toFixed(2)}%`;
                const row = document.createElement('tr');
                row.appendChild(rank);
                row.appendChild(placement);
                row.appendChild(prob);
                this.placementTable.appendChild(row);
            }
        }
    }
};