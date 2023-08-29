#include "engine/internal/move_ordering.hpp"
#include "engine/internal/tt.hpp"
#include <chess/internal/move_list.hpp>
#include <chess/move_gen.hpp>
#include <cstring>
#include <engine/evaluation.hpp>
#include <engine/search.hpp>

using namespace Chess::MoveGen;

namespace Chess::Engine::Search::Internal {
namespace {
bool AB(int score, int &alpha, int beta) {
    if (score >= beta)
        return true;
    if (score > alpha)
        alpha = score;
    return false;
}
} // namespace
int Quiesce(Board &board, int alpha, int beta, const PV &pv) {
    int standPat = Evaluation::Eval(board.Pos());
    if (AB(standPat, alpha, beta))
        return beta;

    MoveList moves = GenerateMoves<GenType::Attack>(board.Pos());
    MoveOrdering::MVVLVA(board, moves);
    MoveOrdering::PVPrioity(board, pv, moves);
    for (auto move : moves) {
        board.MakeMove(move);
        int score = -Quiesce(board, -beta, -alpha, pv);
        board.UndoMove();
        if (AB(score, alpha, beta))
            return beta;
    }

    return alpha;
}

int Negamax(Board &board, int alpha, int beta, int depth, int searchDepth, const PV &pv,
            SearchLimit *limit) {
    if (limit != nullptr && depth > 1 && limit->Reached())
        limit->Exit();
    if (board.IsThreefoldRepetition())
        return 0;

    const uint64_t hash = board.Pos().GetHash();
    auto tt = TT::Probe(hash, depth, searchDepth, alpha, beta);
    if (tt.score != TT::ProbeFail)
        return tt.score;

    if (depth == 0)
        return Quiesce(board, alpha, beta, pv);


    int ttBound = TT::ProbeUpper;
    MoveList moves = GenerateMoves(board.Pos());
    if (moves.empty())
        return Evaluation::EvalNoMove(board.Pos());

    MoveOrdering::All(board, tt.move, pv, moves);
    Move bm;
    for (auto move : moves) {
        board.MakeMove(move);
        int value =
            -Negamax(board, -beta, -alpha, depth - 1, searchDepth + 1, pv, limit);
        board.UndoMove();
        if (value >= beta) {
            TT::StoreEval(hash, depth, searchDepth, beta, TT::ProbeLower, move);
            return beta;
        }
        if (value > alpha) {
            ttBound = TT::ProbeExact;
            alpha = value;
            bm = move;
        }
    }

    TT::StoreEval(hash, depth, searchDepth, alpha, ttBound, bm);
    return alpha;
}
} // namespace Chess::Engine::Search::Internal
