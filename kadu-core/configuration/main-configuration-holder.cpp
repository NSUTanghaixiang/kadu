/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "misc/kadu-paths.h"

#include "main-configuration-holder.h"

MainConfigurationHolder *MainConfigurationHolder::Instance = 0;

MainConfigurationHolder * MainConfigurationHolder::instance()
{
	return Instance;
}

void MainConfigurationHolder::createInstance()
{
	if (!Instance)
		Instance = new MainConfigurationHolder();
}

void MainConfigurationHolder::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

MainConfigurationHolder::MainConfigurationHolder() :
		SetStatus(SetStatusPerIdentity)
{
	configurationUpdated();
}

void MainConfigurationHolder::configurationUpdated()
{
	QString statusContainerType = config_file.readEntry("General", "StatusContainerType", "Identity");

	SetStatusMode newStatus = SetStatusPerIdentity;
	if (statusContainerType == "Account")
		newStatus = SetStatusPerAccount;
	else if (statusContainerType == "All")
		newStatus = SetStatusForAll;

	if (SetStatus != newStatus)
	{
		SetStatus = newStatus;
		emit setStatusModeChanged();
	}
}

#include "moc_main-configuration-holder.cpp"
