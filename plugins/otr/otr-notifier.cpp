/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "notify/notify-event.h"
#include "notify/notification-manager.h"
#include "notify/notification/chat-notification.h"

#include "otr-notifier.h"

QString OtrNotifier::OtrNotifyTopic("OTR");
QString OtrNotifier::TryToStartSessionNotifyTopic("OTR/TryToStartSession");
QString OtrNotifier::GoneSecureNotifyTopic("OTR/GoneSecure");
QString OtrNotifier::GoneInsecureNotifyTopic("OTR/GoneInsecure");
QString OtrNotifier::StillSecureNotifyTopic("OTR/StillSecure");

OtrNotifier::OtrNotifier(QObject *parent) :
		QObject(parent)
{
	OtrNotifyEvent.reset(new NotifyEvent(OtrNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "OTR Encryption")));
	TryToStartSessionNotifyEvent.reset(new NotifyEvent(TryToStartSessionNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Try to start private conversation")));
	GoneSecureNotifyEvent.reset(new NotifyEvent(GoneSecureNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Conversation gone secure")));
	GoneInsecureNotifyEvent.reset(new NotifyEvent(GoneInsecureNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Conversation gone insecure")));
	StillSecureNotifyEvent.reset(new NotifyEvent(StillSecureNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Conversation still secure")));
}

OtrNotifier::~OtrNotifier()
{
}

QList<NotifyEvent *> OtrNotifier::notifyEvents()
{
	return QList<NotifyEvent *>()
			<< OtrNotifyEvent.data()
			<< TryToStartSessionNotifyEvent.data()
			<< GoneSecureNotifyEvent.data()
			<< GoneInsecureNotifyEvent.data()
			<< StillSecureNotifyEvent.data();
}

void OtrNotifier::notifyTryToStartSession(const Chat &chat)
{
	ChatNotification *notification = new ChatNotification(chat, TryToStartSessionNotifyTopic, KaduIcon());
	notification->setTitle(tr("OTR Encryption"));
	notification->setText(tr("Trying to start private conversation"));

	NotificationManager::instance()->notify(notification);
}

void OtrNotifier::notifyGoneSecure(const Chat &chat)
{
	ChatNotification *notification = new ChatNotification(chat, GoneSecureNotifyTopic, KaduIcon());
	notification->setTitle(tr("OTR Encryption"));
	notification->setText(tr("Private conversation started"));

	NotificationManager::instance()->notify(notification);
}

void OtrNotifier::notifyGoneInsecure(const Chat &chat)
{
	ChatNotification *notification = new ChatNotification(chat, GoneInsecureNotifyTopic, KaduIcon());
	notification->setTitle(tr("OTR Encryption"));
	notification->setText(tr("Private conversation stopped"));

	NotificationManager::instance()->notify(notification);
}

void OtrNotifier::notifyStillSecure(const Chat &chat)
{
	ChatNotification *notification = new ChatNotification(chat, StillSecureNotifyTopic, KaduIcon());
	notification->setTitle(tr("OTR Encryption"));
	notification->setText(tr("Conversation is still private"));

	NotificationManager::instance()->notify(notification);
}
