/*
    This file is part of Sylvan.
    Copyright (C) 2008-2018 The Cute Chess Authors
    Copyright (C) 2019-2020 Wilbert Lee

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

#include <QVariant>
#include <QStringList>

#include "westernboard.h"
#include "westernzobrist.h"
#include "boardtransition.h"

namespace Chess {

WesternBoard::WesternBoard(WesternZobrist* zobrist)
    : Board(zobrist),
      m_arwidth(0),
      m_sign(1),
      m_plyOffset(0),
      m_reversibleMoveCount(0),
      m_zobrist(zobrist)
{
    setPieceType(Pawn, tr("pawn"), "P");
    setPieceType(Horse, tr("horse"), "N");
    setPieceType(Elephant, tr("elephant"), "B");
    setPieceType(Guard, tr("guard"), "A");
    setPieceType(Rook, tr("rook"), "R");
    setPieceType(Cannon, tr("cannon"), "C");
    setPieceType(King, tr("king"), "K");
}

int WesternBoard::width() const
{
    return 9;
}

int WesternBoard::height() const
{
    return 10;
}

bool WesternBoard::kingsCountAssertion(int redKings, int blackKings) const
{
    return redKings == 1 && blackKings == 1;
}

void WesternBoard::vInitialize()
{
    m_arwidth = width() + 2;

    m_kingSquare[Side::Red] = 0;
    m_kingSquare[Side::Black] = 0;

    m_BPawnOffsets.resize(3);
    m_BPawnOffsets[0] = m_arwidth;
    m_BPawnOffsets[1] = -1;
    m_BPawnOffsets[2] = 1;

    m_RPawnOffsets.resize(3);
    m_RPawnOffsets[0] = -m_arwidth;
    m_RPawnOffsets[1] = -1;
    m_RPawnOffsets[2] = 1;

    m_HorseLegOffsets.resize(8);
    m_HorseLegOffsets[0] = -m_arwidth;
    m_HorseLegOffsets[1] = -m_arwidth;
    m_HorseLegOffsets[2] = -1;
    m_HorseLegOffsets[3] = +1;
    m_HorseLegOffsets[4] = -1;
    m_HorseLegOffsets[5] = +1;
    m_HorseLegOffsets[6] = +m_arwidth;
    m_HorseLegOffsets[7] = +m_arwidth;

    m_HorseOffsets.resize(8);
    m_HorseOffsets[0] = -2 * m_arwidth - 1;
    m_HorseOffsets[1] = -2 * m_arwidth + 1;
    m_HorseOffsets[2] = -m_arwidth - 2;
    m_HorseOffsets[3] = -m_arwidth + 2;
    m_HorseOffsets[4] = m_arwidth - 2;
    m_HorseOffsets[5] = m_arwidth + 2;
    m_HorseOffsets[6] = 2 * m_arwidth - 1;
    m_HorseOffsets[7] = 2 * m_arwidth + 1;

    m_HorseCheckLegOffsets.resize(8);
    m_HorseCheckLegOffsets[0] = -m_arwidth-1;
    m_HorseCheckLegOffsets[1] = -m_arwidth+1;
    m_HorseCheckLegOffsets[2] = -m_arwidth -1;
    m_HorseCheckLegOffsets[3] = -m_arwidth +1;
    m_HorseCheckLegOffsets[4] = m_arwidth -1;
    m_HorseCheckLegOffsets[5] = m_arwidth +1;
    m_HorseCheckLegOffsets[6] = +m_arwidth-1;
    m_HorseCheckLegOffsets[7] = +m_arwidth+1;

    m_ElephantOffsets.resize(4);
    m_ElephantOffsets[0] = -2 * m_arwidth - 2;
    m_ElephantOffsets[1] = -2 * m_arwidth + 2;
    m_ElephantOffsets[2] = 2 * m_arwidth - 2;
    m_ElephantOffsets[3] = 2 * m_arwidth + 2;

    m_ElephantEyeOffsets.resize(4);
    m_ElephantEyeOffsets[0] = -m_arwidth - 1;
    m_ElephantEyeOffsets[1] = -m_arwidth + 1;
    m_ElephantEyeOffsets[2] = m_arwidth - 1;
    m_ElephantEyeOffsets[3] = m_arwidth + 1;

    m_RookOffsets.resize(4);
    m_RookOffsets[0] = -m_arwidth;
    m_RookOffsets[1] = -1;
    m_RookOffsets[2] = 1;
    m_RookOffsets[3] = m_arwidth;

    m_GuardOffsets.resize(4);
    m_GuardOffsets[0] = -m_arwidth - 1;
    m_GuardOffsets[1] = -m_arwidth + 1;
    m_GuardOffsets[2] = m_arwidth - 1;
    m_GuardOffsets[3] = m_arwidth + 1;

    strnumCn.resize(10);
    strnumCn[0] = "\u3007";
    strnumCn[1] = "\u4e00";
    strnumCn[2] = "\u4e8c";
    strnumCn[3] = "\u4e09";
    strnumCn[4] = "\u56db";
    strnumCn[5] = "\u4e94";
    strnumCn[6] = "\u516d";
    strnumCn[7] = "\u4e03";
    strnumCn[8] = "\u516b";
    strnumCn[9] = "\u4e5d";

    strnumEn.resize(10);
    strnumEn[0] = "\uff10";
    strnumEn[1] = "\uff11";
    strnumEn[2] = "\uff12";
    strnumEn[3] = "\uff13";
    strnumEn[4] = "\uff14";
    strnumEn[5] = "\uff15";
    strnumEn[6] = "\uff16";
    strnumEn[7] = "\uff17";
    strnumEn[8] = "\uff18";
    strnumEn[9] = "\uff19";

    strnumName.resize(16);
    strnumName[0] = "\u2003";
    strnumName[1] = "\u5175";
    strnumName[2] = "\u76f8";
    strnumName[3] = "\u4ed5";
    strnumName[4] = "\u70ae";
    strnumName[5] = "\u9a6c";
    strnumName[6] = "\u8f66";
    strnumName[7] = "\u5e05";
    strnumName[8] = "\u5352";
    strnumName[9] = "\u8c61";
    strnumName[10] = "\u58eb";
    strnumName[11] = "\u70ae";
    strnumName[12] = "\u9a6c";
    strnumName[13] = "\u8f66";
    strnumName[14] = "\u5c06";
    strnumName[15] = "\u2003";
}

int WesternBoard::captureType(const Move& move) const
{
    return Board::captureType(move);
}

Move WesternBoard::moveFromStandardString(const QString& str)
{
    QVarLengthArray<Move> moves;
    generateMoves(moves);

    for(auto const& move : moves)
    {
        QString cn = standardMoveString(move);

        if (str != cn)
            continue;
        if (vIsLegalMove(move))
            return move;
    }

    return Move();
}

QString WesternBoard::lanMoveString(const Move& move)
{
    return Board::lanMoveString(move);
}

QString WesternBoard::standardMoveString(const Move& move)
{
    int source = move.sourceSquare();
    int target = move.targetSquare();
    QString str;
    Side side(sideToMove());
    Piece piece = pieceAt(source);
    Square from = chessSquare(source);
    Square to = chessSquare(target);

    int pieceType = piece.type();
    int maxAmbiguityFile = 0;
    int minAmbiguityFile = 0;
    AmbiguityStep fileAmbiguityStep[9];

    int strnumFromIndex = 0;
    int strnumToIndex = 0;
    int pieceTypeIndex = 0;
    QVarLengthArray<QString> strnumNameIndex;

    for (int file = 0; file < 9; file++)
    {
        const AmbiguityStep &ambiguityStep = pieceAmbiguity(side, file);
        fileAmbiguityStep[file] = ambiguityStep;
        if (ambiguityStep.ambiguityType > fileAmbiguityStep[maxAmbiguityFile].ambiguityType)
            maxAmbiguityFile = file;
    }

    for (int file = 0; file < 9; file++)
    {
        const AmbiguityStep &ambiguityStep = fileAmbiguityStep[file];
        if (file == maxAmbiguityFile)
            continue;
        if (ambiguityStep.ambiguityType > fileAmbiguityStep[minAmbiguityFile].ambiguityType)
            minAmbiguityFile = file;
    }

    if (side == Side::Red)
    {
        strnumFromIndex = 9 - from.file();
        strnumToIndex = 9 - to.file();
        pieceTypeIndex = pieceType;
        strnumNameIndex = strnumCn;
    }
    else
    {
        strnumFromIndex = from.file() + 1;
        strnumToIndex = to.file() + 1;
        pieceTypeIndex = pieceType + 7;
        strnumNameIndex = strnumEn;
    }

    if (fileAmbiguityStep[from.file()].ambiguityType == AmbiguityType::None
            || fileAmbiguityStep[from.file()].pieceType != pieceType)
    {
        str += strnumName[pieceTypeIndex];
        str += strnumNameIndex[strnumFromIndex];
    }
    else if (fileAmbiguityStep[from.file()].ambiguityType == AmbiguityType::SuperHou)
    {
        int index = 0, index2 = 0;
        for (int rank = 0; rank < 10; rank++)
        {
            const Square& sq = Square(from.file(), rank);
            const Piece& piece = pieceAt(sq);
            if (piece.side() != side || piece.type() != pieceType)
                continue;
            index++;
            if (sq.rank() < from.rank())
                index2++;
        }

        if (side == Side::Red)
            index2 = index - index2;
        else
            index2 = index2 + 1;

        str += strnumNameIndex[index2];
        str += strnumName[pieceTypeIndex];
    }
    else
    {
        bool front = false, behind = false;

        for (int rank = 0; rank < 10; rank++)
        {
            const Square& sq = Square(from.file(), rank);
            const Piece& other = pieceAt(sq);
            if (other.side() != side || other.type() != pieceType)
                continue;
            if (rank < from.rank())
                front = true;
            else if (rank > from.rank())
                behind = true;
        }

        if (front != behind)
        {
            if (side == Side::Red)
                str += front ? "\u524d" : "\u540e";
            else
                str += front ? "\u540e" : "\u524d";
        }
        else
        {
            str += "\u4e2d";
        }

        if (fileAmbiguityStep[minAmbiguityFile].ambiguityType == AmbiguityType::None)
            str += strnumName[pieceTypeIndex];
        else
            str += strnumNameIndex[strnumFromIndex];
    }

    if (to.rank() == from.rank())
    {
        str += "\u5e73";
        str += strnumNameIndex[strnumToIndex];
    }
    else
    {
        if (side == Side::Red)
            str += target < source ? "\u8fdb" : "\u9000";
        else
            str += target < source ? "\u9000" : "\u8fdb";

        if (pieceType == Elephant || pieceType == Guard || pieceType == Horse)
            str += strnumNameIndex[strnumToIndex];
        else
            str += strnumNameIndex[abs(from.rank() - to.rank())];
    }

    return str;
}

Move WesternBoard::moveFromLanString(const QString& str)
{
    return Board::moveFromEnglishString(str);
}

AmbiguityStep WesternBoard::pieceAmbiguity(Side side, int file) const
{
    int pieceCount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    AmbiguityStep maxAmbiguityStep = { AmbiguityType::None, 0 };

    for (int rank = 0; rank < 10; rank++)
    {
        const Square& sq = Square(file, rank);
        const Piece& piece = pieceAt(sq);
        if (piece.side() != side)
            continue;
        pieceCount[piece.type()]++;
    }

    for (int pt = Pawn; pt <= King; pt++)
    {
        int countToIndex = pieceCount[pt] - 1;
        const AmbiguityType &ambiguityType = static_cast<AmbiguityType>(countToIndex);
        if (ambiguityType > maxAmbiguityStep.ambiguityType)
        {
            maxAmbiguityStep.pieceType = pt;
            maxAmbiguityStep.ambiguityType = ambiguityType;
        }
    }

    return maxAmbiguityStep;
}

QString WesternBoard::vFenIncludeString(FenNotation notation) const
{
    Q_UNUSED(notation);
    return QString();
}

QString WesternBoard::vFenString(FenNotation notation) const
{
    QString fen = vFenIncludeString(notation);

    // Reversible halfmove count
    fen += "- - ";
    fen += QString::number(m_reversibleMoveCount);

    // Full move number
    fen += ' ';
    fen += QString::number((m_history.size() + m_plyOffset) / 2 + 1);

    return fen;
}

bool WesternBoard::vSetFenString(const QStringList& fen)
{
    if (fen.size() < 2)
        return false;
    QStringList::const_iterator token = fen.begin();

    // Find the king squares
    int kingCount[2] = {0, 0};
    for (int sq = 0; sq < arraySize(); sq++)
    {
        Piece tmp = pieceAt(sq);
        if (tmp.type() == King)
        {
            m_kingSquare[tmp.side()] = sq;
            kingCount[tmp.side()]++;
        }
    }
    if (!kingsCountAssertion(kingCount[Side::Red],
                             kingCount[Side::Black]))
        return false;

    // Short non-standard format without castling and ep fields?
    bool isShortFormat = false;
    if (fen.size() < 3)
        token->toInt(&isShortFormat);

    if (!isShortFormat)
        ++token;

    // En-passant square
    Side side(sideToMove());
    m_sign = (side == Side::Red) ? 1 : -1;

    if (!isShortFormat)
        ++token;

    // Reversible halfmove count
    if (token != fen.end())
    {
        bool ok;
        int tmp = token->toInt(&ok);
        if (!ok || tmp < 0)
            return false;
        m_reversibleMoveCount = tmp;
        ++token;
    }
    else
        m_reversibleMoveCount = 0;

    // Read the full move number and calculate m_plyOffset
    if (token != fen.end())
    {
        bool ok;
        int tmp = token->toInt(&ok);
        if (!ok || tmp < 1)
            return false;
        m_plyOffset = 2 * (tmp - 1);
    }
    else
        m_plyOffset = 0;

    if (m_sign != 1)
        m_plyOffset++;

    m_history.clear();
    return true;
}

void WesternBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
    Side side = sideToMove();
    int source = move.sourceSquare();
    int target = move.targetSquare();
    Piece capture = pieceAt(target);
    int pieceType = pieceAt(source).type();

    bool clearSource = true;
    bool isReversible = true;

    Q_ASSERT(target != 0);

    MoveData md = { capture, m_reversibleMoveCount };

    if (source == target)
        clearSource = 0;

    if (pieceType == King)
        m_kingSquare[side] = target;

    if (captureType(move) != Piece::NoPiece)
        isReversible = false;

    if (transition != nullptr)
        if (source != 0)
            transition->addMove(chessSquare(source),
                                chessSquare(target));

    setSquare(target, Piece(side, pieceType));

    if (clearSource)
        setSquare(source, Piece::NoPiece);

    if (isReversible)
        m_reversibleMoveCount++;
    else
        m_reversibleMoveCount = 0;

    m_history.append(md);
    m_sign *= -1;
}

void WesternBoard::vUndoMove(const Move& move)
{
    const MoveData& md = m_history.last();
    int source = move.sourceSquare();
    int target = move.targetSquare();

    m_sign *= -1;
    Side side = sideToMove();

    m_reversibleMoveCount = md.reversibleMoveCount;

    if (target == m_kingSquare[side])
        m_kingSquare[side] = source;

    setSquare(source, pieceAt(target));
    setSquare(target, md.capture);
    m_history.pop_back();
}

void WesternBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
                                         int pieceType,
                                         int sourceSquare) const
{
    switch (pieceType)
    {
    case Pawn:
    {
        Piece piece = pieceAt(sourceSquare);
        if (piece.side() == Side::Red) {
            Side opSide = sideToMove().opposite();
            for (int i = 0; i < m_RPawnOffsets.size(); i++)
            {
                int targetSquare = sourceSquare + m_RPawnOffsets[i];
                if (!isValidSquare(chessSquare(targetSquare)))
                    continue;
                if (sourceSquare > 75)
                    if (i != 0)  continue;

                Piece capture = pieceAt(targetSquare);
                if (capture.isEmpty() || capture.side() == opSide)
                    moves.append(Move(sourceSquare, targetSquare));
            }
        }
        else
        {
            Side opSide = sideToMove().opposite();
            for (int i = 0; i < m_BPawnOffsets.size(); i++)
            {
                int targetSquare = sourceSquare + m_BPawnOffsets[i];
                if (!isValidSquare(chessSquare(targetSquare)))
                    continue;
                if (sourceSquare < 78)
                    if (i != 0)  continue;

                Piece capture = pieceAt(targetSquare);
                if (capture.isEmpty() || capture.side() == opSide)
                    moves.append(Move(sourceSquare, targetSquare));
            }
        }
    }
        break;
    case King:
    {
        Side opSide = sideToMove().opposite();
        for (int i = 0; i < m_RookOffsets.size(); i++)
        {
            int targetSquare = sourceSquare + m_RookOffsets[i];
            if (!isValidSquare(chessSquare(targetSquare)))
                continue;

            if (!isInFort(chessSquare(targetSquare)))
                continue;

            Piece capture = pieceAt(targetSquare);
            if (capture.isEmpty() || capture.side() == opSide)
                moves.append(Move(sourceSquare, targetSquare));
        }
    }
        break;
    case Guard:
    {
        Side opSide = sideToMove().opposite();
        for (int i = 0; i < m_GuardOffsets.size(); i++)
        {
            int targetSquare = sourceSquare + m_GuardOffsets[i];
            if (!isValidSquare(chessSquare(targetSquare)))
                continue;

            if (!isInFort(chessSquare(targetSquare)))
                continue;

            Piece capture = pieceAt(targetSquare);
            if (capture.isEmpty() || capture.side() == opSide)
                moves.append(Move(sourceSquare, targetSquare));
        }
    }
        break;
    case Rook:
    {
        Side side = sideToMove();
        for (int i = 0; i < m_RookOffsets.size(); i++)
        {
            int offset = m_RookOffsets[i];
            int targetSquare = sourceSquare + offset;
            Piece capture;
            while (!(capture = pieceAt(targetSquare)).isWall()
                   && capture.side() != side)
            {
                moves.append(Move(sourceSquare, targetSquare));
                if (!capture.isEmpty())
                    break;
                targetSquare += offset;
            }
        }
    }
        break;
    case Cannon:
    {
        Side side = sideToMove();
        for (int i = 0; i < m_RookOffsets.size(); i++)
        {
            int offset = m_RookOffsets[i];
            int targetSquare = sourceSquare + offset;
            Piece capture;
            while (!(capture = pieceAt(targetSquare)).isWall() && capture.isEmpty())
            {
                moves.append(Move(sourceSquare, targetSquare));
                if (!capture.isEmpty())
                    break;
                targetSquare += offset;
            }
            if (!capture.isEmpty())
            {
                while (true)
                {
                    targetSquare += offset;
                    capture = pieceAt(targetSquare);
                    if (capture.isEmpty())
                        continue;
                    if (capture.isWall())
                        break;
                    if (capture.side() != side)
                    {
                        moves.append(Move(sourceSquare, targetSquare));
                        break;
                    }
                }
            }
        }
        break;
    }
    case Horse:
    {
        Side opSide = sideToMove().opposite();
        for (int i = 0; i < m_HorseOffsets.size(); i++)
        {
            int targetSquare = sourceSquare + m_HorseOffsets[i];
            if (!isValidSquare(chessSquare(targetSquare)))
                continue;
            int leg = sourceSquare + m_HorseLegOffsets[i];
            if (!pieceAt(leg).isEmpty())
                continue;
            Piece capture = pieceAt(targetSquare);
            if (capture.isEmpty() || capture.side() == opSide)
                moves.append(Move(sourceSquare, targetSquare));
        }
    }
        break;
    case Elephant:
    {
        Side opSide = sideToMove().opposite();
        for (int i = 0; i < m_ElephantOffsets.size(); i++)
        {
            int targetSquare = sourceSquare + m_ElephantOffsets[i];
            if (!isValidSquare(chessSquare(targetSquare)))
                continue;
            int leg = sourceSquare + m_ElephantEyeOffsets[i];
            if (!pieceAt(leg).isEmpty())
                continue;

            Piece capture = pieceAt(targetSquare);
            if (capture.isEmpty() || capture.side() == opSide)
            {
                if (sourceSquare > 78 && targetSquare < 78)
                    continue;
                if (sourceSquare < 78 && targetSquare > 78)
                    continue;
                moves.append(Move(sourceSquare, targetSquare));
            }
        }
    }
        break;
    }
}

bool WesternBoard::inCheck(Side side) const
{
    Side opSide = side.opposite();

    for (int i = 0; i < m_RookOffsets.size(); i++)
    {
        int count = 0;

        for (int targetSquare = m_kingSquare[side] + m_RookOffsets[i];
             isValidSquare(chessSquare(targetSquare));
             targetSquare += m_RookOffsets[i])
        {
            Piece piece = pieceAt(targetSquare);
            if (piece.isEmpty())
                continue;
            count++;
            if (piece.side() != opSide)
                continue;
            if (count == 1)
            {
                if (piece.type() == Rook || piece.type() == King)
                    return true;
                else if (piece.type() == Pawn)
                {
                    if (abs(targetSquare - m_kingSquare[side]) == 1)
                        return true;
                    int offset = side == Side::Red ?
                                m_kingSquare[side] - 11 : m_kingSquare[side] + 11;
                    if (targetSquare == offset)
                        return true;
                }
            }
            else if (count == 2)
            {
                if (piece.type() == Cannon)
                    return true;
            }
        }
    }

    // Knight, archbishop, chancellor attacks
    for (int i = 0; i < m_HorseOffsets.size(); i++)
    {
        Piece piece = pieceAt(m_kingSquare[side] + m_HorseOffsets[i]);
        if (piece.side() == opSide && piece.type() == Horse)
        {
            Piece leg = pieceAt(m_kingSquare[side] + m_HorseCheckLegOffsets[i]);

            if (leg.isEmpty())
                return true;
        }
    }

    return false;
}

bool WesternBoard::isLegalPosition()
{
    Side side = sideToMove().opposite();
    return inCheck(side) ? false : true;
}

bool WesternBoard::vIsLegalMove(const Move& move)
{
    Q_ASSERT(!move.isNull());
    return Board::vIsLegalMove(move);
}

int WesternBoard::kingSquare(Side side) const
{
    Q_ASSERT(!side.isNull());
    return m_kingSquare[side];
}

int WesternBoard::reversibleMoveCount() const
{
    return m_reversibleMoveCount;
}

Result WesternBoard::result()
{
    QString str;

    // Checkmate/Stalemate
    if (!canMove())
    {
        Side winner = sideToMove().opposite();
        str = tr("%1 Win!").arg(winner.toString());
        return Result(Result::Win, winner, str);
    }

    // Insufficient mating material
    int material = 0;

    for (int i = 0; i < arraySize(); i++)
    {
        const Piece& piece = pieceAt(i);
        if (!piece.isValid())
            continue;

        switch (piece.type())
        {
        case King:
            break;
        case Guard:
            break;
        case Elephant:
            material++;
            break;
        default:
            material += 2;
            break;
        }
    }

    if (material <= 0)
    {
        str = tr("Both sides have no attacks, judged draw!");
        return Result(Result::Draw, Side::NoSide, str);
    }

    if (m_reversibleMoveCount >= 120)
    {
        str = tr("60 step did not eat, judged draw!");
        return Result(Result::Draw, Side::NoSide, str);
    }

    if (repeatCount() >= 2)
    {
        str = tr("Cycle 3 steps, judged draw!");
        return Result(Result::Draw, Side::NoSide, str);
    }

    return Result();
}

} // namespace Chess
