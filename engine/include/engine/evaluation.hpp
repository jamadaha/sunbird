#ifndef CHESS_ENGINE_EVALUATION
#define CHESS_ENGINE_EVALUATION

#include <chess/board.hpp>
#include <chess/internal/types.hpp>

namespace Chess::Engine::Evaluation {
int Eval(const Position &pos);
int EvalNoMove(const Position &pos);
} // namespace Chess::Engine::Evaluation

#endif // ENGINE_EVALUATION
