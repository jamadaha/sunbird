#include "chess/internal/bitboard.hpp"
#include "jank/bit/bit.hpp"
#include <chess/internal/constants.hpp>
#include <chess/internal/utilities.hpp>
#include <chess/move_gen.hpp>
#include <functional>

namespace Chess::MoveGen {
namespace {

// HACK: This needs to be refactored
template <GenType gType> void GenerateKingMoves(const Position &pos, Color color, MoveList &moves) {
    constexpr static CastlingBlockSquares castlingBlock[2][2] = {
        {CastlingBlockSquares::KSideWhite, CastlingBlockSquares::QSideWhite},
        {CastlingBlockSquares::KSideBlack, CastlingBlockSquares::QSideBlack}};
    constexpr static CastlingAttackSquares castlingAttack[2][2] = {
        {CastlingAttackSquares::KSideWhite, CastlingAttackSquares::QSideWhite},
        {CastlingAttackSquares::KSideBlack, CastlingAttackSquares::QSideBlack}};
    const Color oppColor = Utilities::GetOppositeColor(color);
    const auto attackedSquares = pos.GenerateAttackSquares(oppColor);
    const Square kingPos = (Square)jank::bit::lsb(pos.GetPieces(color, PieceType::King));
    if constexpr (gType == GenType::Quiet || gType == GenType::All) {
        BB qMoves = Ring(kingPos, 1) & ~pos.GetPieces() & ~attackedSquares;
        while (qMoves) {
            Square move = (Square)jank::bit::lsb_pop(qMoves);
            if (pos.IsKingSafe((pos.GetPieces() ^ kingPos | move),
                                 pos.GetPieces(oppColor), ToBB(move)))
                moves << Move(MoveType::Quiet, kingPos, move);
        }
        if (pos.AllowsCastling(Castling::King, color) &&
            !(pos.GetPieces() & (BB)castlingBlock[(int)color][(int)Castling::King - 1]) &&
            !(attackedSquares & (BB)castlingAttack[(int)color][(int)Castling::King - 1]))
            moves << Move(MoveType::KingCastle, kingPos,
                          (Square)jank::bit::lsb(
                              Shift<Direction::East>(Shift<Direction::East>(ToBB(kingPos)))));
        if (pos.AllowsCastling(Castling::Queen, color) &&
            !(pos.GetPieces() & (BB)castlingBlock[(int)color][(int)Castling::Queen - 1]) &&
            !(attackedSquares & (BB)castlingAttack[(int)color][(int)Castling::Queen - 1]))
            moves << Move(MoveType::QueenCastle, kingPos,
                          (Square)jank::bit::lsb(
                              Shift<Direction::West>(Shift<Direction::West>(ToBB(kingPos)))));
    }
    if constexpr (gType == GenType::Attack || gType == GenType::All) {
        BB aMoves = Ring(kingPos, 1) & pos.GetPieces(oppColor) & ~attackedSquares;
        while (aMoves) {
            Square move = (Square)jank::bit::lsb_pop(aMoves);
            if (pos.IsKingSafe(pos.GetPieces() ^ kingPos,
                                 pos.GetPieces(oppColor) ^ move, ToBB(move)))
                moves << Move(MoveType::Capture, kingPos, move);
        }
    }
}

// HACK: This needs to be refactored
template <GenType gType> void GeneratePawnMoves(const Position &pos, Color color, MoveList &moves) {
    constexpr Direction dirs[2] = {Direction::North, Direction::South};
    Direction dir = dirs[(int)color];
    const Color oppColor = Utilities::GetOppositeColor(color);
    BB pieces = pos.GetPieces(color, PieceType::Pawn);
    while (pieces) {
        Square piece = (Square)jank::bit::lsb_pop(pieces);
        if constexpr (gType == GenType::Quiet || gType == GenType::All) {
            BB to = Ray(piece, dir) & Ring(piece, 1);
            if (!(to & pos.GetPieces())) {
                if (pos.IsKingSafe(pos.GetPieces() ^ piece | to)) {
                    if (ToBB(piece) & (BB)PawnRow[(int)oppColor])
                        for (const auto prom : PromotionMoves)
                            moves << Move(prom, piece, (Square)jank::bit::lsb(to));
                    else
                        moves << Move(MoveType::Quiet, piece, (Square)jank::bit::lsb(to));
                }
                to = Ray(piece, dir) & Ring(piece, 2);
                if (piece & (BB)PawnRow[(int)color] && !(to & pos.GetPieces()))
                    if (pos.IsKingSafe(pos.GetPieces() ^ piece | to))
                        moves << Move(MoveType::DoublePawnPush, piece,
                                      (Square)jank::bit::lsb(to));
            }
        }
        if constexpr (gType == GenType::Attack || gType == GenType::All) {
            BB attacks = PawnAttacks(piece, color) & pos.GetPieces(oppColor);
            while (attacks) {
                Square attack = (Square)jank::bit::lsb_pop(attacks);
                assert(pos.GetType((Square)attack) != PieceType::None);
                if (pos.IsKingSafe((pos.GetPieces() ^ piece) | attack,
                                     pos.GetPieces(oppColor) ^ attack)) {
                    if (piece & (BB)PawnRow[(int)oppColor])
                        for (const auto prom : PromotionCapturesMoves)
                            moves << Move(prom, piece, attack);
                    else
                        moves << Move(MoveType::Capture, piece, attack);
                }
            }
            BB attack = (BB)PawnAttacks(piece, color) &
                        (BB)((color == Color::White) ? Row::Row6 : Row::Row3) & (BB)pos.GetEP();
            if (attack) {
                Square sq = (Square)jank::bit::lsb_pop(attack);
                Square captured = (Square)jank::bit::lsb(Shift(ToBB(sq), dirs[(int)oppColor]));
                if (pos.IsKingSafe((pos.GetPieces() ^ piece ^ captured) |
                                         sq,
                                     pos.GetPieces(oppColor) ^ captured | sq))
                    moves << Move(MoveType::EPCapture, piece, sq);
            }
        }
    }
}

// Generates possible quiet moves for pieces of type pType
template <PieceType pType> void GenerateQuiet(const Position &pos, Color color, MoveList &moves) {
    static_assert(pType != PieceType::King && pType != PieceType::Pawn);

    BB pieces = pos.GetPieces(color, pType);
    while (pieces) {
        Square piece = (Square)jank::bit::lsb_pop(pieces);
        BB unblocked = Attacks(piece, pType);
        for (int offset = 1; offset < 8; ++offset) {
            BB ring = Ring(piece, offset);
            BB potMoves = ring & unblocked;
            BB blockers = potMoves & pos.GetPieces();
            potMoves ^= blockers;

            while (blockers)
                unblocked = unblocked & ~Ray(piece, (Square)jank::bit::lsb_pop(blockers));

            while (potMoves) {
                const Square sq = (Square)jank::bit::lsb_pop(potMoves);
                if (pos.IsKingSafe((pos.GetPieces() ^ piece | sq)))
                    moves << Move(MoveType::Quiet, piece, sq);
            }
        }
    }
};

// Generates possible attack moves for pieces of type pType
template <PieceType pType> void GenerateAttack(const Position &pos, Color color, MoveList &moves) {
    static_assert(pType != PieceType::King && pType != PieceType::Pawn);
    Color oppColor = Utilities::GetOppositeColor(color);

    BB pieces = pos.GetPieces(color, pType);
    while (pieces) {
        Square piece = (Square)jank::bit::lsb_pop(pieces);
        BB unblocked = Attacks(piece, pType);
        for (int offset = 1; offset < 8; ++offset) {
            BB ring = Ring(piece, offset);
            BB blockers = ring & unblocked & pos.GetPieces();
            BB tempBlockers = blockers;
            while (tempBlockers)
                unblocked = unblocked & ~Ray(piece, (Square)jank::bit::lsb_pop(tempBlockers));
            blockers &= pos.GetPieces(oppColor);
            while (blockers) {
                const Square blocker = (Square)jank::bit::lsb_pop(blockers);
                if (pos.IsKingSafe((pos.GetPieces() ^ piece) | blocker,
                                     pos.GetPieces(oppColor) ^ blocker))
                    moves << Move(MoveType::Capture, (Square)piece, (Square)blocker);
            }
        }
    }
};
} // namespace

template <GenType gType, PieceType pType>
void Generate(const Position &pos, Color color, MoveList &moves) {
    if constexpr (gType == GenType::All) {
        if constexpr (pType == PieceType::King)
            GenerateKingMoves<GenType::All>(pos, color, moves);
        else if constexpr (pType == PieceType::Pawn)
            GeneratePawnMoves<GenType::All>(pos, color, moves);
        else {
            GenerateAttack<pType>(pos, color, moves);
            GenerateQuiet<pType>(pos, color, moves);
        }

    } else if constexpr (gType == GenType::Attack) {
        if constexpr (pType == PieceType::King)
            GenerateKingMoves<GenType::Attack>(pos, color, moves);
        else if constexpr (pType == PieceType::Pawn)
            GeneratePawnMoves<GenType::Attack>(pos, color, moves);
        else
            GenerateAttack<pType>(pos, color, moves);
    } else if constexpr (gType == GenType::Quiet) {
        if constexpr (pType == PieceType::King)
            GenerateKingMoves<GenType::Quiet>(pos, color, moves);
        else if constexpr (pType == PieceType::Pawn)
            GeneratePawnMoves<GenType::Quiet>(pos, color, moves);
        else
            GenerateQuiet<pType>(pos, color, moves);
    }
}
// clang-format off
template void Generate<GenType::All, PieceType::Pawn>(const Position &, Color, MoveList &moves);
template void Generate<GenType::All, PieceType::Knight>(const Position &, Color, MoveList &moves);
template void Generate<GenType::All, PieceType::Bishop>(const Position &, Color, MoveList &moves);
template void Generate<GenType::All, PieceType::Rook>(const Position &, Color, MoveList &moves);
template void Generate<GenType::All, PieceType::Queen>(const Position &, Color, MoveList &moves);
template void Generate<GenType::All, PieceType::King>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Quiet, PieceType::Pawn>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Quiet, PieceType::Knight>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Quiet, PieceType::Bishop>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Quiet, PieceType::Rook>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Quiet, PieceType::Queen>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Quiet, PieceType::King>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Attack, PieceType::Pawn>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Attack, PieceType::Knight>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Attack, PieceType::Bishop>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Attack, PieceType::Rook>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Attack, PieceType::Queen>(const Position &, Color, MoveList &moves);
template void Generate<GenType::Attack, PieceType::King>(const Position &, Color, MoveList &moves);
// clang-format on
}; // namespace Chess::MoveGen
