#include <stdexcept>
#include <string>
#include <stdlib.h>

#include "../../../classes/headers/constants.hh"
#include "../../../classes/headers/bit_board.hh"
#include "../../../classes/headers/board_importer.hh"
#include "../../../classes/headers/bit_evaluator.hh"

int main(int argc, char* argv[]) {
    BitBoard board = BitBoard();
    BoardImporter::ImportFEN(&board, (std::string) argv[2]);
    int expectedEval = std::atoi(argv[1]);
    BitEvaluator evaluator = BitEvaluator();

    int eval = evaluator.EvaluatePieceCount(board);

    if (eval == std::atoi(argv[1]))
        exit(EXIT_SUCCESS);
    else
        throw std::logic_error("Incorrect Evaluation | Expected " + (std::string)argv[1] + " - Actual " + std::to_string(eval));
}