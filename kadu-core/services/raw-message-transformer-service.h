/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef RAW_MESSAGE_TRANSFORMER_SERVICE_H
#define RAW_MESSAGE_TRANSFORMER_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"
#include "protocols/services/raw-message-transformer.h"
#include "services/configurable-transformer-service.h"

class KADUAPI RawMessageTransformerService : public QObject,
                                             public ConfigurableTransformerService<RawMessageTransformer>
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit RawMessageTransformerService(QObject *parent = nullptr);
    virtual ~RawMessageTransformerService();
};

#endif   // RAW_MESSAGE_TRANSFORMER_SERVICE_H
