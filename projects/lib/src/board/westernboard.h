/*
    This file is part of Sylvan.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sylvan.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WESTERNBOARD_H
#define WESTERNBOARD_H

#include "board.h"

namespace Chess {

class WesternZobrist;

/*! Basic piece types for ambiguity type. */
enum AmbiguityType
{
    None,
    QianHou,
    QianZhongHou,
    SuperHou
};

/*! Stores a move \a type and a move direction \a file for a Pawn move */
struct AmbiguityStep { AmbiguityType ambiguityType; int pieceType; };

/*!
 * \brief A board for western chess variants
 *
 * WesternBoard serves as the overclass for all western variants.
 * In addition to possibly unique pieces, a western variant has
 * the same pieces as standard chess, the same rules for castling,
 * en-passant capture, promotion, etc.
 *
 * WesternBoard implements the rules of standard chess, including
 * check, checkmate, stalemate, promotion, 3-fold repetition,
 * 50 move rule and draws by insufficient material.
 */
class LIB_EXPORT WesternBoard : public Board
{
    Q_DECLARE_TR_FUNCTIONS(WesternBoard)

public:
    /*! Basic piece types for western variants. */
    enum WesternPieceType
    {
        Pawn = 1,	//!< Pawn
        Elephant,	//!< Bishop
        Guard,		//!< Advisor
        Cannon,		//!< Cannon
        Horse,		//!< Knight
        Rook,		//!< Rook
        King		//!< King
    };

    /*! Creates a new WesternBoard object. */
    WesternBoard(WesternZobrist* zobrist);

    // Inherited from Board
    virtual int width() const;
    virtual int height() const;
    virtual Result result();
    virtual int reversibleMoveCount() const;

protected:

    /*!
     * Helper function for Pawn moves. Returns the count of moves
     * of the given \a type that are specified in pawnSteps.
     */
    AmbiguityStep pieceAmbiguity(Side side,
                                 int file) const;
    /*!
         * Returns true if both counts of kings given by
         * \a redKings and \a blackKings are correct.
         * WesternBoard expects exactly one king per side.
         * \sa AntiBoard
         * \sa HordeBoard
         */
    virtual bool kingsCountAssertion(int redKings,
                                     int blackKings) const;
    /*! Returns the king square of \a side. */
    int kingSquare(Side side) const;

    /*!
         * Returns true if \a side is under attack at \a square.
         * If \a square is 0, then the king square is used.
         */
    virtual bool inCheck(Side side) const;

    /*!
         * Returns FEN extensions. The default is an empty string.
         *
         * This function is called by fenString() via vFenString().
         * Returns additional parts of the current position's (extended)
         * FEN string which succeed the en passant field.
         */
    virtual QString vFenIncludeString(FenNotation notation) const;

    // Inherited from Board
    virtual void vInitialize();
    virtual QString vFenString(FenNotation notation) const;
    virtual bool vSetFenString(const QStringList& fen);
    virtual QString lanMoveString(const Move& move);
    virtual QString standardMoveString(const Move& move);
    virtual Move moveFromLanString(const QString& str);
    virtual void vMakeMove(const Move& move,
                           BoardTransition* transition);
    virtual void vUndoMove(const Move& move);
    virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
                                       int pieceType,
                                       int square) const;
    virtual bool vIsLegalMove(const Move& move);
    virtual bool isLegalPosition();
    virtual int captureType(const Move& move) const;

    virtual Move moveFromStandardString(const QString& str);

private:
    // Data for reversing/unmaking a move
    struct MoveData
    {
        Piece capture;
        int reversibleMoveCount;
    };

    int m_arwidth;
    int m_sign;
    int m_kingSquare[2];
    int m_plyOffset;
    int m_reversibleMoveCount;

    const WesternZobrist* m_zobrist;
    QVector<MoveData> m_history;
    QVarLengthArray<int> m_BPawnOffsets;
    QVarLengthArray<int> m_RPawnOffsets;
    QVarLengthArray<int> m_HorseOffsets;
    QVarLengthArray<int> m_HorseLegOffsets;
    QVarLengthArray<int> m_HorseCheckLegOffsets;

    QVarLengthArray<int> m_ElephantOffsets;
    QVarLengthArray<int> m_ElephantEyeOffsets;

    QVarLengthArray<int> m_RookOffsets;
    QVarLengthArray<int> m_GuardOffsets;

    QVarLengthArray<QString> strnumCn;
    QVarLengthArray<QString> strnumEn;
    QVarLengthArray<QString> strnumName;
};

} // namespace Chess
#endif // WESTERNBOARD_H
