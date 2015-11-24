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

#pragma once

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class GaduDriveSessionToken;
class GaduDriveSendTicket;

class QIODevice;
class QNetworkAccessManager;
class QNetworkReply;

class GaduDrivePutTransfer : public QObject
{
	Q_OBJECT

public:
	explicit GaduDrivePutTransfer(GaduDriveSessionToken sessionToken, GaduDriveSendTicket ticket, QString fileName, QIODevice *source,
		QNetworkAccessManager *networkAccessManager, QObject *parent = nullptr);
	virtual ~GaduDrivePutTransfer();

signals:
	void finished();

private:
	QPointer<QNetworkReply> m_reply;

private slots:
	void requestFinished();

};