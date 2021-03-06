/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "windows/main-configuration-window.h"

#include <QtCore/QPointer>

class Configuration;

class MPDConfig : public ConfigurationAwareObject
{
    QPointer<Configuration> m_configuration;

    QString Host;
    QString Port;
    QString Timeout;

    void createDefaultConfiguration();

protected:
    virtual void configurationUpdated();

public:
    MPDConfig();
    MPDConfig(Configuration *configuration);
    virtual ~MPDConfig();

    const QString &host() const
    {
        return Host;
    }
    const QString &port() const
    {
        return Port;
    }
    const QString &timeout() const
    {
        return Timeout;
    }
};
