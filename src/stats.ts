import '../styles/style.css';
import { Select, wrapSelectInField } from './components';

const PERCENTILES = [2, 5, 10, 20, 50, 100, 300, 500, 700, 900, 950, 980, 990, 995, 998];
const LINE_LABELS = ['Average', 'Stddev', 'Median', '>=130 (%)', '>=230 (%)', '>=330 (%)'];
const SCORE_LABELS = ['Average', 'Stddev'].concat(PERCENTILES.map(p => `p${p/10}`));

const initAnalysisTable = async () => {
    // load data
    const response = await fetch("/btpg/stats.json");
    const data = await response.json();

    // init fields
    const paramsSection = document.getElementById('params-select')! as HTMLDivElement;
    const hzSelect = new Select(
        'hz-select',
        ['10hz', '12hz', '15hz', '20hz', '24hz', '30hz', 'slow5'],
        ['10hz', '12hz', '15hz', '20hz', '24hz', '30hz', 'slow 5 tap'],
        4,
        false,
    );
    const hzField = wrapSelectInField(hzSelect.element, 'Tap speed:');

    const REACTION_TIMES = [0, 18, 21, 24, 30, 61];
    const reactionSelect = new Select(
        'reaction-select',
        REACTION_TIMES,
        REACTION_TIMES.map((delay) => delay == 61 ? 'No adj' :
            Math.round((delay * 1000) / 60).toString() + 'ms (' + delay + 'f)'),
        2,
        false,
    );
    const reactionField = wrapSelectInField(reactionSelect.element, 'Reaction time:');

    paramsSection.appendChild(hzField);
    paramsSection.appendChild(reactionField);

    // init table
    const table = document.getElementById('stats-table')! as HTMLTableSectionElement;
    const tableCells: Array<Array<HTMLTableCellElement>> = [[], [], [], [], [], []];
    const generateTable = (title: string, labels: Array<string>) => {
        for (let i = 0; i < labels.length; i++) {
            const row = document.createElement('tr');
            if (i == 0) {
                const titleCell = document.createElement('td');
                titleCell.innerText = title;
                titleCell.rowSpan = labels.length;
                titleCell.classList.add('stat-title');
                row.appendChild(titleCell);
            }
            const labelCell = document.createElement('td');
            labelCell.innerText = labels[i];
            labelCell.classList.add('stat-title');
            row.appendChild(labelCell);
            for (let j = 0; j < 6; j++) {
                const cell = document.createElement('td');
                cell.classList.add('stat-cell');
                row.appendChild(cell);
                tableCells[j].push(cell);
            }
            table.appendChild(row);
        }
    }
    generateTable('Lines', LINE_LABELS);
    generateTable('Score', SCORE_LABELS);

    const formatPercent = (percent: number) => {
        if (percent < 0.02) return '<0.02';
        if (percent > 99.98) return '>99.98';
        return percent.toFixed(2);
    };
    const loadData = (hz: string, reaction: number) => {
        const curData = data[`${hz}_${reaction}`];
        for (let i = 0; i < 6; i++) {
            const [scoreStat, percentiles, lineStat] = curData[i];
            for (let j = 0; j < 2; j++) {
                tableCells[i][j].innerText = lineStat[j].toFixed(2);
            }
            tableCells[i][2].innerText = lineStat[2].toFixed(0);
            for (let j = 3; j < 6; j++) {
                tableCells[i][j].innerText = formatPercent(lineStat[j] * 100);
            }
            for (let j = 0; j < 2; j++) {
                tableCells[i][j + 6].innerText = Math.round(scoreStat[j]).toLocaleString('en-US');
            }
            for (let j = 0; j < PERCENTILES.length; j++) {
                tableCells[i][j + 8].innerText = Math.round(percentiles[PERCENTILES[j]]).toLocaleString('en-US');
            }
        }
    };
    loadData(hzSelect.value, reactionSelect.value);

    hzSelect.onchange = (_, value) => {
        loadData(value, reactionSelect.value);
    }
    reactionSelect.onchange = (_, value) => {
        loadData(hzSelect.value, value);
    };
};

initAnalysisTable();
