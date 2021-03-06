/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GROUP_FILTER_TAB_DATA_H
#define GROUP_FILTER_TAB_DATA_H

#include "talkable/filter/group-filter.h"

class IconsManager;

class GroupFilterTabData
{
    GroupFilter Filter;

public:
    explicit GroupFilterTabData(const GroupFilter &filter);

    GroupFilter filter() const;

    QString tabName() const;
    QIcon tabIcon(IconsManager *iconsManager) const;
};

#endif   // GROUP_FILTER_TAB_DATA_H
