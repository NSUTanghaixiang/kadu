/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"

#include "encryption-ng-configuration.h"

EncryptionNgConfiguration * EncryptionNgConfiguration::Instance = 0;

EncryptionNgConfiguration * EncryptionNgConfiguration::instance()
{
	return Instance;
}

void EncryptionNgConfiguration::createInstance()
{
	if (!Instance)
		Instance = new EncryptionNgConfiguration();
}

void EncryptionNgConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionNgConfiguration::EncryptionNgConfiguration()
{
	configurationUpdated();

	createDefaultConfiguration();
}

EncryptionNgConfiguration::~EncryptionNgConfiguration()
{

}

void EncryptionNgConfiguration::createDefaultConfiguration()
{
	config_file.addVariable("Chat", "Encryption", true);
	config_file.addVariable("Chat", "EncryptAfterReceiveEncryptedMessage", true);
}

void EncryptionNgConfiguration::configurationUpdated()
{
	EncryptByDefault = config_file.readBoolEntry("Chat", "Encryption", true);
	EncryptAfterReceiveEncryptedMessage = config_file.readBoolEntry("Chat", "EncryptAfterReceiveEncryptedMessage", true);
}
