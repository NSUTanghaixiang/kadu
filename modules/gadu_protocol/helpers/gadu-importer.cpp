/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/account-shared.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "buddies/buddy.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-protocol-factory.h"

#include "gadu-importer.h"

GaduImporter * GaduImporter::Instance;

GaduImporter * GaduImporter::instance()
{
	if (0 == Instance)
		Instance = new GaduImporter();

	return Instance;
}

void GaduImporter::importAccounts()
{
	if (0 == config_file.readNumEntry("General", "UIN"))
		return;

	Account defaultGaduGadu = Account::create();
	defaultGaduGadu.setProtocolName("gadu");

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(defaultGaduGadu.details());
	accountDetails->setState(StorableObject::StateNew);

	if (!IdentityManager::instance()->items().isEmpty())
		defaultGaduGadu.setAccountIdentity(IdentityManager::instance()->items()[0]);

	defaultGaduGadu.setId(config_file.readEntry("General", "UIN"));
	defaultGaduGadu.setPassword(pwHash(config_file.readEntry("General", "Password")).toUtf8().constData());
	defaultGaduGadu.setRememberPassword(true);
	defaultGaduGadu.setHasPassword(!defaultGaduGadu.password().isEmpty());
	accountDetails->setAllowDcc(config_file.readBoolEntry("Network", "AllowDCC"));

	QHostAddress host;
	if (!host.setAddress(config_file.readEntry("Network", "DccIP")))
		host.setAddress("0.0.0.0");
	accountDetails->setDccIP(host);
	if (!host.setAddress(config_file.readEntry("Network", "ExternalIP")))
		host.setAddress("0.0.0.0");
	accountDetails->setDccExternalIP(host);

	accountDetails->setDccExternalPort(config_file.readNumEntry("Network", "ExternalPort"));
	accountDetails->setDccPort(config_file.readNumEntry("Network", "ExternalPort"));
	accountDetails->setDccIpDetect(config_file.readBoolEntry("Network", "DccIpDetect"));
	accountDetails->setDccLocalPort(config_file.readNumEntry("Network", "LocalPort"));
	accountDetails->setDccForwarding(config_file.readBoolEntry("Network", "DccForwarding"));
	accountDetails->setMaximumImageSize(config_file.readNumEntry("Chat", "MaxImageSize", 255));
	accountDetails->setReceiveImagesDuringInvisibility(config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility"));
	accountDetails->setMaximumImageRequests(config_file.readNumEntry("Chat", "MaxImageRequests"));
	accountDetails->setRemoveCompletedTransfers(config_file.readBoolEntry("Network", "RemoveCompletedTransfers"));

	defaultGaduGadu.setUseProxy(config_file.readBoolEntry("Network", "UseProxy"));
	if (!host.setAddress(config_file.readEntry("Network", "ProxyHost")))
		host.setAddress("0.0.0.0");
	defaultGaduGadu.setProxyHost(host);
	defaultGaduGadu.setProxyPassword(config_file.readEntry("Network", "ProxyPassword"));
	defaultGaduGadu.setProxyPort(config_file.readNumEntry("Network", "ProxyPort"));
	defaultGaduGadu.setProxyUser(config_file.readEntry("Network", "ProxyUser"));
	defaultGaduGadu.setProxyRequiresAuthentication(!defaultGaduGadu.proxyUser().isEmpty());

	accountDetails->import_0_6_5_LastStatus();

	AccountManager::instance()->addItem(defaultGaduGadu);
}

void GaduImporter::importContacts()
{
	connect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));

	foreach (Buddy buddy, BuddyManager::instance()->items())
		buddyAdded(buddy);

	importIgnored();
}

void GaduImporter::importGaduContact(Buddy &buddy)
{
	QList<Account> allGaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	if (0 == allGaduAccounts.count())
		return;

	// take 1st one
	Account account = allGaduAccounts[0];
	QString id = buddy.customData("uin");

	Contact contact = ContactManager::instance()->byId(account, id, ActionCreateAndAdd);

	buddy.removeCustomData("uin");
	buddy.setBlocked(QVariant(buddy.customData("blocking")).toBool());
	buddy.setOfflineTo(QVariant(buddy.customData("offline_to")).toBool());
	buddy.removeCustomData("blocking");
	buddy.removeCustomData("offline_to");

	contact.setOwnerBuddy(buddy);

	ContactManager::instance()->addItem(contact);
}

void GaduImporter::importIgnored()
{
	Account account = AccountManager::instance()->defaultAccount();
	if (account.isNull())
		return;

	QDomElement ignored = xml_config_file->getNode("Ignored", XmlConfigFile::ModeFind);
	if (ignored.isNull())
		return;

	QList<QDomElement> ignoredGroups = xml_config_file->getNodes(ignored, "IgnoredGroup");
	foreach (QDomElement ignoredGroup, ignoredGroups)
	{
		QList<QDomElement> ignoredContacts = xml_config_file->getNodes(ignoredGroup, "IgnoredContact");
		foreach (QDomElement ignoredContact, ignoredContacts)
		{
			Buddy buddy = BuddyManager::instance()->byId(account, ignoredContact.attribute("uin"), ActionCreateAndAdd);
			buddy.setBlocked(true);
		}
	}
}

void GaduImporter::buddyAdded(Buddy &buddy)
{
	if (!buddy.customData("uin").isEmpty())
		importGaduContact(buddy);
}
