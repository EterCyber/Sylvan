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

#include "result.h"

namespace Chess {

Result::Result(Type type, Side winner, const QString& description)
    : m_type(type),
      m_winner(winner),
      m_description(description)
{
}

Result::Result(const QString& str)
    : m_type(ResultError)
{
    if (str.startsWith("1-0"))
    {
        m_type = Win;
        m_winner = Side::Red;
    }
    else if (str.startsWith("0-1"))
    {
        m_type = Win;
        m_winner = Side::Black;
    }
    else if (str.startsWith("1/2-1/2"))
        m_type = Draw;
    else if (str.startsWith("*"))
        m_type = NoResult;

    int start = str.indexOf('{');
    int end = str.lastIndexOf('}');
    if (start != -1 && end != -1)
        m_description = str.mid(start + 1, end - start - 1);
}

bool Result::operator==(const Result& other) const
{
    return (m_type == other.m_type &&
            m_winner == other.m_winner &&
            m_description == other.m_description);
}

bool Result::operator!=(const Result& other) const
{
    return (m_type != other.m_type ||
            m_winner != other.m_winner ||
            m_description != other.m_description);
}

bool Result::isNone() const
{
    return m_type == NoResult;
}

bool Result::isDraw() const
{
    return (m_winner.isNull() &&
            m_type != NoResult &&
            m_type != ResultError);
}

Side Result::winner() const
{
    return m_winner;
}

Side Result::loser() const
{
    if (m_winner.isNull())
        return Side::NoSide;
    return m_winner.opposite();
}

Result::Type Result::type() const
{
    return m_type;
}

QString Result::description() const
{
    QString w(winner().toString());
    QString l(loser().toString());
    QString str;

    if (m_type == Resignation)
        str = tr("%1's Red side admit defeat").arg(l);
    else if (m_type == Timeout)
    {
        if (l.isEmpty())
            str = tr("Timeout draw");
        else
            str = tr("%1's Timeout defeat").arg(l);
    }
    else if (m_type == Adjudication)
    {
        if (w.isEmpty())
            str = tr("Manual draw");
        else
            str = tr("%1's Manual win").arg(w);
    }
    else if (m_type == IllegalMove)
        str = tr("%1's Output illegal moves").arg(l);
    else if (m_type == Disconnection)
    {
        if (l.isEmpty())
            str = tr("Disconnect draw");
        else
            str = tr("%1's Disconnect").arg(l);
    }
    else if (m_type == StalledConnection)
    {
        if (l.isEmpty())
            str = tr("Stalled connection draw");
        else
            str = tr("%1's Disconnect").arg(l);
    }
    else if (m_type == Agreement)
    {
        if (w.isEmpty())
            str = tr("Rules draw");
        else
            str = tr("%1's Rules win").arg(w);
    }
    else if (m_type == NoResult)
        str = tr("No result");
    else if (m_type == ResultError)
        str = tr("Result error");

    if (m_description.isEmpty())
    {
        if (m_type == Win)
            str = tr("%1's Win").arg(w);
        else if (m_type == Draw)
            str = tr("Draw");
    }
    else
    {
        if (!str.isEmpty())
            str += tr(": ");
        str += m_description;
    }

    Q_ASSERT(!str.isEmpty());
    str[0] = str.at(0).toUpper();
    return str;
}

QString Result::toShortString() const
{
    if (m_type == NoResult || m_type == ResultError)
        return "*";
    if (m_winner == Side::Red)
        return "1-0";
    if (m_winner == Side::Black)
        return "0-1";
    return "1/2-1/2";
}

QString Result::toVerboseString() const
{
    return toShortString() + " {" + description() + "}";
}

} // namespace Chess
