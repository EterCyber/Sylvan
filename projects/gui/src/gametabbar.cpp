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

#include "gametabbar.h"

GameTabBar::GameTabBar(QWidget* parent)
    : QTabBar(parent)
{
    setMovable(true);
}

void GameTabBar::showNextTab()
{
    if (count() < 2)
        return;

    if (currentIndex() == count() - 1)
        setCurrentIndex(0);
    else
        setCurrentIndex(currentIndex() + 1);
}

void GameTabBar::showPreviousTab()
{
    if (count() < 2)
        return;

    if (currentIndex() == 0)
        setCurrentIndex(count() - 1);
    else
        setCurrentIndex(currentIndex() - 1);
}
