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

#include "side.h"

namespace Chess {

Side::Side(const QString& symbol)
{
    if (symbol == "w")
        m_type = Red;
    else if (symbol == "b")
        m_type = Black;
    else
        m_type = NoSide;
}

QString Side::symbol() const
{
    if (m_type == Red)
        return "w";
    else if (m_type == Black)
        return "b";

    return QString();
}

QString Side::toString() const
{
    if (m_type == Red)
        return tr("Red");
    else if (m_type == Black)
        return tr("Black");

    return QString();
}

} // namespace Chess
