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

#ifndef SYLVAN_CORE_APPLICATION_H
#define SYLVAN_CORE_APPLICATION_H

#include <QCoreApplication>
#include <QMessageLogContext>

class EngineManager;
class GameManager;

class SylvanCoreApplication : public QCoreApplication
{
	Q_OBJECT

	public:
        SylvanCoreApplication(int& argc, char* argv[]);
        virtual ~SylvanCoreApplication();

		QString configPath();
		EngineManager* engineManager();
		GameManager* gameManager();
        static SylvanCoreApplication* instance();

		static void messageHandler(QtMsgType type,
					   const QMessageLogContext &context,
					   const QString &message);
	private:
		EngineManager* m_engineManager;
		GameManager* m_gameManager;
};

#endif  // SYLVAN_CORE_APPLICATION_H
