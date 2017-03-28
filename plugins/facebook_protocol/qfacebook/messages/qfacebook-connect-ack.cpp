/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Based on bitlbee-facebook by James Geboski (jgeboski@gmail.com) and
 * dequis <dx@dxzone.com.ar>.
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

#include "qfacebook-connect-ack.h"

#include "qmqtt/qmqtt-message.h"
#include "qmqtt/qmqtt-reader.h"
#include "qmqtt/qmqtt-types.h"
#include "qmqtt/qmqtt-writer.h"

QMqttMessage QFacebookConnectAck::encode() const
{
    auto mqttWriter = QMqttWriter{};

    mqttWriter.write(unknown);
    mqttWriter.write(errorCode);

    return QMqttMessage{static_cast<uint8_t>(messageType()), uint8_t{0}, mqttWriter.result()};
}

QFacebookConnectAck QFacebookConnectAck::decode(const QMqttMessage &message)
{
    auto reader = QMqttReader{message.content};
    auto result = QFacebookConnectAck{};
    result.unknown = reader.readUint8();
    result.errorCode = reader.readUint8();
    return result;
}
