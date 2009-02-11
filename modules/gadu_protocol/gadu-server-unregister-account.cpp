/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"

#include "gadu-pubdir-socket-notifiers.h"

#include "gadu-server-unregister-account.h"

GaduServerUnregisterAccount::GaduServerUnregisterAccount(TokenReader *reader, UinType uin, const QString &password)
	: GaduServerConnector(reader), Uin(uin), Password(password)
{
}

void GaduServerUnregisterAccount::performAction(const QString &tokenId, const QString &tokenValue)
{
	struct gg_http* h = gg_unregister3(Uin, unicode2cp(Password).data(), unicode2cp(tokenId).data(),
		unicode2cp(tokenValue).data(), 1);
	if (h)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers(h, this);
		connect(sn, SIGNAL(done(bool, struct gg_http *)),
			this, SLOT(done(bool, struct gg_http *)));
		sn->start();
	}
	else
		finished(false);
}

void GaduServerUnregisterAccount::done(bool ok, struct gg_http *h)
{
	finished(ok);
}
