/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/injected-factory.h"
#include "network/proxy/network-proxy-manager.h"
#include "network/proxy/network-proxy-shared.h"
#include "storage/storage-point.h"

#include "network-proxy.h"

#include <QtNetwork/QNetworkProxy>

KaduSharedBaseClassImpl(NetworkProxy)

    NetworkProxy NetworkProxy::null;

NetworkProxy::NetworkProxy()
{
}

NetworkProxy::NetworkProxy(NetworkProxyShared *data) : SharedBase<NetworkProxyShared>(data)
{
}

NetworkProxy::NetworkProxy(QObject *data)
{
    NetworkProxyShared *shared = qobject_cast<NetworkProxyShared *>(data);
    if (shared)
        setData(shared);
}

NetworkProxy::NetworkProxy(const NetworkProxy &copy) : SharedBase<NetworkProxyShared>(copy)
{
}

NetworkProxy::~NetworkProxy()
{
}

KaduSharedBase_PropertyDefCRW(NetworkProxy, QString, type, Type, QString("http"))
    KaduSharedBase_PropertyDefCRW(NetworkProxy, QString, address, Address, QString())
        KaduSharedBase_PropertyDef(NetworkProxy, int, port, Port, 0)
            KaduSharedBase_PropertyDefCRW(NetworkProxy, QString, user, User, QString())
                KaduSharedBase_PropertyDefCRW(NetworkProxy, QString, password, Password, QString())
                    KaduSharedBase_PropertyDefCRW(NetworkProxy, QString, pollingUrl, PollingUrl, QString())
                        KaduSharedBase_PropertyReadDef(NetworkProxy, QString, displayName, DisplayName, QString())

                            QNetworkProxy toQNetworkProxy(const NetworkProxy &networkProxy)
{
    if (!networkProxy)
        return {};

    auto proxy = QNetworkProxy{};
    if (networkProxy.type() == "socks")
        proxy.setType(QNetworkProxy::Socks5Proxy);
    else
        proxy.setType(QNetworkProxy::HttpProxy);

    proxy.setHostName(networkProxy.address());
    proxy.setPort(networkProxy.port());
    proxy.setUser(networkProxy.user());
    proxy.setPassword(networkProxy.password());

    return proxy;
}
