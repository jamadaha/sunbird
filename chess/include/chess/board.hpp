#ifndef CHESS_BOARD
#define CHESS_BOARD

#include <cassert>
#include <string>

#include "internal/bitboard.hpp"
#include "internal/constants.hpp"
#include "internal/move.hpp"
#include "internal/utilities.hpp"
#include "internal/zobrist.hpp"
#include "jank/bit/bit.hpp"
#include "jank/container/fixed_stack.hpp"

namespace Chess {
// Class representing the current state of a game of chess
class Board {
public:
    Board() {
        EP.push(Column::None);
        castling.push({Castling::None, Castling::None});
    }
    // Initialization
    void Initialize();
    // Pieces
    inline PieceType GetType(Square square) const;
    inline int GetPieceCount(Color color, PieceType type) const;
    inline BB GetPiecePos(PieceType type) const;
    inline BB GetPiecePos(Color color, PieceType type) const;
    inline BB GetOccupiedBB() const;
    inline BB GetColorBB(Color color) const;
    inline int GetPly() const;
    inline void PlacePiece(Square square, PieceChar pieceChar);
    inline void PlacePiece(Square square, PieceType type, Color color);
    inline void RemovePiece(Square square, PieceType type, Color color);
    // Moves
    void DoMove(Move &move);
    void UndoMove(Move move);
    // King Safety
    bool IsKingSafe(BB tempOccuracyBoard, BB tempEnemyBoard, BB tempKingBoard) const;
    inline bool IsKingSafe(BB tempOccuracyBoard, BB tempEnemyBoard) const;
    inline bool IsKingSafe(BB tempOccuracyBoard) const;
    inline bool IsKingSafe() const;
    // Castling
    inline bool IsCastlingAllowed(Color color, Castling side) const {
        return castling.top()[(int)color] & side;
    }
    void PushCastling(std::array<Castling, 2> cast) {
        for (int i = 0; i < 2; i++) {
            if ((cast[i] & Castling::King) != (castling.top()[i] & Castling::King)) [[unlikely]]
                zobrist.FlipCastling((Color) i, Castling::King);
            if ((cast[i] & Castling::Queen) != (castling.top()[i] & Castling::Queen)) [[unlikely]]
                zobrist.FlipCastling((Color) i, Castling::Queen);
        }
        castling.push(std::move(cast));
    }
    void PopCastling() {
        const std::array<Castling, 2> cast = castling.top();
        castling.pop();
        for (int i = 0; i < 2; i++) {
            if ((cast[i] & Castling::King) != (castling.top()[i] & Castling::King)) [[unlikely]]
                zobrist.FlipCastling((Color) i, Castling::King);
            if ((cast[i] & Castling::Queen) != (castling.top()[i] & Castling::Queen)) [[unlikely]]
                zobrist.FlipCastling((Color) i, Castling::Queen);
        }
    }
    // EnPassant
    inline Column GetEP() const;
    void PushEP(Column col) {
        if (EP.top() != Column::None) [[unlikely]]
            zobrist.FlipEnPassant(EP.top());
        if (col != Column::None) [[unlikely]]
            zobrist.FlipEnPassant(col);
        EP.push(Column(col));
    }
    void PopEP() {
        if (EP.top() != Column::None) [[unlikely]]
            zobrist.FlipEnPassant(EP.top());
        EP.pop();
        if (EP.top() != Column::None) [[unlikely]]
            zobrist.FlipEnPassant(EP.top());
    }
    // Misc
    inline Color GetColor() const;
    inline Color GetOppColor() const;
    inline Color GetColor(Square sq) const;
    inline uint64_t GetHash() const;
    inline bool IsThreefoldRep() const;
    BB GenerateAttackSquares(Color color) const;
    inline void SetTurn(Color color);
    inline void SwitchTurn();
    inline BB GetMoveCount() const noexcept { return moveCount; }
    inline void ResetMoveCount() noexcept { moveCount = 0; }

private:
    Color turn = Color::None;
    Color oppColor = Color::None;
    BB pieceBB[PIECECOUNT]{0};
    BB colorBB[COLORCOUNT]{0};
    BB occupiedBB = 0;
    std::array<PieceType, SQUARECOUNT> pieceBoard = [] {
        std::array<PieceType, SQUARECOUNT> array;
        for (auto square : SQUARES)
            array[(int)square] = PieceType::None;
        return array;
    }();
    Zobrist zobrist = Zobrist();
    int ply = 0;
    uint64_t moveCount = 0;

