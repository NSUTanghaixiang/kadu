/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_CONTACT_PERSONAL_INFO_SERVICE_H
#define JABBER_CONTACT_PERSONAL_INFO_SERVICE_H

#include "buddies/buddy.h"

#include "protocols/services/contact-personal-info-service.h"

#include "services/jabber-vcard-fetch-callback.h"

namespace XMPP { class JabberProtocol; }

class JabberContactPersonalInfoService : public ContactPersonalInfoService, public XMPP::JabberVCardFetchCallback
{
	Q_OBJECT

	QWeakPointer<XMPP::JabberVCardService> VCardService;
	Buddy CurrentBuddy;

protected:
	virtual void vCardFetched(bool ok, const XMPP::VCard &vCard);

public:
	explicit JabberContactPersonalInfoService(QObject *parent = 0);
	virtual ~JabberContactPersonalInfoService();

	void setVCardService(XMPP::JabberVCardService *vcardService);

	virtual void fetchPersonalInfo(Contact contact);

};

#endif // JABBER_CONTACT_PERSONAL_INFO_SERVICE_H

