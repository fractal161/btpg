import { TetrisState } from './tetris';
import { Parameters } from './params';

// generic class for betatetris models
export interface Model {
    run(state: TetrisState, params: Parameters): Promise<any>; // TODO: what do we return???
}
