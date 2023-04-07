#include <stdexcept>
#include <string>
#include <cstdlib>

#include "board.hh"
#include "board_importer.hh"
#include "perft.hh"

int main(int, char* argv[]) {
    BitShifts::Init();
    int expectedLeafCount = std::atoi(argv[1]);

    std::string FEN = std::string(argv[2]);
    Board board = BoardImporter::ImportFEN(FEN);

    int depth = std::atoi(argv[3]);

    Perft perft = Perft(std::make_shared<Board>(board));
    int actualLeafCount = perft.RunFromPosition(depth);

    if (expectedLeafCount == actualLeafCount)
        exit(EXIT_SUCCESS);
    else
        throw std::logic_error("Incorrect leaf count | Expected " + std::to_string(expectedLeafCount) + " - Actual " + std::to_string(actualLeafCount));
}