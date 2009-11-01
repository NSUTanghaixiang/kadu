/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

#include "buddies/buddy.h"

#include "exports.h"

class Chat;
class MessageData;

class KADUAPI Message : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<MessageData> Data;

public:
	enum Status
	{
		StatusUnknown,
		StatusReceived,
		StatusSent,
		StatusDelivered,
		StatusWontDeliver
	};
	enum Type
	{
		TypeUnknown,
		TypeReceived,
		TypeSent,
		TypeSystem
	};

private slots:
	void statusChanged(Message::Status);

public:
	Message(Chat *chat = 0, Type type = TypeUnknown, Buddy sender = Buddy::null);
	Message(const Message &copy);
	virtual ~Message();

	void operator = (const Message &copy);

	Chat * chat() const;
	Message & setChat(Chat *chat);

	Buddy sender() const;
	Message & setSender(Buddy sender);

	QString content() const;
	Message & setContent(const QString &content);

	QDateTime receiveDate() const;
	Message & setReceiveDate(QDateTime receiveDate);

	QDateTime sendDate() const;
	Message & setSendDate(QDateTime sendDate);

	Message::Status status() const;
	Message & setStatus(Message::Status status);

	Message::Type type() const;
	Message & setType(Message::Type type);

	int id() const;
	Message & setId(int id);

signals:
	void statusChanged(Message, Message::Status);

};

#endif // MESSAGE_H
