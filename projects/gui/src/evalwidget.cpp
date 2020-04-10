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

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QVector>
#include <QTime>

#include <chessplayer.h>

#include "evalwidget.h"

EvalWidget::EvalWidget(QWidget *parent)
    : QWidget(parent),
      m_player(nullptr),
      m_statsTable(new QTableWidget(1, 5, this)),
      m_pvTable(new QTableWidget(0, 4, this)),
      m_time("0.0"),
      m_depth("-1")
{
    m_statsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto hHeader = m_statsTable->horizontalHeader();
    auto vHeader = m_statsTable->verticalHeader();
    vHeader->hide();
    int maxHeight = hHeader->sizeHint().height() + vHeader->defaultSectionSize();
    m_statsTable->setMaximumHeight(maxHeight);

    QStringList statsHeaders;
    statsHeaders << tr("Speed") << tr("Hash")
                 << tr("Ponder") << tr("Probability") << tr("EGB");
    m_statsTable->setHorizontalHeaderLabels(statsHeaders);
    hHeader->setSectionResizeMode(QHeaderView::Stretch);

    auto protoItem = new QTableWidgetItem;
    protoItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignCenter);
    m_statsTable->setItemPrototype(protoItem);
    m_statsTable->setWordWrap(false);

    m_pvTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pvTable->verticalHeader()->hide();
    m_pvTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    m_pvTable->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);

    QStringList pvHeaders;
    pvHeaders << tr("Depth") << tr("Time")
              << tr("Score") << tr("PV");
    m_pvTable->setHorizontalHeaderLabels(pvHeaders);
    m_pvTable->setColumnWidth(0, 50);
    m_pvTable->setColumnWidth(1, 60);
    m_pvTable->setColumnWidth(2, 60);
    m_pvTable->horizontalHeader()->setStretchLastSection(true);
    m_pvTable->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_statsTable);
    layout->addWidget(m_pvTable);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void EvalWidget::clear()
{
    m_statsTable->clearContents();
    m_time = "0.0";
    m_depth = "-1";
    m_pv.clear();
    m_score.clear();
    m_pvTable->clearContents();
    m_pvTable->setRowCount(0);
}

void EvalWidget::setPlayer(ChessPlayer* player)
{
    if (player != m_player || !player)
        clear();
    if (m_player)
        m_player->disconnect(this);
    m_player = player;
    if (!player)
        return;

    connect(player, SIGNAL(startedThinking(int)),
            this, SLOT(clear()));
    connect(player, SIGNAL(thinking(MoveEvaluation)),
            this, SLOT(onEval(MoveEvaluation)));
}

void EvalWidget::onEval(const MoveEvaluation& eval)
{
    auto nps = eval.nps();
    if (nps)
    {
        QString npsStr = nps < 10000 ? QString("%1").arg(nps)
                                     : QString("%1k").arg(nps / 1000);
        auto item = m_statsTable->itemPrototype()->clone();
        item->setText(npsStr);
        m_statsTable->setItem(0, NpsHeader, item);
    }
    if (eval.tbHits())
    {
        auto item = m_statsTable->itemPrototype()->clone();
        item->setText(QString::number(eval.tbHits()));
        m_statsTable->setItem(0, TbHeader, item);
    }
    if (eval.hashUsage())
    {
        double usage = double(eval.hashUsage()) / 10.0;
        auto item = m_statsTable->itemPrototype()->clone();
        item->setText(QString("%1%").arg(usage, 0, 'f', 1));
        m_statsTable->setItem(0, HashHeader, item);
    }
    auto ponderMove = eval.ponderMove();
    if (!ponderMove.isEmpty())
    {
        auto item = m_statsTable->itemPrototype()->clone();
        item->setText(ponderMove);
        m_statsTable->setItem(0, PonderMoveHeader, item);
    }
    if (eval.ponderhitRate())
    {
        double rate = double(eval.ponderhitRate() / 10.0);
        auto item = m_statsTable->itemPrototype()->clone();
        item->setText(QString("%1%").arg(rate, 0, 'f', 1));
        m_statsTable->setItem(0, PonderHitHeader, item);
    }

    QString depth;
    if (eval.depth())
    {
        depth = QString::number(eval.depth());
        if (eval.selectiveDepth())
            depth += "/" + QString::number(eval.selectiveDepth());
    }

    QString time;
    double ms = static_cast<double>(eval.time());
    if (eval.time())
        time = QString::number(ms / 1000, 'f', 1);

    QString nodeCount;
    if (eval.nodeCount())
        nodeCount = QString::number(eval.nodeCount() / 1000) + "k";

    QString score = eval.scoreText();

    QVector<QTableWidgetItem*> items;

    items << new QTableWidgetItem(eval.depth() ? depth : m_depth);
    items << new QTableWidgetItem(eval.time() ? time : m_time);
    items << new QTableWidgetItem(score.isEmpty() ? m_score : score);
    items << new QTableWidgetItem(eval.pv().isEmpty() ? m_pv : eval.pv());

    for (int i = 0; i < 3; i++)
        items[i]->setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
    items[3]->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_pvTable->insertRow(0);

    if (eval.depth())
        m_depth = depth;
    if (eval.time())
        m_time = time;
    if (!eval.scoreText().isEmpty())
        m_score = score;
    if (!eval.pv().isEmpty())
        m_pv = eval.pv();

    for (int i = 0; i < items.size(); i++)
        m_pvTable->setItem(0, i, items.at(i));
}
