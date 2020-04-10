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

#include <QMutex>
#include <QMutexLocker>
#include <QVarLengthArray>

#include "piece.h"
#include "zobrist.h"

namespace {

QVarLengthArray<quint64, 0x10> s_keys;
QMutex s_mutex;

} // anonymous namespace

namespace Chess {

int Zobrist::s_randomSeed = 1;

Zobrist::Zobrist(const quint64* keys)
    : m_initialized(false),
      m_squareCount(0),
      m_pieceTypeCount(0),
      m_keys(keys)
{
}

bool Zobrist::isInitialized() const
{
    return m_initialized;
}

void Zobrist::initialize(int squareCount,
                         int pieceTypeCount)
{
    Q_ASSERT(squareCount > 0);
    Q_ASSERT(pieceTypeCount > 1);

    QMutexLocker locker(&s_mutex);

    if (m_initialized)
        return;

    m_squareCount = squareCount;
    m_pieceTypeCount = pieceTypeCount;
    m_initialized = true;
}

quint64 Zobrist::side() const
{
    return m_keys[0];
}

quint64 Zobrist::piece(const Piece& piece, int square) const
{
    Q_ASSERT(piece.isValid());
    Q_ASSERT(piece.type() >= 0 && piece.type() < m_pieceTypeCount);
    Q_ASSERT(square >= 0 && square < m_squareCount);

    int i = 1 + m_squareCount * m_pieceTypeCount * piece.side() +
            piece.type() * m_squareCount + square;
    return m_keys[i];
}

quint64 Zobrist::reservePiece(const Piece& piece, int slot) const
{
    Q_ASSERT(slot >= 0);

    // HACK: Use the "wall" squares (0...n) as slots
    // for hand pieces.
    return this->piece(piece, slot);
}

} // namespace Chess
