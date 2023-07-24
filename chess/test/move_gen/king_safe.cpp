#include <stdexcept>
#include <stdlib.h>
#include <string>

#include <chess/board.hpp>
#include <chess/import.hpp>

using namespace Chess;

int main(int, char *argv[]) {
    Board board = Import::FEN((std::string)argv[1]);
    int expectedBool = std::atoi(argv[2]);

    int kingSafe = board.IsKingSafe();

    if (expectedBool == kingSafe)
        exit(EXIT_SUCCESS);
    else
        throw std::logic_error("Incorrect move count | Expected " + std::to_string(expectedBool) +
                               " - Actual " + std::to_string(kingSafe));
}
