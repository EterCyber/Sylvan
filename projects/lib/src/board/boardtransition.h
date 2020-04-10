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

#ifndef BOARDTRANSITION_H
#define BOARDTRANSITION_H

#include <QList>

#include "square.h"
#include "piece.h"

namespace Chess {

/*!
 * \brief Details of a board transition caused by a move.
 *
 * This class stores information about the movement of pieces
 * and changed squares on a chessboard. Two types of information
 * are stored: movement, and changed squares and reserve pieces.
 * The latter type of information is sufficient for updating the
 * state of a graphical board, and the former can be used to
 * display or animate the actual chessmove.
 *
 * \sa Board::makeMove()
 */
class LIB_EXPORT BoardTransition
{
public:
    /*! \brief Movement on the board. */
    struct Move
    {
        Square source;	//!< Source square
        Square target;	//!< Target square
    };

    /*! Creates a new empty BoardTransition object. */
    BoardTransition();

    /*! Returns true if there are no transitions. */
    bool isEmpty() const;
    /*! Clears all data, ie. empties the transition. */
    void clear();

    /*!
         * Returns a list of "moves".
         *
         * One chessmove can involve several moving pieces, and
         * the actual chessmove may not be on the returned list.
         */
    QList<Move> moves() const;
    /*! Returns a list of changed squares. */
    QList<Square> squares() const;

    /*! Adds a new "move" from \a source to \a target. */
    void addMove(const Square& source, const Square& target);
    /*!
         * Adds a new changed square.
         *
         * If \a square already exists in the transition, this
         * function does nothing.
         */
    void addSquare(const Square& square);

private:
    QList<Move> m_moves;
    QList<Square> m_squares;
};

} // namespace Chess
#endif // BOARDTRANSITION_H
