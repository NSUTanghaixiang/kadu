/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#   include <libotr/privkey.h>
}

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "misc/kadu-paths.h"

#include "otr-create-private-key-job.h"
#include "otr-user-state.h"

#include "otr-private-key-service.h"

OtrPrivateKeyService::OtrPrivateKeyService(QObject *parent) :
		QObject(parent), UserState(0)
{
}

OtrPrivateKeyService::~OtrPrivateKeyService()
{
}

void OtrPrivateKeyService::setUserState(OtrUserState *userState)
{
	UserState = userState;
}

QString OtrPrivateKeyService::privateStoreFileName()
{
	return KaduPaths::instance()->profilePath() + QString("/keys/otr_private");
}

void OtrPrivateKeyService::createPrivateKey(const Account &account)
{
	if (CreateJobs.contains(account))
		return;

	OtrCreatePrivateKeyJob *job = new OtrCreatePrivateKeyJob(this);
	job->setAccount(account);
	job->setUserState(UserState);
	job->setPrivateStoreFileName(privateStoreFileName());
	job->createPrivateKey();

	connect(job, SIGNAL(finished(Account, bool)), this, SLOT(jobFinished(Account, bool)));

	CreateJobs.insert(account, job);
	emit createPrivateKeyStarted(account);
}

void OtrPrivateKeyService::jobFinished(const Account &account, bool ok)
{
	emit createPrivateKeyFinished(account, ok);
	CreateJobs.value(account)->deleteLater();
	CreateJobs.remove(account);
}

void OtrPrivateKeyService::readPrivateKeys()
{
	if (!UserState)
		return;

	OtrlUserState userState = UserState->userState();
	otrl_privkey_read(userState, privateStoreFileName().toUtf8().data());
}