/*
 * %kadu copyright begin%
 * Copyright 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_ICON_H
#define KADU_ICON_H

#include <QtCore/QString>

#include "exports.h"

class QIcon;

/**
 * @addtogroup Icons
 * @{
 */

/**
 * @class KaduIcon
 * @short Object that stores Kadu-specific information about icon and allows to retrieve the icon itself.
 *
 * Objects of this class store Kadu-specific information about icons and allow to retrieve the icons
 * themselves, as well as their paths and sizes and also full system paths and WebKit-friendly paths.
 * KaduIcon objects can be bound to specific theme or to current one (default behavior). Use setThemePath()
 * method to set specific theme.
 *
 * Creating objects of this class is relatively cheap as no data is verified or additionally retrieved
 * in the constructor.
 *
 * It is recommended for use everywhere using icons provided by IconsManager. It saves direct calls to
 * IconsManager and helps managing icons, especially when one wants to store for example both the icon
 * path and QIcon object.
 */
class KADUAPI KaduIcon
{
    QString ThemePath;
    QString Path;
    QString IconSize;

public:
    /**
     * @short Creates null KaduIcon object.
     *
     * Creates null KaduIcon object. isNull() will return true.
     */
    KaduIcon()
    {
    }

    /**
     * @short Creates KaduIcon object.
     * @param path path to this icon
     * @param size requested size as string formatted as "WIDTHxHEIGHT"
     *
     * Creates KaduIcon object from given path and optionally size and name.
     */
    explicit KaduIcon(const QString &path, const QString &size = QString()) : Path(path), IconSize(size)
    {
    }

    /**
     * @short Returns true if this object is null.
     * @return true if this object is null, false otherwise
     *
     * Returns true if this object is null. Every object with empty path() is considered null.
     */
    bool isNull() const
    {
        return path().isEmpty();
    }

    /**
     * @short Return theme path of this icon.
     * @return theme path of this icon
     *
     * If this method returns empty string, then current theme path should be used.
     */
    const QString &themePath() const
    {
        return ThemePath;
    }

    /**
     * @short Set theme path of this icon.
     * @param themePath new theme path of this icon
     *
     * Empty value of themePath means that current theme path should be used for this icon.
     */
    void setThemePath(const QString &themePath)
    {
        ThemePath = themePath;
    }

    /**
     * @short Returns path to this icon.
     * @return path to this icon
     *
     * Returns path to this icon.
     */
    const QString &path() const
    {
        return Path;
    }

    /**
     * @short Sets path to this icon and resets all other data.
     * @param path path to this icon
     *
     * Sets path to this icon and resets its size.
     */
    void setPath(const QString &path)
    {
        Path = path;
    }

    /**
     * @short Returns requested size of file pointed by fullPath() and webKitPath(), if set.
     * @return size as string formatted like "WIDTHxHEIGHT"
     *
     * Returns requested size of file pointed by fullPath() and webKitPath(), if set.
     */
    const QString &size() const
    {
        return IconSize;
    }

    /**
     * @short Sets the requested size of icon file.
     * @param size requested size as string formatted as "WIDTHxHEIGHT"
     *
     * Sets the requested size of icon file returned by fullPath() and webKitPath() methods.
     */
    void setSize(const QString &size)
    {
        IconSize = size;
    }
};

/**
 * @}
 */

#endif   // KADU_ICON_H
