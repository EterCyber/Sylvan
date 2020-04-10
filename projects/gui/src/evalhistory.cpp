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

#include <QVBoxLayout>
#include <QtGlobal>
#include <qcustomplot.h>

#include <chessgame.h>
#include <moveevaluation.h>

#include "evalhistory.h"

EvalHistory::EvalHistory(QWidget *parent)
    : QWidget(parent),
      m_plot(new QCustomPlot(this)),
      m_game(nullptr)
{
    auto x = m_plot->xAxis;
    auto y = m_plot->yAxis;
    auto ticker = new QCPAxisTickerFixed;

    x->setLabel(tr("Step"));
    x->setRange(1, 20); // 5->10
    x->setTicks(false);
    x->setSubTicks(false);
    x->setTicker(QSharedPointer<QCPAxisTicker>(ticker));
    x->setLabelColor(QApplication::palette().text().color());
    x->setTickLabelColor(QApplication::palette().text().color());
    x->setTickPen(QApplication::palette().text().color());
    x->setBasePen(QApplication::palette().text().color());

    y->setLabel(tr("Score"));
    y->setRange(-5, 5); // (-1, 1)->(-5, 5)
    x->setTicks(false);
    y->setSubTicks(false);
    y->setLabelColor(QApplication::palette().text().color());
    y->setTickLabelColor(QApplication::palette().text().color());
    y->setTickPen(QApplication::palette().text().color());
    y->setBasePen(QApplication::palette().text().color());

    m_plot->setBackground(QApplication::palette().window());

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_plot);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    setMinimumHeight(120);
}

void EvalHistory::setGame(ChessGame* game)
{
    if (m_game)
        m_game->disconnect(this);
    m_game = game;
    m_plot->clearGraphs();
    if (!game)
    {
        replot(0);
        return;
    }

    connect(m_game, SIGNAL(scoreChanged(int,int)),
            this, SLOT(onScore(int,int)));

    setScores(game->scores());
}

void EvalHistory::setPgnGame(PgnGame* pgn)
{
    if (pgn == nullptr || pgn->isNull())
        return;

    setScores(pgn->extractScores());
}

void EvalHistory::setScores(const QMap< int, int >& scores)
{
    m_plot->addGraph();
    m_plot->addGraph();

    auto cRed = QColor(0xff, 0x30, 0x30);
    auto cBlack = QColor(0x30, 0x30, 0xff);

    auto pRed = QPen(cRed.darker(100));
    pRed.setWidth(2);
    auto pBlack = QPen(cBlack.darker(100));
    pBlack.setWidth(2);

    m_plot->graph(0)->setPen(pRed);
    cRed.setAlpha(100);
    m_plot->graph(0)->setBrush(QBrush(cRed));
    m_plot->graph(1)->setPen(pBlack);
    cBlack.setAlpha(100);
    m_plot->graph(1)->setBrush(QBrush(cBlack));

    int ply = -1;

    for (auto it = scores.constBegin(); it != scores.constEnd(); ++it)
    {
        ply = it.key();
        addData(ply, it.value());
    }
    replot(ply);
}

void EvalHistory::addData(int ply, int score)
{
    if (score == MoveEvaluation::NULL_SCORE)
        return;

    int side = (ply % 2 == 0) ? 0 : 1;
    double x = double(ply + 2) / 2;
    double y = qBound(-15.0, double(score) / 100.0, 15.0);

    if (side == 1)
        y = -y;
    if (y > 5.0)
        y = 5.0;
    else if (y < -5.0)
        y = -5.0;

    if (ply < 2)
        m_plot->graph(1 - side)->addData(1, -y);

    m_plot->graph(side)->addData(x, y);
}

void EvalHistory::replot(int maxPly)
{
    if (maxPly == -1)
    {
        auto ticker = new QCPAxisTickerFixed;
        m_plot->xAxis->setRange(1, 20); // (1, 5)->(1, 10)
        m_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(ticker));
        m_plot->yAxis->setRange(-5, 5); // (-1, 1)->(-5, 5)
    }
    else
    {
        maxPly /= 2;
        maxPly += static_cast<int>(maxPly * 0.2);
        m_plot->xAxis->setRange(1, qMax(20, maxPly));
    }
    m_plot->replot();
}

void EvalHistory::onScore(int ply, int score)
{
    addData(ply, score);
    replot(ply);
}
