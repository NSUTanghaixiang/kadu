/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "icons/kadu-icon.h"
#include "status/status.h"

#include "exports.h"

namespace QCA
{
	class Initializer;
}

class KaduIcon;
class KaduWindow;
class Message;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	Buddy Myself;
	KaduWindow *Window;
	bool IsClosing;
	bool ShowMainWindowOnStart; // TODO: 0.11.0, it is a hack

	// NOTE: Kadu core itself doesn't use QCA, but important plugins do. And QCA lib
	// isn't very well suited to be unloaded, so we just link to it in core and initialize
	// here.
	QCA::Initializer *QcaInit;

	Core();
	virtual ~Core();

	void import_0_6_5_configuration();
	void createDefaultConfiguration();
	void createAllDefaultToolbars();

	void init();
	void loadDefaultStatus();

	void storeConfiguration();

private slots:
	void statusUpdated();

	void deleteOldConfigurationFiles();
	void kaduWindowDestroyed();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();

public:
	static Core * instance();

	static QString name();
	static QString version();
	static QString nameWithVersion();

	bool isClosing() { return IsClosing; }
	Buddy myself() { return Myself; }

	void createGui();
	void setShowMainWindowOnStart(bool show);
	void showMainWindow();
	KaduWindow * kaduWindow();

	void initialized();
	void setIcon(const KaduIcon &icon);

public slots:
	void receivedSignal(const QString &signal);

	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	void messageSent(const Message &message);

	// TODO: remove
	void settingMainIconBlocked(bool &blocked);

	//TODO:
	void searchingForTrayPosition(QPoint &);

	void mainIconChanged(const KaduIcon &);

};

#endif // CORE_H
