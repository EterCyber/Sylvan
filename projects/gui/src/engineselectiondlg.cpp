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

#include "engineconfigproxymodel.h"
#include "engineselectiondlg.h"
#include "ui_engineselectiondlg.h"

EngineSelectionDialog::EngineSelectionDialog(EngineConfigurationProxyModel* model,
                                             QWidget* parent)
    : QDialog(parent),
      m_model(model),
      ui(new Ui::EngineSelectionDialog)
{
    Q_ASSERT(model != nullptr);
    ui->setupUi(this);

    ui->m_enginesList->setModel(m_model);

    connect(ui->m_engineFilterEdit, SIGNAL(textChanged(QString)),
            m_model, SLOT(setFilterWildcard(QString)));
}

EngineSelectionDialog::~EngineSelectionDialog()
{
    m_model->setFilterWildcard(QString());
    delete ui;
}

QItemSelection EngineSelectionDialog::selection() const
{
    return m_model->mapSelectionToSource(ui->m_enginesList->selectionModel()->selection());
}

QListView* EngineSelectionDialog::enginesList() const
{
    return ui->m_enginesList;
}
