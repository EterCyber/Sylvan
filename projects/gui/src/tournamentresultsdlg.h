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

#ifndef TOURNAMENTRESULTSDIALOG_H
#define TOURNAMENTRESULTSDIALOG_H

#include <QDialog>

class QPlainTextEdit;
class Tournament;

class TournamentResultsDialog : public QDialog
{
    Q_OBJECT

public:
    /* Creates a new TournamentResultsDialog. */
    TournamentResultsDialog(QWidget* parent = nullptr);
    virtual ~TournamentResultsDialog();

    /*! Sets \a tournament as the current tournament. */
    void setTournament(Tournament* tournament);

public slots:
    void update();

private:
    QPlainTextEdit* m_resultsEdit;
};

#endif // TOURNAMENTRESULTSDIALOG_H
