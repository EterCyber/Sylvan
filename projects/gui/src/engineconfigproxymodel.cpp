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

#include <QStringList>

#include "engineconfigproxymodel.h"

EngineConfigurationProxyModel::EngineConfigurationProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void EngineConfigurationProxyModel::setFilterVariant(const QString& variant)
{
    m_filterVariant = variant;
    invalidateFilter();
}

bool EngineConfigurationProxyModel::filterAcceptsRow(int sourceRow,
                                                     const QModelIndex& sourceParent) const
{
    QModelIndex variantsIndex = sourceModel()->index(sourceRow, 4, sourceParent);
    QStringList variants(sourceModel()->data(variantsIndex).toStringList());

    if (!m_filterVariant.isEmpty() && !variants.contains(m_filterVariant))
        return false;

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
