#ifndef ENGINE_EVALUATION
#define ENGINE_EVALUATION

#include "internal/values.hpp"
#include <chess/board.hpp>
#include <chess/internal/constants.hpp>

namespace Engine::Evaluation {
int EvalMaterial(const Board &board);
int EvalPosition(const Board &board);
int EvalPosition(const Board &board, Color color);
int Eval(const Board &board);
int EvalNoMove(bool isKingSafe);
} // namespace Engine::Evaluation

#endif // ENGINE_EVALUATION
