/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "widgets/chat-top-bar-widget-factory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatTypeManager;
class PluginInjectedFactory;
class OtrChatTopBarWidget;
class OtrPeerIdentityVerificationWindowRepository;
class OtrSessionService;
class OtrTrustLevelService;

class OtrChatTopBarWidgetFactory : public QObject, public ChatTopBarWidgetFactory
{
    Q_OBJECT

public:
    Q_INVOKABLE OtrChatTopBarWidgetFactory();
    virtual ~OtrChatTopBarWidgetFactory();

    virtual QWidget *createWidget(const Chat &chat, QWidget *parent);

private:
    QPointer<ChatTypeManager> m_chatTypeManager;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<OtrPeerIdentityVerificationWindowRepository> PeerIdentityVerificationWindowRepository;
    QPointer<OtrSessionService> SessionService;
    QPointer<OtrTrustLevelService> TrustLevelService;
    QList<OtrChatTopBarWidget *> Widgets;

private slots:
    INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setPeerIdentityVerificationWindowRepository(
        OtrPeerIdentityVerificationWindowRepository *peerIdentityVerificationWindowRepository);
    INJEQT_SET void setSessionService(OtrSessionService *sessionService);
    INJEQT_SET void setTrustLevelService(OtrTrustLevelService *trustLevelService);

    void widgetDestroyed(QObject *widget);
};
