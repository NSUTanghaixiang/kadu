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

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <set>

class ConfigurationManager;
class Configuration;
class PluginDependencyHandler;
class PluginStateService;
class StoragePoint;
class StoragePointFactory;

enum class PluginState;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginStateService
 * @short Stores and load plugin states from configuration file to PluginStateService.
 *
 * This service is used to load plugin states from configuration to PluginStateService
 * and to load current configuration from PluginStateService and store it in configuration
 * file.
 */
class PluginStateManager : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit PluginStateManager(QObject *parent = nullptr);
    virtual ~PluginStateManager();

    /**
     * @short Load plugin states from configuration file to PluginStateService.
     *
     * Uses PluginStateService as destination of data and StoragePointFactory to
     * access storage point of configuration.
     */
    void loadPluginStates();

    /**
     * @short Store plugin states from PluginStateService to configuration file.
     *
     * Uses PluginStateService as source of data and StoragePointFactory to
     * access storage point of configuration. In addition PluginManager is used
     * to get list of all currently installed plugin in case import from 0.9 configuration
     * format is required.
     */
    void storePluginStates();

private:
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<Configuration> m_configuration;
    QPointer<PluginDependencyHandler> m_pluginDependencyHandler;
    QPointer<PluginStateService> m_pluginStateService;
    QPointer<StoragePointFactory> m_storagePointFactory;

    QMap<QString, PluginState> loadPluginStates(StoragePoint *storagePoint, bool importedFrom09) const;

private slots:
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler);
    INJEQT_SET void setPluginStateService(PluginStateService *pluginStateService);
    INJEQT_SET void setStoragePointFactory(StoragePointFactory *storagePointFactory);

    void storePluginStatesAndFlush();
};

/**
 * @}
 */
