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

#ifndef TOURNAMENTSETTINGSWIDGET_H
#define TOURNAMENTSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class TournamentSettingsWidget;
}

class TournamentSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TournamentSettingsWidget(QWidget *parent = nullptr);
    virtual ~TournamentSettingsWidget();

    QString tournamentType() const;
    int gamesPerEncounter() const;
    int rounds() const;
    int seedCount() const;
    int delayBetweenGames() const;
    bool openingRepetition() const;
    bool engineRecovery() const;
    bool savingOfUnfinishedGames() const;

    void enableSettingsUpdates();

private:
    void readSettings();

    Ui::TournamentSettingsWidget *ui;
};

#endif // TOURNAMENTSETTINGSWIDGET_H
