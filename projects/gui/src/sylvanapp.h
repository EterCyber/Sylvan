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

#ifndef SYLVAN_APPLICATION_H
#define SYLVAN_APPLICATION_H

#include <QApplication>
#include <QPointer>

class EngineManager;
class GameManager;
class MainWindow;
class SettingsDialog;
class TournamentResultsDialog;
class GameDatabaseManager;
class GameDatabaseDialog;
class PgnImporter;
class ChessGame;
class GameWall;

class SylvanApplication : public QApplication
{
    Q_OBJECT

public:
    SylvanApplication(int& argc, char* argv[]);
    virtual ~SylvanApplication();

    QString configPath();
    EngineManager* engineManager();
    GameManager* gameManager();
    GameDatabaseManager* gameDatabaseManager();
    QList<MainWindow*> gameWindows();
    void showGameWindow(int index);
    TournamentResultsDialog* tournamentResultsDialog();

    static SylvanApplication* instance();
    static QString userName();

public slots:
    MainWindow* newGameWindow(ChessGame* game);
    void newDefaultGame();
    void showSettingsDialog();
    void showTournamentResultsDialog();
    void showGameDatabaseDialog();
    void showGameWall();
    void closeDialogs();
    void onQuitAction();

private:
    void showDialog(QWidget* dlg);

    SettingsDialog* m_settingsDialog;
    TournamentResultsDialog* m_tournamentResultsDialog;
    EngineManager* m_engineManager;
    GameManager* m_gameManager;
    GameDatabaseManager* m_gameDatabaseManager;
    QList<QPointer<MainWindow> > m_gameWindows;
    GameDatabaseDialog* m_gameDatabaseDialog;
    QPointer<GameWall> m_gameWall;
    bool m_initialWindowCreated;

private slots:
    void onLastWindowClosed();
    void onAboutToQuit();
};

#endif // SYLVAN_APPLICATION_H
