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

#include <QCoreApplication>
#include <QDir>
#include <QTime>
#include <QFileInfo>
#include <QSettings>
#include <QTextCodec>

#include <mersenne.h>
#include <enginebuilder.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <humanbuilder.h>

#include "sylvanapp.h"
#include "pgnimporter.h"
#include "gamewall.h"
#include "gamedatabasemanager.h"
#include "mainwindow.h"
#include "settingsdlg.h"
#include "importprogressdlg.h"
#include "gamedatabasedlg.h"
#include "tournamentresultsdlg.h"

#ifndef Q_OS_WIN32
#	include <sys/types.h>
#	include <pwd.h>
#endif

SylvanApplication::SylvanApplication(int& argc, char* argv[])
    : QApplication(argc, argv),
      m_settingsDialog(nullptr),
      m_tournamentResultsDialog(nullptr),
      m_engineManager(nullptr),
      m_gameManager(nullptr),
      m_gameDatabaseManager(nullptr),
      m_gameDatabaseDialog(nullptr),
      m_gameWall(nullptr),
      m_initialWindowCreated(false)
{
    Mersenne::initialize(static_cast<quint32>(QTime(0,0,0).msecsTo(QTime::currentTime())));

    // Set the application icon
    QIcon icon;
    icon.addFile(":/sylvan-win.png");

    setWindowIcon(icon);
    setQuitOnLastWindowClosed(false);

    QCoreApplication::setOrganizationName(QLatin1String("EterCyber"));
    QCoreApplication::setApplicationName(QLatin1String("Sylvan"));
    QCoreApplication::setApplicationVersion(SYLVAN_VERSION);

    // Use Ini format on all platforms
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // Load the engines
    engineManager()->loadEngines(configPath() + QLatin1String("/engines.json"));

    // Read the game database state
    gameDatabaseManager()->readState(configPath() + QLatin1String("/gamedb.bin"));

    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(onLastWindowClosed()));
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
}

SylvanApplication::~SylvanApplication()
{
    delete m_gameDatabaseDialog;
    delete m_settingsDialog;
    delete m_tournamentResultsDialog;
    delete m_gameWall;
}

SylvanApplication* SylvanApplication::instance()
{
    return static_cast<SylvanApplication*>(QApplication::instance());
}

QString SylvanApplication::userName()
{
    //QTextCodec* codec = QTextCodec::codecForName("UTF-8");

    //QTextCodec::setCodecForTr(codec);

    //QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

#ifdef Q_OS_WIN32

    QString name = QString::fromLocal8Bit(qgetenv("USERNAME"));
    return name;

#else
    if (QSettings().value("ui/use_full_user_name", true).toBool())
    {
        auto pwd = getpwnam(qgetenv("USER"));
        if (pwd != nullptr)
            return QString(pwd->pw_gecos).split(',')[0];
    }
    return qgetenv("USER");
#endif
}

QString SylvanApplication::configPath()
{
    // We want to have the exact same config path in "gui" and
    // "cli" applications so that they can share resources
    QSettings settings;
    QFileInfo fi(settings.fileName());
    QDir dir(fi.absolutePath());

    if (!dir.exists())
        dir.mkpath(fi.absolutePath());

    return fi.absolutePath();
}

EngineManager* SylvanApplication::engineManager()
{
    if (m_engineManager == nullptr)
        m_engineManager = new EngineManager(this);

    return m_engineManager;
}

GameManager* SylvanApplication::gameManager()
{
    if (m_gameManager == nullptr)
    {
        m_gameManager = new GameManager(this);
        int concurrency = QSettings()
                .value("tournament/concurrency", 1).toInt();
        m_gameManager->setConcurrency(concurrency);
    }

    return m_gameManager;
}

QList<MainWindow*> SylvanApplication::gameWindows()
{
    m_gameWindows.removeAll(nullptr);

    QList<MainWindow*> gameWindowList;
    const auto gameWindows = m_gameWindows;
    for (const auto& window : gameWindows)
        gameWindowList << window.data();

    return gameWindowList;
}

