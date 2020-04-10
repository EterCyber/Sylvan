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

#include "boardtransition.h"

namespace Chess {

BoardTransition::BoardTransition()
{
}

bool BoardTransition::isEmpty() const
{
    return (m_moves.isEmpty() &&
            m_squares.isEmpty());
}

void BoardTransition::clear()
{
    m_moves.clear();
    m_squares.clear();
}

QList<BoardTransition::Move> BoardTransition::moves() const
{
    return m_moves;
}

QList<Square> BoardTransition::squares() const
{
    return m_squares;
}

void BoardTransition::addMove(const Square& source, const Square& target)
{
    Move move = { source, target };
    m_moves.append(move);

    addSquare(source);
    addSquare(target);
}

void BoardTransition::addSquare(const Square& square)
{
    if (!m_squares.contains(square))
        m_squares.append(square);
}

} // namespace Chess
