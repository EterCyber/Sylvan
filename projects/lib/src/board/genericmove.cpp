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

#include "genericmove.h"

namespace Chess {

GenericMove::GenericMove()
{
}

GenericMove::GenericMove(const Square& sourceSquare,
                         const Square& targetSquare)
    : m_sourceSquare(sourceSquare),
      m_targetSquare(targetSquare)
{
}

bool GenericMove::operator==(const GenericMove& other) const
{
    if (m_sourceSquare == other.m_sourceSquare
            &&  m_targetSquare == other.m_targetSquare)
        return true;
    return false;
}

bool GenericMove::operator!=(const GenericMove& other) const
{
    if (m_sourceSquare != other.m_sourceSquare
            ||  m_targetSquare != other.m_targetSquare)
        return true;
    return false;
}

bool GenericMove::isNull() const
{
    bool validSource = (m_sourceSquare.isValid());
    return !(validSource && m_targetSquare.isValid());
}

Square GenericMove::sourceSquare() const
{
    return m_sourceSquare;
}

Square GenericMove::targetSquare() const
{
    return m_targetSquare;
}

void GenericMove::setSourceSquare(const Square& square)
{
    m_sourceSquare = square;
}

void GenericMove::setTargetSquare(const Square& square)
{
    m_targetSquare = square;
}

} // namespace Chess
