/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_AVATAR_PEP_UPLOADER_H
#define JABBER_AVATAR_PEP_UPLOADER_H

#include <QtGui/QImage>

#include <iris/xmpp_pubsubitem.h>
#include <xmpp/jid/jid.h>

#include "accounts/account.h"

namespace XMPP
{
	class Client;

	class JabberProtocol;
}

class JabberPepService;

class JabberAvatarPepUploader : public QObject
{
	Q_OBJECT

	QWeakPointer<XMPP::Client> XmppClient;
	QWeakPointer<JabberPepService> PepService;

	QImage UploadedAvatar;

	QString ItemId;

	void doUpload(const QByteArray &data);
	void doRemove();

private slots:
	void publishSuccess(const QString &ns, const XMPP::PubSubItem &item);
	void publishError(const QString &ns, const XMPP::PubSubItem &item);

public:
	static JabberAvatarPepUploader * createForAccount(const Account &account, QObject *parent);

	explicit JabberAvatarPepUploader(XMPP::Client *xmppClient, JabberPepService *pepService, QObject *parent);
	virtual ~JabberAvatarPepUploader();

	void uploadAvatar(const QImage &avatar, const QByteArray &data);

signals:
	void avatarUploaded(bool ok);

};

#endif // JABBER_AVATAR_UPLOADER_H
