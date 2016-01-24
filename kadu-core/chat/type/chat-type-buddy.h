/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "chat/type/chat-type.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <injeqt/injeqt.h>

class InjectedFactory;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatTypeBuddy
 * @author Rafal 'Vogel' Malinowski
 * @short Representation of chat type that contains other chats.
 *
 * Representation of chat type that contains other chats.
 */
class KADUAPI ChatTypeBuddy : public ChatType
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatTypeBuddy(QObject *parent = nullptr);
	virtual ~ChatTypeBuddy();

	virtual QString name() const;
	virtual QStringList aliases() const;
	virtual KaduIcon icon() const;
	virtual QString windowRole() const;

	virtual ChatDetails * createChatDetails(ChatShared *chatData) const;
	virtual ChatEditWidget *createEditWidget(const Chat &chat, QWidget *parent) const;

private:
	QPointer<InjectedFactory> m_injectedFactory;

	QStringList m_aliases;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

};

Q_DECLARE_METATYPE(ChatTypeBuddy *)

/**
 * @}
 */
