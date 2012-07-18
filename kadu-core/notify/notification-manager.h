/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QtCore/QTimer>
#include <QtGui/QGroupBox>

#include "accounts/accounts-aware-object.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "status/status.h"

class Action;
class Group;
class Message;
class MultilogonSession;
class Notification;
class Notifier;
class NotifyEvent;

/**
 * @defgroup notify Notify
 * @{
 */

class KADUAPI NotificationManager : public QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NotificationManager)

	static NotificationManager *Instance;

	void init();

	QList<Notifier *> Notifiers;
	QList<NotifyEvent *> NotifyEvents;

	NotificationManager();
	virtual ~NotificationManager();

private slots:
	void multilogonSessionConnected(MultilogonSession *session);
	void multilogonSessionDisconnected(MultilogonSession *session);

	void statusUpdated();
	void contactStatusChanged(Contact contact, Status oldStatus);

	void groupAdded(const Group &group);
	void groupUpdated();

	void accountConnected();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static NotificationManager * instance();

	void notify(Notification *notification);

	void registerNotifier(Notifier *notifier);
	void unregisterNotifier(Notifier *notifier);

	void registerNotifyEvent(NotifyEvent *notifyEvent);
	void unregisterNotifyEvent(NotifyEvent *notifyEvent);

	const QList<Notifier *> & notifiers() const;
	const QList<NotifyEvent *> & notifyEvents() const;

	QString notifyConfigurationKey(const QString &eventType);

signals:
	void notiferRegistered(Notifier *notifier);
	void notiferUnregistered(Notifier *notifier);

	void notifyEventRegistered(NotifyEvent *notifyEvent);
	void notifyEventUnregistered(NotifyEvent *notifyEvent);

};

void checkNotify(Action *);

/** @} */

#endif // NOTIFICATION_MANAGER_H
