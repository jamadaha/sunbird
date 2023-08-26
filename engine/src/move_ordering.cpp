#include <algorithm>
#include <cstring>
#include <engine/internal/move_ordering.hpp>

namespace Chess::Engine::MoveOrdering {
void MVVLVA(const Board &board, MoveList &moves) {
    const Position &pos = board.Pos();
    std::sort(moves.begin(), &moves[moves.attacks()], [pos](Move lhs, Move rhs) {
        const int leftCapture = (int)pos.GetType(lhs.GetTo());
        const int rightCapture = (int)pos.GetType(rhs.GetTo());
        if (leftCapture > rightCapture)
            return true;
        const int leftPiece = (int)pos.GetType(lhs.GetFrom());
        const int rightPiece = (int)pos.GetType(rhs.GetFrom());
        if (leftCapture == rightCapture && leftPiece < rightPiece)
            return true;
        return false;
    });
}

void PVPrioity(const Board &board, const PV &pv, MoveList &moves) {
    size_t pvIndex = board.Ply() - pv.ply;
    if (pvIndex > pv.moves.size() || pv.moves.size() == 0)
        return;
    Move pvMove = pv.moves[pvIndex];
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i] == pvMove) {
            std::memmove(&moves[i], &moves[i + 1], (moves.size() - i) * sizeof(Move));
            std::memmove(&moves[1], &moves[0], (moves.size() - 1) * sizeof(Move));
            moves[0] = pvMove;
            break;
        }
    }
}

void TTPrioity(Move move, MoveList &moves) {
    if (move.GetValue() == 0)
        return;
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i] == move) {
            std::memmove(&moves[i], &moves[i + 1], (moves.size() - i) * sizeof(Move));
            std::memmove(&moves[1], &moves[0], (moves.size() - 1) * sizeof(Move));
            moves[0] = move;
            break;
        }
    }
}

void All(const Board &board, Move ttMove, const PV &pv, MoveList &moves) { 
    MVVLVA(board, moves);
    TTPrioity(ttMove, moves);
    PVPrioity(board, pv, moves);
}
} // namespace Chess::Engine::MoveOrdering
