#ifndef CHESS_ZOBRIST
#define CHESS_ZOBRIST

#include <unordered_map>

#include "constants.hpp"

namespace Chess::Zobrist {
    uint64_t FlipSquare(uint64_t hash, Square square, PieceType type, Color color);
    uint64_t FlipCastling(uint64_t hash, Color col, Castling side);
    uint64_t FlipEnPassant(uint64_t hash, Column col);
} // namespace Chess

#endif // CHESS_ZOBRIST
