#ifndef PERFT
#define PERFT

#define DEBUG_MOVE_TYPE_COUNT true
#define DEBUG_PRINT_MOVES true

#include <string>

#include "constants.hh"
#include "board.hh"
#include "move_gen.hh"
#include "move.hh"

class Perft {
public:
    Perft(Board* board);
    int RunFromMove(Move move, int depth);
    int RunFromPosition(int depth);

private:
    Board* board;
    MoveGen* moveGens[2];

    unsigned int moveTypeCount[16] = { 0 };

    int Run(int depth, U64 attackedSquares[2]);

};
#endif // PERFT
