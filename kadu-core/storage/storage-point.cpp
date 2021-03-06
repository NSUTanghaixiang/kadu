/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "storage/storage-point.h"

/**
 * @short Constructs StoragePoint.
 * @arg storage pointer to XML storage file
 * @arg point DOM Node - parent of all data stored by this class
 *
 * Constructs StoragePoint from given XML file and DOM node.
 */
StoragePoint::StoragePoint(ConfigurationApi *storage, QDomElement point) : Storage(storage), Point(point)
{
}

/**
 * @short Returns pointer to current XML storage file.
 *
 * Returns pointer to current XML storage file.
 */
ConfigurationApi *StoragePoint::storage() const
{
    return Storage;
}

/**
 * @short Returns current parent DOM Node for this object.
 *
 * Returns current parent DOM Node for this object.
 */
QDomElement StoragePoint::point() const
{
    return Point;
}

void StoragePoint::storeValue(const QString &name, const QVariant value)
{
    Storage->createTextNode(Point, name, value.toString());
}

void StoragePoint::storeAttribute(const QString &name, const QVariant value)
{
    Point.setAttribute(name, value.toString());
}

void StoragePoint::removeValue(const QString &name)
{
    Storage->removeNode(Point, name);
}

void StoragePoint::removeAttribute(const QString &name)
{
    Point.removeAttribute(name);
}
