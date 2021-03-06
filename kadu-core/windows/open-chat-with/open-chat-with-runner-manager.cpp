/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "open-chat-with-runner-manager.h"
#include "open-chat-with-runner-manager.moc"

#include "buddies/buddy.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

OpenChatWithRunnerManager *OpenChatWithRunnerManager::Instance = 0;

OpenChatWithRunnerManager *OpenChatWithRunnerManager::instance()
{
    if (!Instance)
        Instance = new OpenChatWithRunnerManager();

    return Instance;
}

OpenChatWithRunnerManager::OpenChatWithRunnerManager()
{
}

OpenChatWithRunnerManager::~OpenChatWithRunnerManager()
{
}

void OpenChatWithRunnerManager::registerRunner(OpenChatWithRunner *runner)
{
    Runners.append(runner);
}

void OpenChatWithRunnerManager::unregisterRunner(OpenChatWithRunner *runner)
{
    Runners.removeAll(runner);
}

BuddyList OpenChatWithRunnerManager::matchingContacts(const QString &query)
{
    BuddyList matchingContacts;
    for (auto runner : Runners)
        matchingContacts += runner->matchingContacts(query);

    return matchingContacts;
}
