#include <iostream>

#include "../classes/headers/bit_board.hh"
#include "../classes/headers/board_importer.hh"
#include "../classes/headers/minimax.hh"
#include "../classes/headers/move_gen.hh"
#include "../classes/headers/perft.hh"

int main(int argc, char* argv[]) {
    BitShifts::Init();
    BitBoard board = BitBoard();
    BoardImporter::ImportFEN(&board, "r2q2nr/ppp2kp1/2n5/2bpp2p/5Pb1/2N5/PPPPP1PP/R1BQKB1R w KQ - 0 8");
    MiniMax max = MiniMax(&board);
    Move move = max.GetBestMove(4);
    printf("%s\n", move.ToString().c_str());
}
