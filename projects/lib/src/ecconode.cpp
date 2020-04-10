/*
    This file is part of Sylvan.
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
#include <QFile>
#include <QDataStream>
#include <QMutex>

#include <ecconode.h>
#include <pgnstream.h>

#include "pgngame.h"

namespace {

QStringList s_openings;
EccoNode* s_root = nullptr;

class EccoDeleter
{
public:
    ~EccoDeleter()
    {
        delete s_root;
    }
};
EccoDeleter s_eccoDeleter;

int eccoFromString(const QString& eccoString)
{
    if (eccoString.length() < 2)
        return -1;
    int hundreds = eccoString.at(0).toUpper().toLatin1() - 'A';

    bool ok = false;
    int tens = eccoString.right(eccoString.length() - 1).toInt(&ok);
    if (!ok)
        return -1;

    return hundreds * 100 + tens;
}

} // anonymous namespace

QDataStream& operator<<(QDataStream& out, const EccoNode* node)
{
    if (!node)
        return out;
    out << node->m_eccoCode
        << node->m_opening
        << node->m_variation
        << node->m_children;

    return out;
}

QDataStream& operator>>(QDataStream& in, EccoNode*& node)
{
    node = new EccoNode;

    in >> node->m_eccoCode
            >> node->m_opening
            >> node->m_variation
            >> node->m_children;

    return in;
}

void EccoNode::initialize()
{
    static QMutex mutex;
    if (s_root)
        return;

    mutex.lock();
    if (!s_root)
    {
        Q_INIT_RESOURCE(ecco);

        QFile file(":/ecco.bin");
        if (!file.open(QIODevice::ReadOnly))
            qWarning("Could not open ECCO file");
        else
        {
            QDataStream in(&file);
            in.setVersion(QDataStream::Qt_4_6);
            in >> s_openings >> s_root;
        }
    }
    mutex.unlock();
}

void EccoNode::initialize(PgnStream& in)
{
    if (s_root)
        return;

    if (!in.isOpen())
    {
        qWarning("The pgn stream is not open");
        return;
    }

    s_root = new EccoNode;
    EccoNode* current = s_root;
    QMap<QString, int> tmpOpenings;

    PgnGame game;
    while (game.read(in, INT_MAX - 1, false))
    {
        current = s_root;
        for (const PgnGame::MoveData& move : game.moves())
        {
            QString san = move.moveString;
            EccoNode* node = current->child(san);
            if (node == nullptr)
            {
                node = new EccoNode;
                current->addChild(san, node);
            }
            current = node;
        }
        if (current == s_root)
            continue;

        current->m_eccoCode = eccoFromString(game.tagValue("ECCO"));

        QString val = game.tagValue("Opening");
        if (!val.isEmpty())
        {
            int index = tmpOpenings.value(val, -1);
            if (index == -1)
            {
                index = tmpOpenings.count();
                tmpOpenings[val] = index;
                s_openings.append(val);
            }
            current->m_opening = index;
        }

        current->m_variation = game.tagValue("Variation");
    }
}

const EccoNode* EccoNode::root()
{
    if (!s_root)
        initialize();
    return s_root;
}

const EccoNode* EccoNode::find(const QVector<PgnGame::MoveData>& moves)
{
    if (!s_root)
        return nullptr;

    EccoNode* current = s_root;
    EccoNode* valid = nullptr;

    for (const PgnGame::MoveData& move : moves)
    {
        EccoNode* node = current->child(move.moveString);
        if (node == nullptr)
            return valid;
        if (!node->opening().isEmpty())
            valid = node;
        current = node;
    }

    return nullptr;
}

void EccoNode::write(const QString& fileName)
{
    if (!s_root)
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("Could not open file %s", qUtf8Printable(fileName));
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_6);
    out << s_openings << s_root;
}

EccoNode::EccoNode()
    : m_eccoCode(-1),
      m_opening(-1)
{
}

EccoNode::~EccoNode()
{
    qDeleteAll(m_children);
}

bool EccoNode::isLeaf() const
{
    return m_eccoCode != -1;
}

QString EccoNode::eccoCode() const
{
    if (m_eccoCode == -1)
        return QString();

    QChar segment('A' + m_eccoCode / 100);
    return segment + QString("%1").arg(m_eccoCode % 100, 2, 10, QChar('0'));
}

QString EccoNode::opening() const
{
    return m_opening >= 0 ? s_openings[m_opening] : QString();
}

QString EccoNode::variation() const
{
    return m_variation;
}

EccoNode* EccoNode::child(const QString& sanMove) const
{
    return m_children.value(sanMove);
}

void EccoNode::addChild(const QString& sanMove, EccoNode* child)
{
    m_children[sanMove] = child;
}
