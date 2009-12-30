/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "status/status.h"

#include "exports.h"

class KaduWindow;
class Message;
class UserStatusChanger;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	Buddy Myself;
	KaduWindow *Window;
	bool ShowMainWindowOnStart; // TODO: 0.7.1, it is a hack

//	Status NextStatus;
	UserStatusChanger *StatusChanger;

	Core();
	virtual ~Core();

	void createDefaultConfiguration();
	void createAllDefaultToolbars();

	void init();
	void loadDefaultStatus();

	void storeConfiguration();

private slots:
	void statusChanged();
	void statusChanged(StatusContainer *, Status);

	void deleteOldConfigurationFiles();
	void kaduWindowDestroyed();

protected:
	virtual void accountAdded(Account account);
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();

public:
	static Core * instance();

	Buddy myself() { return Myself; }
	Status status(StatusContainer *container);

	void createGui();
	void setShowMainWindowOnStart(bool show);
	KaduWindow * kaduWindow();

	void setIcon(const QPixmap &icon);

public slots:
	void setStatus(StatusContainer *container, const Status &status);
	void setOnline(StatusContainer *container, const QString &description = QString::null);
	void setAway(StatusContainer *container, const QString &description = QString::null);
	void setInvisible(StatusContainer *container, const QString &description = QString::null);
	void setOffline(StatusContainer *container, const QString &description = QString::null);

	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	void messageReceived(const Message &message);
	void messageSent(const Message &message);

	// TODO: remove
	void settingMainIconBlocked(bool &blocked);

	//TODO:
	void searchingForTrayPosition(QPoint &);

	void mainIconChanged(const QIcon &);


};

#endif // CORE_H
