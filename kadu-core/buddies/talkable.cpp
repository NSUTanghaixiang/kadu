/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "avatars/avatar.h"
#include "buddies/buddy-preferred-manager.h"
#include "model/roles.h"

#include "talkable.h"

Talkable::Talkable() :
		Type(ItemNone)
{
}

Talkable::Talkable(const Buddy &buddy) :
		Type(ItemBuddy), MyBuddy(buddy)
{
}

Talkable::Talkable(const Contact &contact) :
		Type(ItemContact), MyContact(contact)
{
}

Talkable::Talkable(const Talkable &copyMe)
{
	Type = copyMe.Type;

	switch (Type)
	{
		case ItemBuddy:
			MyBuddy = copyMe.MyBuddy;
			break;
		case ItemContact:
			MyContact = copyMe.MyContact;
			break;
		default:
			break;
	}
}

Talkable & Talkable::operator = (const Talkable &copyMe)
{
	Type = copyMe.Type;

	switch (Type)
	{
		case ItemBuddy:
			MyBuddy = copyMe.MyBuddy;
			break;
		case ItemContact:
			MyContact = copyMe.MyContact;
			break;
		default:
			break;
	}

	return *this;
}

bool Talkable::operator == (const Talkable &compareTo) const
{
	if (Type != compareTo.Type)
		return false;

	switch (Type)
	{
		case ItemNone: return true;
		case ItemBuddy: return MyBuddy == compareTo.MyBuddy;
		case ItemContact: return MyContact == compareTo.MyContact;
		default:
			return false;
	}
}

bool Talkable::operator != (const Talkable &compareTo) const
{
	return !(*this == compareTo);
}

Buddy Talkable::buddy() const
{
	switch (Type)
	{
		case ItemBuddy: return MyBuddy;
		case ItemContact: return MyContact.ownerBuddy();
		default:
			return Buddy::null;
	}
}

Contact Talkable::contact() const
{
	switch (Type)
	{
		case ItemBuddy: return BuddyPreferredManager::instance()->preferredContact(MyBuddy);
		case ItemContact: return MyContact;
		default:
			return Contact::null;
	}
}

bool Talkable::isEmpty() const
{
	switch (Type)
	{
		case ItemBuddy: return MyBuddy.isNull();
		case ItemContact: return MyContact.isNull();
		default:
			return true;
	}
}

Avatar Talkable::avatar() const
{
	Avatar avatar;
	if (Talkable::ItemBuddy == Type)
		avatar = buddy().buddyAvatar();

	if (!avatar || avatar.pixmap().isNull())
		avatar = contact().contactAvatar();

	return avatar;
}

bool Talkable::isBlocked() const
{
	return buddy().isBlocked();
}

bool Talkable::isBlocking() const
{
	return contact().isBlocking();
}

Account Talkable::account() const
{
	return contact().contactAccount();
}

Status Talkable::currentStatus() const
{
	return contact().currentStatus();
}
