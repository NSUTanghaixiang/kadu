/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <stdio.h>

#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "buddies/filter/abstract-buddy-filter.h"
#include "chat/chat.h"
#include "chat/filter/chat-filter.h"
#include "chat/type/chat-type.h"
#include "contacts/contact-set.h"
#include "model/history-chats-model.h"
#include "model/roles.h"
#include "talkable/filter/talkable-filter.h"

#include "history-chats-model-proxy.h"

HistoryChatsModelProxy::HistoryChatsModelProxy(QObject *parent) :
		QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

int HistoryChatsModelProxy::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
			? n1.toLower().localeAwareCompare(n2.toLower())
			: n1.localeAwareCompare(n2);
}

bool HistoryChatsModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QModelIndex sourceChild = sourceParent.child(sourceRow, 0);
	Chat chat = sourceChild.data(ChatRole).value<Chat>();
	if (chat)
	{
		foreach (TalkableFilter *filter, TalkableFilters)
			switch (filter->filterChat(chat))
			{
				case TalkableFilter::Accepted: return true;
				case TalkableFilter::Undecided: break;
				case TalkableFilter::Rejected: return false;
			}

		return true;
	}

	Buddy buddy = sourceChild.data(BuddyRole).value<Buddy>();
	if (buddy)
	{
		foreach (TalkableFilter *filter, TalkableFilters)
			switch (filter->filterBuddy(buddy))
			{
				case TalkableFilter::Accepted: return true;
				case TalkableFilter::Undecided: break;
				case TalkableFilter::Rejected: return false;
			}

		foreach (AbstractBuddyFilter *filter, BuddyFilters)
			if (!filter->acceptBuddy(buddy))
				return false;

		return true;
	}

	return true;
}

bool HistoryChatsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	// chats?
	Chat leftChat = left.data(ChatRole).value<Chat>();
	Chat rightChat = right.data(ChatRole).value<Chat>();

	if (!leftChat.isNull() && !rightChat.isNull())
	{
		bool isLeftAllAnonymous = leftChat.contacts().toBuddySet().isAllAnonymous();
		bool isRightAllAnonymous = rightChat.contacts().toBuddySet().isAllAnonymous();

		if (isLeftAllAnonymous && !isRightAllAnonymous)
			return false;
		if (isRightAllAnonymous && !isLeftAllAnonymous)
			return true;

		const QString &leftName = leftChat.display().isEmpty() ? leftChat.name() : leftChat.display();
		const QString &rightName = rightChat.display().isEmpty() ? rightChat.name() : rightChat.display();

		return compareNames(leftName, rightName) < 0;
	}

	ChatType *leftType = left.data(ChatTypeRole).value<ChatType *>();
	ChatType *rightType = right.data(ChatTypeRole).value<ChatType *>();

	QString leftName = leftType ? leftType->displayName() : left.data(Qt::DisplayRole).toString();
	QString rightName = rightType ? rightType->displayName() : right.data(Qt::DisplayRole).toString();

	if (!leftType && rightType)
		return false;
	if (!rightType && leftType)
		return true;

	return compareNames(leftName, rightName) < 0;
}

void HistoryChatsModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
	QSortFilterProxyModel::setSourceModel(sourceModel);
	Model = qobject_cast<HistoryChatsModel *>(sourceModel);
}

void HistoryChatsModelProxy::addTalkableFilter(TalkableFilter *filter)
{
	if (!filter)
		return;

	TalkableFilters.append(filter);
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));

	invalidateFilter();
}

void HistoryChatsModelProxy::removeTalkableFilter(TalkableFilter *filter)
{
	if (!filter)
		return;

	TalkableFilters.removeAll(filter);
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));

	invalidateFilter();
}

void HistoryChatsModelProxy::addBuddyFilter(AbstractBuddyFilter *filter)
{
	if (!filter)
		return;

	BuddyFilters.append(filter);
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));

	invalidateFilter();
}

void HistoryChatsModelProxy::removeBuddyFilter(AbstractBuddyFilter *filter)
{
	if (!filter)
		return;

	BuddyFilters.removeAll(filter);
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));

	invalidateFilter();
}

QModelIndex HistoryChatsModelProxy::chatTypeIndex(ChatType *type) const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->chatTypeIndex(type);
	return mapFromSource(index);
}

QModelIndex HistoryChatsModelProxy::chatIndex(const Chat &chat) const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->chatIndex(chat);
	return mapFromSource(index);
}

QModelIndex HistoryChatsModelProxy::statusIndex() const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->statusIndex();
	return mapFromSource(index);
}

QModelIndex HistoryChatsModelProxy::statusBuddyIndex(const Buddy &buddy) const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->statusBuddyIndex(buddy);
	return mapFromSource(index);
}

QModelIndex HistoryChatsModelProxy::smsIndex() const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->smsIndex();
	return mapFromSource(index);
}

QModelIndex HistoryChatsModelProxy::smsRecipientIndex(const QString& smsRecipient) const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->smsRecipientIndex(smsRecipient);
	return mapFromSource(index);
}
