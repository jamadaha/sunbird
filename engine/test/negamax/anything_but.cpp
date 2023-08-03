#include <cstdlib>
#include <stdexcept>
#include <string>

#include <chess/board.hpp>
#include <chess/import.hpp>
#include <engine/negamax.hpp>

using namespace Chess;

int main(int, char *argv[]) {
    std::string fen = std::string(argv[1]);
    Board board = Import::FEN(fen);
    std::string expectedMove = argv[3];

    auto move = Engine::Negamax::GetBestMove(board, std::atoi(argv[2]));

    if (expectedMove != std::get<Move>(move).ToString())
        exit(EXIT_SUCCESS);
    else
        throw std::logic_error("Incorrect Move | Expected " + expectedMove + " - Actual " +
                               std::get<Move>(move).ToString());
}
