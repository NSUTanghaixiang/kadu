/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SOCKET_NOTIFIERS_H
#define GADU_SOCKET_NOTIFIERS_H

#include <QtCore/QObject>

class QSocketNotifier;

#ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduSocketNotifiers : public QObject
{
	Q_OBJECT

	int Socket;
	QSocketNotifier *ReadNotifier;
	QSocketNotifier *WriteNotifier;

	void createSocketNotifiers();
	void deleteSocketNotifiers();
	void recreateSocketNotifiers();

private slots:
	void dataReceived();
	void dataSent();

protected:

	void setReadEnabled(bool enabled);
	void setWriteEnabled(bool enabled);

	void watchFor(int socket);

	void finished()
	{
		deleteSocketNotifiers();
	}

	virtual bool checkRead() = 0;
	virtual bool checkWrite() = 0;
	virtual void socketEvent() = 0;

public:
	GaduSocketNotifiers(int socket, QObject *parent = 0);
	virtual ~GaduSocketNotifiers();

	void stop();

};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_SOCKET_NOTIFIERS_H
