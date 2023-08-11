#ifndef CHESS_CONSTANTS
#define CHESS_CONSTANTS

#include <array>
#include <cstdint>

namespace Chess {

using BB = uint64_t;

#define WIDTH 8
#define HEIGHT 8
#define SQUARECOUNT 64

#define MAX_PLY 256

// 218 I believe to be the max number of moves
#define MAXMOVECOUNT 256

#define PIECECOUNT 6
#define COLORCOUNT 2
#define DIRECTIONCOUNT 8

#define PAWNROWWHITE 1
#define PAWNROWBLACK 6

#define EDGE 0xff818181818181ff
#define CORNER 0x8100000000000081

enum class Color { White, Black, None };

enum class PieceType { Pawn, Knight, Bishop, Rook, Queen, King, None };
constexpr std::array<PieceType, PIECECOUNT> PIECES{PieceType::Pawn,   PieceType::Knight,
                                                   PieceType::Bishop, PieceType::Rook,
                                                   PieceType::Queen,  PieceType::King};

enum class PieceChar : char {
    PawnWhite = 'P',
    PawnBlack = 'p',
    KnightWhite = 'N',
    KnightBlack = 'n',
    BishopWhite = 'B',
    BishopBlack = 'b',
    RookWhite = 'R',
    RookBlack = 'r',
    QueenWhite = 'Q',
    QueenBlack = 'q',
    KingWhite = 'K',
    KingBlack = 'k',
    None = 'O'
};

// clang-format off
enum class Square {
    A1,B1,C1,D1,E1,F1,G1,H1,
    A2,B2,C2,D2,E2,F2,G2,H2,
    A3,B3,C3,D3,E3,F3,G3,H3,
    A4,B4,C4,D4,E4,F4,G4,H4,
    A5,B5,C5,D5,E5,F5,G5,H5,
    A6,B6,C6,D6,E6,F6,G6,H6,
    A7,B7,C7,D7,E7,F7,G7,H7,
    A8,B8,C8,D8,E8,F8,G8,H8,
    None
};
// clang-format on

constexpr std::array<Square, SQUARECOUNT> SQUARES = [] {
    auto s = decltype(SQUARES){};
    for (int i = 0; i < 64; i++)
        s[i] = (Square)i;
    return s;
}();

constexpr Square INIT_ROOKPOS[2][2] = {{Square::H1, Square::A1}, {Square::H8, Square::A8}};
constexpr Square INIT_KINGPOS[2] = {Square::E1, Square::E8};
constexpr Square CASTLEPOS_KING[2][2] {{Square::G1, Square::C1}, {Square::G8, Square::C8}};
constexpr Square CASTLEPOS_ROOK[2][2] {{Square::F1, Square::D1}, {Square::F8, Square::D8}};

enum class Row : BB {
    Row1 = 0xff,
    Row2 = 0xff00,
    Row3 = 0xff0000,
    Row4 = 0xff000000,
    Row5 = 0xff00000000,
    Row6 = 0xff0000000000,
    Row7 = 0xff000000000000,
    Row8 = 0xff00000000000000,
    None = 0x0
};

constexpr Row PawnRow[2] = {Row::Row2, Row::Row7};
constexpr Row PushRow[2] = {Row::Row4, Row::Row5};

enum class Column : BB {
    A = 0x101010101010101,
    B = 0x202020202020202,
    C = 0x404040404040404,
    D = 0x808080808080808,
    E = 0x1010101010101010,
    F = 0x2020202020202020,
    G = 0x4040404040404040,
    H = 0x8080808080808080,
    None = 0x0
};

constexpr std::array<Column, 64> COLUMN_BY_SQUARE = {
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H,
    Column::A, Column::B, Column::C, Column::D, Column::E, Column::F, Column::G, Column::H
};

constexpr std::array<int, SQUARECOUNT> COLUMN_INDEX = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7
};

enum class Direction : int {
    North,
    East,
    South,
    West,

    NorthEast,
    NorthWest,
    SouthEast,
    SouthWest,
    None
};

constexpr std::array<Direction, 8> DIRECTIONS = {
    Direction::North,     Direction::East,      Direction::South,     Direction::West,
    Direction::NorthEast, Direction::NorthWest, Direction::SouthEast, Direction::SouthWest};

enum class NotEdge : BB {
    North = 0xffffffffffffff,
    East = 0x7f7f7f7f7f7f7f7f,
    South = 0xffffffffffffff00,
    West = 0xfefefefefefefefe,
    NorthEast = North & East,
    NorthWest = North & West,
    SouthEast = South & East,
    SouthWest = South & West,
};

constexpr NotEdge notEdges[8] = {NotEdge::North,     NotEdge::East,      NotEdge::South,
                                 NotEdge::West,      NotEdge::NorthEast, NotEdge::NorthWest,
                                 NotEdge::SouthEast, NotEdge::SouthWest};

// NOTE: The values of MoveType are used, and should not be changed
enum class MoveType {
    Quiet = 0,
    DoublePawnPush = 1,
    KingCastle = 2,
    QueenCastle = 3,
    Capture = 4,
    EPCapture = 5,
    NPromotion = 8,
    BPromotion = 9,
    RPromotion = 10,
    QPromotion = 11,
    NPromotionCapture = 12,
    BPromotionCapture = 13,
    RPromotionCapture = 14,
    QPromotionCapture = 15
};
constexpr std::array<MoveType, 4> PromotionMoves{MoveType::NPromotion, MoveType::BPromotion,
                                                 MoveType::RPromotion, MoveType::QPromotion};
constexpr std::array<MoveType, 4> PromotionCapturesMoves{
    MoveType::NPromotionCapture, MoveType::BPromotionCapture, MoveType::RPromotionCapture,
    MoveType::QPromotionCapture};

#define PromotionBit 0x8
#define CaptureBit 0x4
#define CastlingBit 0x2

enum class Castling { 
    None, King, Queen, All 
};

inline bool operator&(Castling lhs, Castling rhs) {
    return ((int) lhs & (int) rhs) != 0;
}

inline Castling operator^(Castling lhs, Castling rhs) {
    return (Castling)((int) lhs ^ (int) rhs);
}

#define CASTLING_KING_BIT 0x0
#define CASTLING_QUEEN_BIT 0x1

enum class CastlingBlockSquares : BB {
    KSideWhite = 0x60,
    QSideWhite = 0xe,
    KSideBlack = 0x6000000000000000,
    QSideBlack = 0xe00000000000000
};

enum class CastlingAttackSquares : BB {
    KSideWhite = 0x70,
    QSideWhite = 0x1c,
    KSideBlack = 0x7000000000000000,
    QSideBlack = 0x1c00000000000000
};
}

#endif // CONSTANTS
