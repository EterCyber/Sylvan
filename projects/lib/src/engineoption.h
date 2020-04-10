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

#ifndef ENGINEOPTION_H
#define ENGINEOPTION_H

#include <QString>
#include <QVariant>

class LIB_EXPORT EngineOption
{
public:
    explicit EngineOption(const QString& name,
                          QVariant::Type valueType,
                          const QVariant& value = QVariant(),
                          const QVariant& defaultValue = QVariant(),
                          const QString& alias = QString());
    virtual ~EngineOption();

    /*! Creates and returns a deep copy of this option. */
    virtual EngineOption* copy() const = 0;

    QVariant::Type valueType() const;
    bool isValid() const;
    virtual bool isValid(const QVariant& value) const = 0;
    virtual bool isEditable() const;

    QString name() const;
    QVariant value() const;
    QVariant defaultValue() const;
    QString alias() const;

    void setName(const QString& name);
    void setValue(const QVariant& value);
    void setDefaultValue(const QVariant& value);
    void setAlias(const QString& alias);

    virtual QVariant toVariant() const = 0;

private:
    QVariant::Type m_valueType;
    QString m_name;
    QVariant m_value;
    QVariant m_defaultValue;
    QString m_alias;
};

#endif // ENGINEOPTION_H
