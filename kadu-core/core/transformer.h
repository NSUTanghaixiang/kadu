/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TRANSFORMER_H
#define TRANSFORMER_H

/**
 * @addtogroup Core
 * @{
 */

/**
 * @class Transformer
 * @short Interface for transforming instances of one type.
 * @param T type to transform
 *
 * Implementations of this interface are used to transform object - like updating data or removing some items from list.
 */
template <typename T>
class Transformer
{
public:
    typedef T object_type;

    virtual ~Transformer<T>()
    {
    }

    /**
     * @short Transform instance of class.
     * @param object object to transform
     * @return transformed object
     *
     * This method returns new object. Original one is untouched.
     */
    virtual T transform(const T &object) = 0;
};

/**
 * @}
 */

#endif   // TRANSFORMER_H
