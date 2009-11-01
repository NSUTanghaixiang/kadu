/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/account-data/contact-account-data.h"

#include "open-chat-with-contact-list-runner.h"

BuddyList OpenChatWithContactListRunner::matchingContacts(const QString &query)
{
	BuddyList matchedContacts;

	foreach (Buddy contact, BuddyManager::instance()->buddies())
	{
		bool found = false;
		foreach (ContactAccountData *data, contact.accountDatas())
			if (data->id().contains(query, Qt::CaseInsensitive))
			{
				matchedContacts.append(contact);
				found = true;
				break;
			}

		if (found)
			continue;
		
		if (contact.firstName().contains(query, Qt::CaseInsensitive) ||
				contact.lastName().contains(query, Qt::CaseInsensitive) ||
				contact.display().contains(query, Qt::CaseInsensitive) ||
				contact.nickName().contains(query, Qt::CaseInsensitive))
			matchedContacts.append(contact);
	}

	return matchedContacts;
}
