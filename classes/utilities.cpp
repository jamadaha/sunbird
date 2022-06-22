#include "headers/utilities.hh"

PieceChar Utilities::GetPieceChar(char c) {
    return (PieceChar) c;
}

PieceChar Utilities::GetPieceChar(PieceType type, Color color) {
    switch(type) {
        case PieceType::Bishop:
            return (color == Color::White) ? PieceChar::BishopWhite : PieceChar::BishopBlack;
        case PieceType::King:
            return (color == Color::White) ? PieceChar::KingWhite : PieceChar::KingBlack;
        case PieceType::Knight:
            return (color == Color::White) ? PieceChar::KnightWhite : PieceChar::KnightBlack;
        case PieceType::Pawn:
            return (color == Color::White) ? PieceChar::PawnWhite : PieceChar::PawnBlack;
        case PieceType::Queen:
            return (color == Color::White) ? PieceChar::QueenWhite : PieceChar::QueenBlack;
        case PieceType::Rook:
            return (color == Color::White) ? PieceChar::RookWhite : PieceChar::RookBlack;
        default:
            return PieceChar::None;
    }
}

PieceType Utilities::GetPieceType(PieceChar pieceChar) {
    switch (pieceChar)
    {
    case PieceChar::BishopBlack:
    case PieceChar::BishopWhite:
        return PieceType::Bishop;

    case PieceChar::KingBlack :
    case PieceChar::KingWhite:
        return PieceType::King;

    case PieceChar::KnightBlack:
    case PieceChar::KnightWhite:
        return PieceType::Knight;

    case PieceChar::PawnBlack:
    case PieceChar::PawnWhite :
        return PieceType::Pawn;

    case PieceChar::QueenBlack:
    case PieceChar::QueenWhite:
        return PieceType::Queen;

    case PieceChar::RookBlack:
    case PieceChar::RookWhite:
        return PieceType::Rook;

    default:
        return PieceType::None;
    }
}

Color Utilities::GetPieceColor(PieceChar pieceChar) {
    if (std::isupper((char) pieceChar))
        return Color::White;
    else
        return Color::Black;
}

Square Utilities::GetSquare(int x, int y) {
    return (Square) (x + 8 * y);
}

int Utilities::GetPieceValue(PieceType type) {
    switch(type) {
        case PieceType::Bishop:
            return (int) PieceValue::Bishop;
        case PieceType::King:
            return (int) PieceValue::King;
        case PieceType::Knight:
            return (int) PieceValue::Knight;
        case PieceType::Pawn:
            return (int) PieceValue::Pawn;
        case PieceType::Queen:
            return (int) PieceValue::Queen;
        case PieceType::Rook:
            return (int) PieceValue::Rook;
        default:
            return (int) PieceValue::None;
    }
}

// This implementation is from https://www.chessprogramming.org/Population_Count
// Specifically the one titled "Brian Kerninghan"
int Utilities::PopCount(U64 x) {
    int count = 0;
    while (x) {
        count++;
        x &= x - 1;
    }
    return count;
}
int Utilities::LSB(U64 x) {
    U64 lsb = ffsll(x) - 1;
    return lsb;
}

int Utilities::LSB_Pop(U64* x) {
    U64 lsb = LSB(*x);
    *x ^= (C64(0) << lsb);
    return lsb;
}

U64 Utilities::NotEdge(Direction dir) {
    switch (dir)
    {
    case Direction::North:
        return (U64) NotEdge::North;
    case Direction::East:
        return (U64) NotEdge::East;
    case Direction::South:
        return (U64) NotEdge::South;
    case Direction::West:
        return (U64) NotEdge::West;
    case Direction::NorthEast:
        return (U64) NotEdge::North & (U64) NotEdge::East;
    case Direction::NorthWest:
        return (U64) NotEdge::North & (U64) NotEdge::West;
    case Direction::SouthEast:
        return (U64) NotEdge::South & (U64) NotEdge::East;
    case Direction::SouthWest:
        return (U64) NotEdge::South & (U64) NotEdge::West;
    }
    return 0;
}
