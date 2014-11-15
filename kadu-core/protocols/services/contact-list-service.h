/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef CONTAT_LIST_SERVICE_H
#define CONTAT_LIST_SERVICE_H

#include "buddies/buddy-list.h"
#include "exports.h"

#include "account-service.h"

class Contact;

class KADUAPI ContactListService : public AccountService
{
	Q_OBJECT

	bool askForAddingContacts(const QMap<Buddy, Contact> &contactsToAdd, const QMap<Buddy, Contact> &contactsToRename);
	QVector<Contact> performAdds(const QMap<Buddy, Contact> &contactsToAdd);
	void performRenames(const QMap<Buddy, Contact> &contactsToRename);
	QVector<Contact> registerBuddies(const BuddyList &buddies, bool ask);

public:
	explicit ContactListService(Account account, QObject *parent = 0);
	virtual ~ContactListService();

	void copySupportedBuddyInformation(const Buddy &destination, const Buddy &source);

	QList<Contact> setBuddiesList(const BuddyList &buddies, bool ask);

};

#endif // CONTAT_LIST_SERVICE_H
