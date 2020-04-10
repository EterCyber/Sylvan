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

#include <QStringList>

#include "board.h"
#include "zobrist.h"

namespace Chess {

QDebug operator<<(QDebug dbg, const Board* board)
{
    QString str = "FEN: " + board->fenString() + '\n';
    str += Board::tr("Zobrist key") + ": 0x" +
            QString::number(board->m_key, 16).toUpper() + '\n';

    int i = (board->m_width + 2) * 2;
    for (int y = 0; y < board->m_height; y++)
    {
        i++;
        for (int x = 0; x < board->m_width; x++)
        {
            Piece pc = board->m_squares[i];
            if (pc.isValid())
                str += board->pieceSymbol(pc);
            else
                str += ".";
            str += ' ';
            i++;
        }
        i++;
        str += '\n';
    }

    dbg.nospace() << str;
    return dbg.space();
}


Board::Board(Zobrist* zobrist)
    : m_initialized(false),
      m_width(0),
      m_height(0),
      m_side(Side::Red),
      m_startingSide(Side::Red),
      m_key(0),
      m_zobrist(zobrist),
      m_sharedZobrist(zobrist)
{
    Q_ASSERT(zobrist != nullptr);

    setPieceType(Piece::NoPiece, QString(), QString());
}

Board::~Board()
{
}

bool Board::variantHasWallSquares() const
{
    return false;
}

QList<Piece> Board::reservePieceTypes() const
{
    return QList<Piece>();
}

Side Board::upperCaseSide() const
{
    return Side::Red;
}

Piece Board::pieceAt(const Square& square) const
{
    if (!isValidSquare(square))
        return Piece::WallPiece;
    return pieceAt(squareIndex(square));
}

void Board::initialize()
{
    if (m_initialized)
        return;

    m_initialized = true;
    m_width = width();
    m_height = height();
    for (int i = 0; i < (m_width + 2) * (m_height + 4); i++)
        m_squares.append(Piece::WallPiece);
    vInitialize();

    m_zobrist->initialize((m_width + 2) * (m_height + 4), m_pieceData.size());
}

void Board::setPieceType(int type,
                         const QString& name,
                         const QString& symbol,
                         const QString& gsymbol)
{
    if (type >= m_pieceData.size())
        m_pieceData.resize(type + 1);

    const QString& graphicalSymbol = gsymbol.isEmpty() ? symbol : gsymbol;

    PieceData data =
    { name, symbol.toUpper(), graphicalSymbol.toUpper() };
    m_pieceData[type] = data;
}

QString Board::pieceSymbol(Piece piece) const
{
    int type = piece.type();
    if (type <= 0 || type >= m_pieceData.size())
        return QString();

    if (piece.side() == upperCaseSide())
        return m_pieceData[type].symbol;
    return m_pieceData[type].symbol.toLower();
}

Piece Board::pieceFromSymbol(const QString& pieceSymbol) const
{
    if (pieceSymbol.isEmpty())
        return Piece::NoPiece;

    int code = Piece::NoPiece;
    QString symbol = pieceSymbol.toUpper();

    for (int i = 1; i < m_pieceData.size(); i++)
    {
        if (symbol == m_pieceData[i].symbol)
        {
            code = i;
            break;
        }
    }
    if (code == Piece::NoPiece)
        return code;

    Side side(upperCaseSide());
    if (pieceSymbol == symbol)
        return Piece(side, code);
    return Piece(side.opposite(), code);
}

QString Board::pieceString(int pieceType) const
{
    if (pieceType <= 0 || pieceType >= m_pieceData.size())
        return QString();
    return m_pieceData[pieceType].name;
}

QString Board::representation(Piece piece) const
{
    int type = piece.type();
    if (type <= 0 || type >= m_pieceData.size())
        return QString();

    if (piece.side() == upperCaseSide())
        return m_pieceData[type].representation;
    return m_pieceData[type].representation.toLower();
}

int Board::reserveType(int pieceType) const
{
    return pieceType;
}

Square Board::chessSquare(int index) const
{
    int arwidth = m_width + 2;
    int file = (index % arwidth) - 1;
    int rank = (m_height - 1) - ((index / arwidth) - 2);
    return Square(file, rank);
}

int Board::squareIndex(const Square& square) const
{
    if (!isValidSquare(square))
        return 0;

    int rank = (m_height - 1) - square.rank();
    return (rank + 2) * (m_width + 2) + 1 + square.file();
}

bool Board::isValidSquare(const Chess::Square& square) const
{
    if (!square.isValid()
            ||  square.file() >= m_width || square.rank() >= m_height)
        return false;
    return true;
}

bool Board::isInFort(const Square& square) const
{
    int f = square.file();
    int r = square.rank();

    if (f < 3) return false;
    if (f > 5) return false;
    if (r > 6) return true;
    if (r < 3) return true;
    return false;
}

QString Board::squareString(int index) const
{
    return squareString(chessSquare(index));
}

QString Board::squareString(const Square& square) const
{
    if (!square.isValid())
        return QString();

    QString str;

    str += QChar('a' + square.file());
    str += QString::number(square.rank());

    return str;
}

Square Board::chessSquare(const QString& str) const
{
    if (str.length() < 2)
        return Square();

    bool ok = false;
    int file = 0;
    int rank = 0;

    file = str.at(0).toLatin1() - 'a';
    rank = str.midRef(1).toInt(&ok);

    if (!ok)
        return Square();
    return Square(file, rank);
}

int Board::squareIndex(const QString& str) const
{
    return squareIndex(chessSquare(str));
}

QString Board::lanMoveString(const Move& move)
{
    QString str;

    str += squareString(move.sourceSquare());
    str += squareString(move.targetSquare());

    return str;
}

QString Board::moveString(const Move& move, MoveNotation notation)
{
    if (notation == Standard)
        return standardMoveString(move);
    return lanMoveString(move);
}

Move Board::moveFromEnglishString(const QString& istr)
{
    QString str(istr);
    str.remove(QRegExp("[x=+#!?]"));
    int len = str.length();

    if (len < 4) return Move();

    for (int i = 2; i < len - 1; i++)
    {
        Square sourceSq(chessSquare(str.mid(0, i)));
        Square targetSq(chessSquare(str.mid(i, len - i)));
        if (!isValidSquare(sourceSq) || !isValidSquare(targetSq))
            continue;
        int source = squareIndex(sourceSq);
        int target = squareIndex(targetSq);

        return Move(source, target);
    }

    return Move();
}

Move Board::moveFromString(const QString& str)
{
    Move move;
    move = moveFromEnglishString(str);
    if (!isLegalMove(move))
        return Move();
    return move;
}

Move Board::moveFromGenericMove(const GenericMove& move) const
{
    int source = squareIndex(move.sourceSquare());
    int target = squareIndex(move.targetSquare());

    return Move(source, target);
}

GenericMove Board::genericMove(const Move& move) const
{
    int source = move.sourceSquare();
    int target = move.targetSquare();

    return GenericMove(chessSquare(source),
                       chessSquare(target));
}

QStringList Board::pieceList(Side side) const
{
    QStringList list;
    for (int file = 0; file < height(); file++)
        for (int rank = 0; rank < width(); rank++)
        {
            Square sq = Chess::Square(file, rank);
            const Piece piece = pieceAt(sq);
            if (piece.side() != side)
                continue;

            QString s = pieceSymbol(piece).toUpper();
            s.append(squareString(sq));
            list.append(s);
        }
    return list;
}

QString Board::fenString(FenNotation notation) const
{
    QString fen;

    // Squares
    int i = (m_width + 2) * 2;
    for (int y = 0; y < m_height; y++)
    {
        int nempty = 0;
        i++;
        if (y > 0)
            fen += '/';
        for (int x = 0; x < m_width; x++)
        {
            Piece pc = m_squares[i];

            if (pc.isEmpty())
                nempty++;

            // Add the number of empty successive squares
            // to the FEN string.
            if (nempty > 0
                    && (!pc.isEmpty() || x == m_width - 1))
            {
                fen += QString::number(nempty);
                nempty = 0;
            }

            if (pc.isValid())
                fen += pieceSymbol(pc);
            else if (pc.isWall())
                fen += "*";

            i++;
        }
        i++;
    }

    // Side to move
    fen += QString(" %1 ").arg(m_side.symbol());

    return fen + vFenString(notation);
}

bool Board::setFenString(const QString& fen)
{
    QStringList strList = fen.split(' ');
    if (strList.isEmpty())
        return false;

    QStringList::iterator token = strList.begin();
    if (token->length() < m_height * 2)
        return false;

    initialize();

    int square = 0;
    int nempty = 0;
    int rankEndSquare = 0; // Last square of the previous rank
    int maxsymlen = 1;
    int boardSize = m_width * m_height;
    int k = (m_width + 2) * 2 + 1;
    QString pieceStr;

    for (int i = 0; i < m_squares.size(); i++)
        m_squares[i] = Piece::WallPiece;
    m_key = 0;


    for (int i = 0; i < token->length(); i++)
    {
        QChar c = token->at(i);

        // Move to the next rank
        if (c == '/')
        {
            if (!pieceStr.isEmpty())
                return false;

            // Reject the FEN string if the rank didn't
            // have exactly 'm_width' squares.
            if (square - rankEndSquare != m_width)
                return false;
            rankEndSquare = square;
            k += 2;
            continue;
        }

        // Wall square
        if (c == '*' && variantHasWallSquares())
        {
            if (!pieceStr.isEmpty())
                return false;
            square++;
            k++;
            continue;
        }

        // Add empty squares
        if (c.isDigit())
        {
            if (!pieceStr.isEmpty())
                return false;

            if (i < (token->length() - 1) && token->at(i + 1).isDigit())
            {
                nempty = token->midRef(i, 2).toInt();
                i++;
            }
            else
                nempty = c.digitValue();

            if (nempty > m_width || square + nempty > boardSize)
                return false;
            for (int j = 0; j < nempty; j++)
            {
                square++;
                setSquare(k++, Piece::NoPiece);
            }
            continue;
        }

        if (square >= boardSize)
            return false;

        // Read ahead for multi-character symbols
        for (int l = qMin(maxsymlen, token->length() - i); l > 0; l--)
        {
            pieceStr = token->mid(i, l);

            Piece piece = pieceFromSymbol(pieceStr);
            if (piece.isValid())
            {
                setSquare(k++, piece);
                i += l - 1;
                pieceStr.clear();
                square++;
                break;
            }
        }

        // Left over: unknown symbols
        if (!pieceStr.isEmpty())
            return false;
    }

    // The board must have exactly 'boardSize' squares and each rank
    // must have exactly 'm_width' squares.
    if (square != boardSize || square - rankEndSquare != m_width)
        return false;

    // Side to move
    if (++token == strList.end())
        return false;

    m_side = Side(*token);
    m_startingSide = m_side;

    if (m_side.isNull())
        return false;

    m_moveHistory.clear();
    m_startingFen = fen.split("moves")[0];

    // Let subclasses handle the rest of the FEN string
    if (token != strList.end())
        ++token;

    strList.erase(strList.begin(), token);

    if (!vSetFenString(strList))
        return false;

    if (m_side == Side::Red)
        xorKey(m_zobrist->side());

    if (!isLegalPosition())
        return false;

    return true;
}

void Board::reset()
{
    setFenString(defaultFenString());
}

void Board::makeMove(const Move& move, BoardTransition* transition)
{
    Q_ASSERT(!m_side.isNull());
    Q_ASSERT(!move.isNull());

    MoveData md = { move, m_key };

    vMakeMove(move, transition);

    xorKey(m_zobrist->side());
    m_side = m_side.opposite();
    m_moveHistory << md;
}

void Board::undoMove()
{
    Q_ASSERT(!m_moveHistory.isEmpty());
    Q_ASSERT(!m_side.isNull());

    m_side = m_side.opposite();
    vUndoMove(m_moveHistory.last().move);

    m_key = m_moveHistory.last().key;
    m_moveHistory.pop_back();
}

void Board::generateMoves(QVarLengthArray<Move>& moves, int pieceType) const
{
    Q_ASSERT(!m_side.isNull());

    // Cut the wall squares (the ones with a value of WallPiece) off
    // from the squares to iterate over. It bumps the speed up a bit.
    int begin = (m_width + 2) * 2;
    int end = m_squares.size() - begin;

    moves.clear();
    for (int sq = begin; sq < end; sq++)
    {
        Piece tmp = m_squares[sq];
        if (tmp.side() == m_side
                &&  (pieceType == Piece::NoPiece || tmp.type() == pieceType))
            generateMovesForPiece(moves, tmp.type(), sq);
    }
}

void Board::generateHoppingMoves(int sourceSquare,
                                 const QVarLengthArray<int>& offsets,
                                 QVarLengthArray<Move>& moves) const
{
    Side opSide = sideToMove().opposite();
    for (int i = 0; i < offsets.size(); i++)
    {
        int targetSquare = sourceSquare + offsets[i];
        if (!isValidSquare(chessSquare(targetSquare)))
            continue;
        Piece capture = pieceAt(targetSquare);
        if (capture.isEmpty() || capture.side() == opSide)
            moves.append(Move(sourceSquare, targetSquare));
    }
}

void Board::generateCheMoves(int sourceSquare,
                             const QVarLengthArray<int>& offsets,
                             QVarLengthArray<Move>& moves) const
{
    Side side = sideToMove();
    for (int i = 0; i < offsets.size(); i++)
    {
        int offset = offsets[i];
        int targetSquare = sourceSquare + offset;
        Piece capture;
        while (!(capture = pieceAt(targetSquare)).isWall()
               &&      capture.side() != side)
        {
            moves.append(Move(sourceSquare, targetSquare));
            if (!capture.isEmpty())
                break;
            targetSquare += offset;
        }
    }
}

bool Board::moveExists(const Move& move) const
{
    Q_ASSERT(!move.isNull());

    int source = move.sourceSquare();
    QVarLengthArray<Move> moves;
    Piece piece = m_squares[source];

    if (piece.side() != m_side)
        return false;
    generateMovesForPiece(moves, piece.type(), source);

    for (int i = 0; i < moves.size(); i++)
        if (moves[i] == move)
            return true;

    return false;
}

int Board::captureType(const Move& move) const
{
    Q_ASSERT(!move.isNull());

    Piece piece(m_squares[move.targetSquare()]);
    if (piece.side() == m_side.opposite())
        return piece.type();
    return Piece::NoPiece;
}

bool Board::vIsBan() {

    bool isBan = false;

    int n = 0;
    int moCheck[2] = { 1, 1 };
    int moCap[2] = { 0, 0 };

    quint64 last_key = m_key;

    for (int i = plyCount() - 1; i >= 0; i--)
    {
        if (!inCheck(sideToMove()))
            moCheck[1 & n] = 0;

        undoMove();

        if (m_key == last_key)
            break;
        if ((moCheck[0] + moCheck[1] + moCap[0] + moCap[1]) == 0)
            return false;

        n++;
    }

    n--;
    if ((moCheck[1] + moCheck[0]) == 2)
        return false;
    if (moCheck[1 & n])
        return true;

    return isBan;
}

bool Board::vIsLegalMove(const Move& move)
{
    Q_ASSERT(!move.isNull());

    int repeatCount = 0;
    bool isBan = false;
    bool isLegal = false;

    makeMove(move);
    isLegal = isLegalPosition();

    if (isLegal == true)
    {
        repeatCount = this->repeatCount();

        // Repeat twice to determine if it was a foul
        if (repeatCount >= 2)
        {
            Board* newB = this->copy();
            isBan = newB->vIsBan();
            delete newB;
        }
    }

    undoMove();

    if (isBan)
        return false;

    return isLegal;
}

bool Board::isLegalMove(const Move& move)
{
    return !move.isNull() && moveExists(move) && vIsLegalMove(move);
}

int Board::repeatCount() const
{
    if (plyCount() < 4)
        return 0;

    int repeatCount = 0;
    for (int i = plyCount() - 1; i >= 0; i--)
        if (m_moveHistory.at(i).key == m_key)
            repeatCount++;

    return repeatCount;
}

int Board::reversibleMoveCount() const
{
    return -1;
}

bool Board::isRepetition(const Chess::Move& move)
{
    Q_ASSERT(!move.isNull());

    makeMove(move);
    bool isRepeat = (repeatCount() > 0);
    undoMove();

    return isRepeat;
}

bool Board::canMove()
{
    QVarLengthArray<Move> moves;

    generateMoves(moves);

    for (int i = 0; i < moves.size(); i++)
        if (vIsLegalMove(moves[i]))
            return true;

    return false;
}

QVector<Move> Board::legalMoves()
{
    QVarLengthArray<Move> moves;
    QVector<Move> legalMoves;

    generateMoves(moves);
    legalMoves.reserve(moves.size());

    for (int i = moves.size() - 1; i >= 0; i--)
        if (vIsLegalMove(moves[i]))
            legalMoves << moves[i];

    return legalMoves;
}

Result Board::tablebaseResult(unsigned int* dtm) const
{
    Q_UNUSED(dtm);

    return Result();
}

} // namespace Chess
