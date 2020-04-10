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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QMutex>
#include <QHash>
#include <QMap>
#include <QSqlDatabase>

class QThread;

class DatabaseManager
{
public:
	static QSqlDatabase database(const QString& connectionName = QLatin1String(QSqlDatabase::defaultConnection), const QString dbName=nullptr);
	static void clear();
	static void removeCurrentThread(QString);

private:
	static QMutex s_databaseMutex;
	static QMap<QString, QMap<QString, QSqlDatabase>> s_instances;
};

#endif // DATABASEMANAGER_H
