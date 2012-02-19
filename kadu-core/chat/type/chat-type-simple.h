/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_TYPE_SIMPLE_H
#define CHAT_TYPE_SIMPLE_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "chat/type/chat-type.h"

#include "exports.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatTypeSimple
 * @author Rafal 'Vogel' Malinowski
 * @short Representation of chat type with one buddy.
 *
 * Representation of chat type with one buddy.
 */
class KADUAPI ChatTypeSimple : public ChatType
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatTypeSimple)

	QStringList Aliases;

public:
	explicit ChatTypeSimple(QObject *parent = 0);
	virtual ~ChatTypeSimple();

	virtual int sortIndex() const;
	virtual QString name() const;
	virtual QStringList aliases() const;
	virtual QString displayName() const;
	virtual QString displayNamePlural() const;
	virtual KaduIcon icon() const;
	virtual QString windowRole() const;

	virtual ChatDetails * createChatDetails(ChatShared *chatData) const;

};

Q_DECLARE_METATYPE(ChatTypeSimple *)

/**
 * @}
 */

#endif // CHAT_TYPE_SIMPLE_H
