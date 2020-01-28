/*
    This file is part of Sylvan.
    Copyright (C) 2008-2018 Cute Chess authors
    Copyright (C) 2019 Wilbert Lee

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

#ifndef MOVE_H
#define MOVE_H

#include <QtGlobal>
#include <QMetaType>

namespace Chess {

/*!
 * \brief A small and efficient chessmove class.
 *
 * This class is designed to be used internally by Board objects,
 * and to store minimal information about the move. A Board object
 * is needed to verify the move's legality or to convert it to a string.
 *
 * The source and target squares have an integer format specific to a
 * certain type of chess variant. The Board class has methods for
 * converting between these integers and the generic Square type.
 *
 * \sa Piece
 * \sa Board
 * \sa GenericMove
 */
class Move
{
public:
    /*! Creates an empty Move (null move). */
    Move();
    /*!
         * Creates a new Move object with at least a source
         * square and a target square.
         */
    Move(int sourceSquare,
         int targetSquare);
    /*!
         * The source square.
         *
         * A value of 0 means that this move is a piece drop,
         * a special move allowed by some variants.
         */
    int sourceSquare() const;
    /*! The target square. */
    int targetSquare() const;
    /*! Returns true if this is a null move. */
    bool isNull() const;
    /*! Returns true if \a other is equal to this move. */
    bool operator==(const Move& other) const;
    /*! Returns true if \a other is different from this move. */
    bool operator!=(const Move& other) const;

private:
    quint32 m_data;
};

inline Move::Move()
    : m_data(0)
{
}

inline Move::Move(int sourceSquare,
                  int targetSquare)
    : m_data(sourceSquare |
             (targetSquare << 8))
{
    Q_ASSERT(sourceSquare >= 0 && sourceSquare <= 0xFF);
    Q_ASSERT(targetSquare >= 0 && targetSquare <= 0xFF);
}

inline bool Move::isNull() const
{
    return (m_data == 0);
}

inline bool Move::operator==(const Move& other) const
{
    return (m_data == other.m_data);
}

inline bool Move::operator!=(const Move& other) const
{
    return (m_data != other.m_data);
}

inline int Move::sourceSquare() const
{
    return m_data & 0xff;
}

inline int Move::targetSquare() const
{
    return (m_data >> 8) & 0xff;
}

} // namespace Chess

Q_DECLARE_METATYPE(Chess::Move)

#endif // MOVE_H