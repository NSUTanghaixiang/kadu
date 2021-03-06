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

#include "screenshot-module.h"

#include "configuration/gui/screenshot-configuration-ui-handler.h"
#include "configuration/screen-shot-configuration.h"
#include "gui/actions/screenshot-actions.h"
#include "screenshot-notification-service.h"
#include "screenshot-plugin-object.h"

ScreenshotModule::ScreenshotModule()
{
    add_type<ScreenshotActions>();
    add_type<ScreenShotConfigurationUiHandler>();
    add_type<ScreenShotConfiguration>();
    add_type<ScreenshotNotificationService>();
    add_type<ScreenshotPluginObject>();
}

ScreenshotModule::~ScreenshotModule()
{
}