    jank::container::fixed_stack<PieceType, 32> captures;
    jank::container::fixed_stack<Column, MAX_PLY> EP;
    jank::container::fixed_stack<std::array<Castling, 2>, MAX_PLY> castling;
};

inline PieceType Board::GetType(Square square) const {
    assert(square != Square::None);
    return pieceBoard[(int)square];
}

inline int Board::GetPieceCount(const Color color, const PieceType type) const {
    assert(type != PieceType::None);
    return jank::bit::popcount(GetPiecePos(color, type));
}

BB Board::GetPiecePos(PieceType type) const {
    assert(type != PieceType::None);
    return pieceBB[(int)type];
}

inline BB Board::GetPiecePos(const Color color, const PieceType type) const {
    assert(type != PieceType::None);
    return pieceBB[(int)type] & colorBB[(int)color];
}

inline BB Board::GetOccupiedBB() const { return occupiedBB; }

inline bool Board::IsKingSafe(BB tempOccuracyBoard, BB tempEnemyBoard) const {
    return IsKingSafe(tempOccuracyBoard, tempEnemyBoard, GetPiecePos(turn, PieceType::King));
}

inline bool Board::IsKingSafe(BB tempOccuracyBoard) const {
    return IsKingSafe(tempOccuracyBoard, GetColorBB(oppColor));
}

inline bool Board::IsKingSafe() const { return IsKingSafe(GetOccupiedBB()); }

inline Color Board::GetColor() const { return turn; }

inline Color Board::GetOppColor() const { return oppColor; }

inline Color Board::GetColor(Square sq) const {
    if (colorBB[(int)Color::White] & sq)
        return Color::White;
    else if (colorBB[(int)Color::Black] & sq)
        return Color::Black;
    else
        return Color::None;
}

inline uint64_t Board::GetHash() const { return zobrist.GetHash(); }

inline int Board::GetPly() const { return ply; }

inline bool Board::IsThreefoldRep() const { return zobrist.IsThreefoldRep(); }

BB Board::GetColorBB(Color color) const { return colorBB[(int)color]; }

inline Column Board::GetEP() const { return EP.top(); }

inline void Board::PlacePiece(Square square, PieceChar pieceChar) {
    PlacePiece(square, Utilities::GetPieceType(pieceChar), Utilities::GetPieceColor(pieceChar));
}

inline void Board::PlacePiece(Square square, PieceType type, Color color) {
    assert(type != PieceType::None);
    pieceBB[(int)type] |= square;
    colorBB[(int)color] |= square;
    occupiedBB |= square;
    pieceBoard[(int)square] = type;
    zobrist.FlipSquare(square, type, color);
    assert(occupiedBB == (colorBB[0] | colorBB[1]));
}

inline void Board::RemovePiece(Square square, PieceType type, Color color) {
    assert(type != PieceType::None);
    pieceBB[(int)type] ^= square;
    colorBB[(int)color] ^= square;
    occupiedBB ^= square;
    pieceBoard[(int)square] = PieceType::None;
    zobrist.FlipSquare(square, type, color);
    assert(occupiedBB == (colorBB[0] | colorBB[1]));
}

void Board::SetTurn(Color color) {
    turn = color;
    oppColor = Utilities::GetOppositeColor(color);
}

void Board::SwitchTurn() {
    turn = Utilities::GetOppositeColor(turn);
    oppColor = Utilities::GetOppositeColor(turn);
}
} // namespace Chess

#endif // CHESS_BOARD
