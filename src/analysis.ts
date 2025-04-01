import { Position, TapSpeed } from '../wasm/tetris';
import { TetrisPreview } from './preview';
import { module, PIECE_NAMES, TRANSITION_PROBS, TetrisState } from './tetris';

function scoreText(avg: number, dev: number) {
    const avgInt = Math.round(avg * 100);
    const devInt = Math.round(dev * 100);
    return `${avgInt}k ± ${devInt}k`;
}

function getPosition(el: HTMLTableCellElement) {
    const x = parseInt(el.getAttribute('data-x')!);
    const y = parseInt(el.getAttribute('data-y')!);
    const r = parseInt(el.getAttribute('data-r')!);
    return {x: x, y: y, r: r};
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

    private mouseEnterHandler(e: Event) {
        const target = e.target as HTMLTableCellElement;
        if (target.getAttribute('data-x') === null) return;
        const piece = parseInt(target.getAttribute('data-piece')!);
        this.preview.previewPiece(piece, getPosition(target));
    }

    private mouseLeaveHandler(_: Event) {
        this.preview.clearPreview();
    }

    private clickPlaceHandler(e: Event) {
        const target = e.target as HTMLTableCellElement;
        if (target.getAttribute('data-x') === null) return;
        const piece = parseInt(target.getAttribute('data-piece')!);
        this.preview.placePiece(piece, getPosition(target));
    }

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
            placement.addEventListener('mouseenter', this.mouseEnterHandler.bind(this));
            placement.addEventListener('mouseleave', this.mouseLeaveHandler.bind(this));
            placement.addEventListener('click', this.clickPlaceHandler.bind(this));
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

        const setPlacementCell = (
                placement: HTMLTableCellElement, position: Position,
                addEvent: Boolean = true, addClickEvent: Boolean = false) => {
            placement.innerText = this.tetris.board.placementNotation(
                piece, position.r, position.x, position.y);
            placement.setAttribute('data-piece', piece.toString());
            placement.setAttribute('data-r', position.r.toString());
            placement.setAttribute('data-x', position.x.toString());
            placement.setAttribute('data-y', position.y.toString());
            if (!addEvent) return;
            placement.addEventListener('mouseenter', this.mouseEnterHandler.bind(this));
            placement.addEventListener('mouseleave', this.mouseLeaveHandler.bind(this));
            if (addClickEvent) {
                placement.addEventListener('click', this.clickPlaceHandler.bind(this));
            }
        };

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
                    setPlacementCell(placement, i.position);
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
                        setPlacementCell(placement, i.position);
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
                setPlacementCell(this.adjustmentRows[i][2], position, false);
                this.adjustmentRows[i][3].innerText = scoreText(result.adj_vals[i][1], result.adj_vals[i][2]);
            }
        } else {
            this.hoverSection.classList.add('hidden');
            this.adjustmentSection.classList.add('hidden');
            this.placementSection.classList.remove('hidden');
            this.placementTable.innerHTML = '';
            for (let i = 0; i < result.moves.length; i++) {
                const rank = document.createElement('td');
                rank.classList.add('cell-rank');
                rank.innerText = `${i + 1}`;
                const placement = document.createElement('td');
                placement.classList.add('cell-placement');
                setPlacementCell(placement, result.moves[i].position, true, true);
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

    public hideAll() {
        this.scoreSection.classList.add('hidden');
        this.hoverSection.classList.add('hidden');
        this.adjustmentSection.classList.add('hidden');
        this.placementSection.classList.add('hidden');
        this.elapsedSection.classList.add('hidden');
    }
};