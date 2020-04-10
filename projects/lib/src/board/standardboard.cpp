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

#include "standardboard.h"
#include "westernzobrist.h"

namespace {

// Zobrist keys for Polyglot opening book compatibility
// Specs: http://alpha.uhasselt.be/Research/Algebra/Toga/book_format.html

} // anonymous namespace

namespace Chess {

StandardBoard::StandardBoard()
    : WesternBoard(new WesternZobrist(nullptr))
{
}

Board* StandardBoard::copy() const
{
    return new StandardBoard(*this);
}

QString StandardBoard::variant() const
{
    return "standard";
}

QString StandardBoard::defaultFenString() const
{
    return "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
}

Result StandardBoard::tablebaseResult(unsigned int* dtz) const
{
    Q_UNUSED(dtz);
    return Result();
}

} // namespace Chess
