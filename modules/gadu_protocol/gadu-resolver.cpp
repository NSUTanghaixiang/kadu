/*
 * %kadu copyright begin%
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>

#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <libgadu.h>

#include "debug.h"

#include "gadu-resolver.h"

GaduResolver *resolver = 0;

GaduResolver::GaduResolver(QObject *parent) :
		QObject(parent)
{
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(abort()));
	id = -1;
}

GaduResolver::~GaduResolver()
{
	if (timer->isActive())
		abort();
	delete timer;
}

void GaduResolver::abort()
{
	if (id < 0)
		return;

	if (timer->isActive())
		timer->stop();

	QHostInfo::abortHostLookup(id);
}

void GaduResolver::resolve(const QString &hostname)
{
	timer->start(10000);
	id = QHostInfo::lookupHost(hostname, this, SLOT(resolved(QHostInfo)));
}

void GaduResolver::resolved(const QHostInfo &host)
{
	struct in_addr addr;

	if (timer->isActive())
		timer->stop();

	if (host.error() == QHostInfo::NoError)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Resolved address to: %s\n", qPrintable(host.addresses().first().toString()));
		addr.s_addr = htonl(host.addresses().first().toIPv4Address());
	}
	else
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Error while resolving: %s\n", qPrintable(host.errorString()));
		addr.s_addr = INADDR_NONE;
	}

	if (write(data->wfd, &addr, sizeof(addr)) != sizeof(addr))
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Writing to pipe failed\n");
	}
}

void GaduResolver::setData(gadu_resolver_data *data)
{
	this->data = data;
}

gadu_resolver_data *GaduResolver::getData()
{
	return this->data;
}

void gadu_resolver_cleanup(void **priv_data, int force)
{
	Q_UNUSED(force)

	struct gadu_resolver_data *data = 0;

	if (resolver != NULL)
	{
		delete resolver;
		resolver = 0;
	}

	if (priv_data == NULL || *priv_data == 0)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "data is null. nothing to do\n");
		return;
	}

	data = (struct gadu_resolver_data *)*priv_data;
	*priv_data = NULL;

	if (data->wfd != -1)
	{
		close(data->wfd);
		data->wfd = -1;
	}
	free(data);
}

int gadu_resolver_start(int *fd, void **priv_data, const char *hostname)
{
	int pipes[2];
	struct gadu_resolver_data *data = 0;

	if (pipe(pipes) == -1)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unable to create pipes\n");
		return -1;
	}

	data = (struct gadu_resolver_data *)malloc(sizeof(struct gadu_resolver_data));
	if (data == NULL)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unable to allocate data\n");
		return -1;
	}

	data->rfd = pipes[0];
	data->wfd = pipes[1];

	resolver = new GaduResolver();
	resolver->setData(data);
	resolver->resolve(QString(hostname));

	*fd = pipes[0];
	*priv_data = data;

	return 0;
}
