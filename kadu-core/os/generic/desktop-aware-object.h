/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DESKTOP_AWARE_OBJECT
#define DESKTOP_AWARE_OBJECT

#include "exports.h"

class QWidget;

class DesktopAwareObjectHelper;

/**
 * @class DesktopAwareObject
 * @short Class that gets informed about desktop resolution and screen number changes.
 *
 * Each class that inherits from DesktopAwareObject has to implement method
 * resolutionChanged that is called automatically when the resolution changes.
 *
 * The default implementation checks if the current object is a toplevel QWidget
 * and, if so, moves it so that it is fully visible on one of the remaining screens.
 */
class KADUAPI DesktopAwareObject
{
    static DesktopAwareObjectHelper *Helper;

    QWidget *Widget;

private:
    static QList<DesktopAwareObject *> Objects;

protected:
    /**
     * @short Method is called when resolution changes
     *
     * Method is called when resolution changes.
     * The default implementation moves the window to the closest available desktop.
     */
    virtual void desktopModified();

public:
    DesktopAwareObject(QWidget *widget);
    virtual ~DesktopAwareObject();

    static void notifyDesktopModified();
};

#endif   // DESKTOP_AWARE_OBJECT
