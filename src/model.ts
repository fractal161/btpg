import { Placement, TetrisState } from './tetris';

// generic class for betatetris models
export interface Model {
    run(state: TetrisState): Promise<Placement>; // TODO: what do we return???
}
