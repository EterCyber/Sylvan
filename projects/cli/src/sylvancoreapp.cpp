/*
    This file is part of Sylvan.

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

#include "sylvancoreapp.h"
#include <QSettings>
#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <mersenne.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <cstdlib>
#include <cstdio>


SylvanCoreApplication::SylvanCoreApplication(int& argc, char* argv[])
	: QCoreApplication(argc, argv),
	  m_engineManager(nullptr),
	  m_gameManager(nullptr)
{
	Mersenne::initialize(QTime(0,0,0).msecsTo(QTime::currentTime()));

    QCoreApplication::setOrganizationName(QLatin1String("EterCyber"));
    QCoreApplication::setApplicationName(QLatin1String("Sylvan"));

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

    qInstallMessageHandler(SylvanCoreApplication::messageHandler);

	// Load the engines
	QString configFile("engines.json");
	if (!QFile::exists(configFile))
		configFile = configPath() + "/" + configFile;
	engineManager()->loadEngines(configFile);
}

SylvanCoreApplication::~SylvanCoreApplication()
{
}

void SylvanCoreApplication::messageHandler(QtMsgType type,
					      const QMessageLogContext &context,
					      const QString &message)
{
	QByteArray msg = message.toLocal8Bit();
	switch (type)
	{
	case QtInfoMsg:
		fprintf(stdout, "%s\n", msg.constData());
		break;
	case QtDebugMsg:
		fprintf(stdout, "Debug: %s\n", msg.constData());
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s\n", msg.constData());
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s (%s:%u, %s)\n", msg.constData(),
			context.file, context.line, context.function);
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", msg.constData(),
			context.file, context.line, context.function);
		abort();
	}
}

QString SylvanCoreApplication::configPath()
{
	// QDesktopServices requires QtGui
	QSettings settings;
	QFileInfo fi(settings.fileName());
	QDir dir(fi.absolutePath());

	if (!dir.exists())
		dir.mkpath(fi.absolutePath());

	return fi.absolutePath();
}

EngineManager* SylvanCoreApplication::engineManager()
{
	if (m_engineManager == nullptr)
		m_engineManager = new EngineManager(this);

	return m_engineManager;
}

GameManager* SylvanCoreApplication::gameManager()
{
	if (m_gameManager == nullptr)
		m_gameManager = new GameManager(this);

	return m_gameManager;
}

SylvanCoreApplication* SylvanCoreApplication::instance()
{
    return static_cast<SylvanCoreApplication*>(QCoreApplication::instance());
}
