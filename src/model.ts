import { Placement, TetrisState } from './tetris';
import { Parameters } from './params';

// generic class for betatetris models
export interface Model {
    run(state: TetrisState, params: Parameters): Promise<Placement>; // TODO: what do we return???
}