MainWindow* SylvanApplication::newGameWindow(ChessGame* game)
{
    MainWindow* mainWindow = new MainWindow(game);
    m_gameWindows.prepend(mainWindow);
    mainWindow->show();
    m_initialWindowCreated = true;

    return mainWindow;
}

void SylvanApplication::newDefaultGame()
{
    // Default game is a human versus human game using standard variant and
    // infinite time control
    ChessGame* game = new ChessGame(Chess::BoardFactory::create("standard"),
                                    new PgnGame());

    game->setTimeControl(TimeControl("inf"));
    game->pause();

    connect(game, SIGNAL(started(ChessGame*)),
            this, SLOT(newGameWindow(ChessGame*)));

    const auto engines = engineManager()->engines();

    /*if (engines.count() > 0)
    {
        EngineConfiguration defaultEngine = engineManager()->engines()[0];

        gameManager()->newGame(game,
                               new EngineBuilder(defaultEngine),
                               new EngineBuilder(defaultEngine));
    }
    else*/
    {
        gameManager()->newGame(game,
                               new HumanBuilder(userName()),
                               new HumanBuilder(userName()));
    }
}

void SylvanApplication::showGameWindow(int index)
{
    auto gameWindow = m_gameWindows.at(index);
    gameWindow->activateWindow();
    gameWindow->raise();
}

GameDatabaseManager* SylvanApplication::gameDatabaseManager()
{
    if (m_gameDatabaseManager == nullptr)
        m_gameDatabaseManager = new GameDatabaseManager(this);

    return m_gameDatabaseManager;
}

void SylvanApplication::showSettingsDialog()
{
    if (m_settingsDialog == nullptr)
        m_settingsDialog = new SettingsDialog();

    showDialog(m_settingsDialog);
}

void SylvanApplication::showTournamentResultsDialog()
{
    showDialog(tournamentResultsDialog());
}

TournamentResultsDialog*SylvanApplication::tournamentResultsDialog()
{
    if (m_tournamentResultsDialog == nullptr)
        m_tournamentResultsDialog = new TournamentResultsDialog();

    return m_tournamentResultsDialog;
}

void SylvanApplication::showGameDatabaseDialog()
{
    if (m_gameDatabaseDialog == nullptr)
        m_gameDatabaseDialog = new GameDatabaseDialog(gameDatabaseManager());

    showDialog(m_gameDatabaseDialog);
}

void SylvanApplication::showGameWall()
{
    if (m_gameWall == nullptr)
    {
        m_gameWall = new GameWall(gameManager());
        auto flags = m_gameWall->windowFlags();
        m_gameWall->setWindowFlags(flags | Qt::Window);
        m_gameWall->setAttribute(Qt::WA_DeleteOnClose, true);
        m_gameWall->setWindowTitle(tr("Currnet Match"));
    }

    showDialog(m_gameWall);
}

void SylvanApplication::onQuitAction()
{
    closeDialogs();
    closeAllWindows();
}

void SylvanApplication::onLastWindowClosed()
{
    if (!m_initialWindowCreated)
        return;

    if (m_gameManager != nullptr)
    {
        connect(m_gameManager, SIGNAL(finished()), this, SLOT(quit()));
        m_gameManager->finish();
    }
    else
        quit();
}

void SylvanApplication::onAboutToQuit()
{
    if (gameDatabaseManager()->isModified())
        gameDatabaseManager()->writeState(configPath() + QLatin1String("/gamedb.bin"));
}

void SylvanApplication::showDialog(QWidget* dlg)
{
    Q_ASSERT(dlg != nullptr);

    if (dlg->isMinimized())
        dlg->showNormal();
    else
        dlg->show();

    dlg->raise();
    dlg->activateWindow();
}

void SylvanApplication::closeDialogs()
{
    if (m_tournamentResultsDialog)
        m_tournamentResultsDialog->close();
    if (m_gameDatabaseDialog)
        m_gameDatabaseDialog->close();
    if (m_settingsDialog)
        m_settingsDialog->close();
    if (m_gameWall)
        m_gameWall->close();
}
