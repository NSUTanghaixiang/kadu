/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "core/application.h"
#include "core/core.h"
#include "identities/identity-manager.h"
#include "misc/misc.h"
#include "network/proxy/network-proxy-manager.h"
#include "protocols/protocols-manager.h"
#include "roster/roster.h"

#include "gadu-account-details.h"
#include "gadu-protocol-factory.h"

#include "gadu-importer.h"

const QString GaduImporter::EntryQuery("/Kadu/Deprecated/ConfigFile[ends-with(@name,'kadu.conf')]/Group[@name='%1']/Entry[@name='%2']/@value/string()");
const QString GaduImporter::ContactsQuery("/Kadu/Contacts/Contact");

GaduImporter::GaduImporter(QObject *parent) :
		QObject{parent}
{
}

GaduImporter::~GaduImporter()
{
}


bool GaduImporter::alreadyImported()
{
	QDomElement node = Application::instance()->configuration()->api()->getNode("Accounts", ConfigurationApi::ModeFind);
	if (node.isNull())
		return false;

	return node.hasAttribute("import_done");
}

void GaduImporter::markImported()
{
	QDomElement node = Application::instance()->configuration()->api()->getNode("Accounts", ConfigurationApi::ModeFind);
	node.setAttribute("import_done", "true");
}

QVariant GaduImporter::readEntry(QXmlQuery &xmlQuery, const QString &groupName, const QString &entryName, const QVariant &defaultValue)
{
	xmlQuery.setQuery(EntryQuery.arg(groupName).arg(entryName));

	QString result;
	if (xmlQuery.evaluateTo(&result))
		return result.trimmed();
	else
		return defaultValue;
}

void GaduImporter::importAccounts()
{
	quint32 importUin = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General", "UIN");
	if (0 == importUin)
		return;

	if (alreadyImported())
		return;

	QString importUinString = QString::number(importUin);

	if (AccountManager::instance()->byId("gadu", importUinString))
		return;

	Account defaultGaduGadu = Account::create("gadu");

	defaultGaduGadu.setId(importUinString);
	defaultGaduGadu.setPassword(pwHash(Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Password")));
	defaultGaduGadu.setRememberPassword(true);
	defaultGaduGadu.setHasPassword(!defaultGaduGadu.password().isEmpty());
	defaultGaduGadu.setPrivateStatus(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "PrivateStatus"));

	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	if (!IdentityManager::instance()->items().isEmpty())
		defaultGaduGadu.setAccountIdentity(IdentityManager::instance()->items().at(0));

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(defaultGaduGadu.details());
	if (accountDetails)
	{
		accountDetails->setState(StorableObject::StateNew);
		accountDetails->setReceiveImagesDuringInvisibility(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ReceiveImagesDuringInvisibility"));
	}

	QString address = Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "ProxyHost");
	if (!address.isEmpty())
	{
		int port = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Network", "ProxyPort");
		QString user = Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "ProxyUser");
		QString password = Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "ProxyPassword");

		NetworkProxy networkProxy = NetworkProxyManager::instance()->byConfiguration(
		            address, port, user, password, ActionCreateAndAdd);
		if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Network", "UseProxy"))
			defaultGaduGadu.setProxy(networkProxy);
	}

	AccountManager::instance()->addItem(defaultGaduGadu);
	defaultGaduGadu.accountContact().setOwnerBuddy(Core::instance()->myself());

	markImported();
}

void GaduImporter::importContacts()
{
	connect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy)),
			this, SLOT(buddyAdded(Buddy)));

	foreach (Buddy buddy, BuddyManager::instance()->items())
		buddyAdded(buddy);

	importIgnored();
}

Contact GaduImporter::importGaduContact(Account account, Buddy buddy)
{
	QString id = buddy.customData("uin");

	Contact contact = ContactManager::instance()->byId(account, id, ActionCreateAndAdd);

	buddy.removeCustomData("uin");
	buddy.setBlocked(QVariant(buddy.customData("blocking")).toBool());
	buddy.setOfflineTo(QVariant(buddy.customData("offline_to")).toBool());
	buddy.removeCustomData("blocking");
	buddy.removeCustomData("offline_to");

	contact.setOwnerBuddy(buddy);

	Roster::instance()->addContact(contact);

	return contact;
}

void GaduImporter::importIgnored()
{
	Account account = AccountManager::instance()->defaultAccount();
	if (account.isNull())
		return;

	QDomElement ignored = Application::instance()->configuration()->api()->getNode("Ignored", ConfigurationApi::ModeFind);
	if (ignored.isNull())
		return;

	QVector<QDomElement> ignoredGroups = Application::instance()->configuration()->api()->getNodes(ignored, "IgnoredGroup");
	foreach (const QDomElement &ignoredGroup, ignoredGroups)
	{
		QVector<QDomElement> ignoredContacts = Application::instance()->configuration()->api()->getNodes(ignoredGroup, "IgnoredContact");
		if (1 == ignoredContacts.count())
		{
			QDomElement ignoredContact = ignoredContacts.at(0);
			Buddy buddy = BuddyManager::instance()->byId(account, ignoredContact.attribute("uin"), ActionCreateAndAdd);
			buddy.setBlocked(true);
		}
	}

	Application::instance()->configuration()->api()->removeNode(Application::instance()->configuration()->api()->rootElement(), "Ignored");
}

void GaduImporter::buddyAdded(const Buddy &buddy)
{
	if (buddy.customData("uin").isEmpty())
		return;

	QVector<Account> allGaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	if (allGaduAccounts.isEmpty())
		return;

	// take 1st one
	Account account = allGaduAccounts.at(0);

	importGaduContact(account, buddy);
}

#include "moc_gadu-importer.cpp"
