/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "firewall-plugin-object.h"

#include "firewall-configuration-ui-handler.h"
#include "firewall-message-filter.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "message/message-filter-service.h"
#include "misc/paths-provider.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "status/status-changer-manager.h"

FirewallPluginObject::FirewallPluginObject(QObject *parent) :
		QObject{parent}
{
}

FirewallPluginObject::~FirewallPluginObject()
{
}

void FirewallPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void FirewallPluginObject::setFirewallConfigurationUiHandler(FirewallConfigurationUiHandler *firewallConfigurationUiHandler)
{
	m_firewallConfigurationUiHandler = firewallConfigurationUiHandler;
}

void FirewallPluginObject::setFirewallMessageFilter(FirewallMessageFilter *firewallMessageFilter)
{
	m_firewallMessageFilter = firewallMessageFilter;
}

void FirewallPluginObject::setMessageFilterService(MessageFilterService *messageFilterService)
{
	m_messageFilterService = messageFilterService;
}

void FirewallPluginObject::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void FirewallPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void FirewallPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/firewall.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_firewallConfigurationUiHandler);
	m_messageFilterService->registerMessageFilter(m_firewallMessageFilter);
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{"firewallNotification", QT_TRANSLATE_NOOP("@default", "Message was firewalled")});
}

void FirewallPluginObject::done()
{
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{"firewallNotification", QT_TRANSLATE_NOOP("@default", "Message was firewalled")});
	m_messageFilterService->unregisterMessageFilter(m_firewallMessageFilter);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_firewallConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/firewall.ui"));
}

#include "moc_firewall-plugin-object.cpp"
