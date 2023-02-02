#ifndef EVALUATOR
#define EVALUATOR

#include "board.hh"
#include "constants.hh"

class Evaluator {
public:
    struct Stats {
        U64 evalCount = 0;
        U64 noMoveEvalCount = 0;
    };
    // Init
    explicit Evaluator() : sPieceValues(pieceValues) {};
    explicit Evaluator(std::array<int, PIECECOUNT> pValues) : sPieceValues(pValues) {};
    ~Evaluator();
    // Evaluation
    int EvaluatePieceCount(const Board &board) const;
    static inline int EvaluatePositionValue(const Board &board);
    static int EvaluatePositionValue(const Board &board, Color color);
    static inline int EvaluatePawnStructure(const Board &board);
    static int EvaluatePawnStructure(const Board &board, Color color);
    inline int Evaluate(const Board &board) const;
    static inline int EvaluateNoMoves(const Board &board, bool isKingSafe) ;
    static inline int SideModifier(const Board &board, int value);
private:
    const std::array<int, PIECECOUNT> sPieceValues;
};

inline int Evaluator::Evaluate(const Board &board) const {
    int value = EvaluatePieceCount(board) + EvaluatePositionValue(board)/* + EvaluatePawnStructure(board)*/;
    
    return SideModifier(board, value);
}

inline int Evaluator::EvaluateNoMoves(const Board &board, const bool isKingSafe) {
    // Checkmate
    if (!isKingSafe)
        return -(int) PieceValue::Inf;
    // Stalemate
    else
        return 0;
}

inline int Evaluator::EvaluatePositionValue(const Board &board) {
    return EvaluatePositionValue(board, Color::White) - EvaluatePositionValue(board, Color::Black);
}

inline int Evaluator::EvaluatePawnStructure(const Board &board) {
    return EvaluatePawnStructure(board, Color::White) - EvaluatePawnStructure(board, Color::Black);
}

inline int Evaluator::SideModifier(const Board &board, const int value) {
    if (board.GetColor() == Color::Black)
        return -value;
    else
        return value;
}

#endif // EVALUATOR
