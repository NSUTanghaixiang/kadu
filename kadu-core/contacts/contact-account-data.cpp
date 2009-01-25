/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dnshandler.h"
#include "xml_config_file.h"

#include "configuration/storage-point.h"

#include "contact-account-data.h"

ContactAccountData::ContactAccountData(Contact contact)
	: ContactAccount(0), OwnerContact(contact), Id(QString::null), Blocked(false), OfflineTo(false)
{
}

ContactAccountData::ContactAccountData(Contact contact, Account *account, const QString &id)
	: ContactAccount(account), OwnerContact(contact), Id(id), Blocked(false), OfflineTo(false)
{
}

ContactAccountData::ContactAccountData(Contact contact, Account *account, StoragePoint *sp)
	: ContactAccount(account), OwnerContact(contact), Blocked(false), OfflineTo(false)
{
	setStorage(sp);
	loadConfiguration();
}

StoragePoint * ContactAccountData::createStoragePoint() const
{
	return ContactAccount
		? OwnerContact.storagePointForAccountData(ContactAccount, true)
		: 0;
}

void ContactAccountData::loadConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return;

	Id = sp->storage()->getTextNode(sp->point(), "Id");
}

void ContactAccountData::storeConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return;

	sp->storage()->createTextNode(sp->point(), "Id", Id);
}

bool ContactAccountData::isValid()
{
	return validateId();
}

void ContactAccountData::setAddressAndPort(QHostAddress address, unsigned int port)
{
	Address = address;
	Port = port;
}

void ContactAccountData::refreshDNSName()
{
	if (!(Address.isNull()))
		connect(new DNSHandler(Id, Address), SIGNAL(result(const QString &, const QString &)),
				this, SLOT(setDNSName(const QString &, const QString &)));
}
