/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "chat/model/chats-model.h"
#include "core/core.h"

#include "talkable-model.h"

TalkableModel::TalkableModel(QObject *parent) :
		KaduMergedProxyModel(parent), IncludeMyself(false)
{
	Chats = new ChatsModel(this);
	Buddies = new BuddyListModel(this);
	BuddiesAdapter = new BuddyManagerAdapter(Buddies);

	QList<QAbstractItemModel *> models;
	models.append(Chats);
	models.append(Buddies);
	setModels(models);
}

TalkableModel::~TalkableModel()
{
}

void TalkableModel::setIncludeMyself(bool includeMyself)
{
	if (IncludeMyself == includeMyself)
		return;

	IncludeMyself = includeMyself;
	if (IncludeMyself)
		Buddies->addBuddy(Core::instance()->myself());
	else
		Buddies->removeBuddy(Core::instance()->myself());
}

bool TalkableModel::includeMyself() const
{
	return IncludeMyself;
}
