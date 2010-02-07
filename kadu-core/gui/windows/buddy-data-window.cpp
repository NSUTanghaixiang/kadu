/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2002, 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2004, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2004 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QTimer>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QIntValidator>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QHostInfo>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"

#include "contacts/contact.h"
#include "gui/widgets/buddy-general-configuration-widget.h"
#include "gui/widgets/buddy-groups-configuration-widget.h"
#include "gui/widgets/buddy-options-configuration-widget.h"
#include "gui/widgets/buddy-personal-info-configuration-widget.h"
#include "gui/widgets/contact-widget.h"
#include "gui/windows/buddy-data-manager.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "buddy-data-window.h"

BuddyDataWindow::BuddyDataWindow(Buddy buddy, QWidget *parent) :
		QWidget(parent, Qt::Dialog), MyBuddy(buddy)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Merged Contact Properties - %1").arg(MyBuddy.display()));

	createGui();
	updateButtons();

	loadWindowGeometry(this, "General", "ManageUsersDialogGeometry", 0, 50, 425, 500);
}

BuddyDataWindow::~BuddyDataWindow()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ManageUsersDialogGeometry");
	kdebugf2();
}

void BuddyDataWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	createTabs(layout);
	createButtons(layout);
}

void BuddyDataWindow::createTabs(QLayout *layout)
{
	QTabWidget *tabWidget = new QTabWidget(this);

	createGeneralTab(tabWidget);
	createGroupsTab(tabWidget);
	createPersonalInfoTab(tabWidget);
	createOptionsTab(tabWidget);
	layout->addWidget(tabWidget);
}

void BuddyDataWindow::createGeneralTab(QTabWidget *tabWidget)
{
	ContactTab = new BuddyGeneralConfigurationWidget(MyBuddy, this);
	connect(ContactTab, SIGNAL(validChanged()), this, SLOT(updateButtons()));

	tabWidget->addTab(ContactTab, tr("General"));
}

void BuddyDataWindow::createGroupsTab(QTabWidget *tabWidget)
{
	GroupsTab = new BuddyGroupsConfigurationWidget(MyBuddy, this);
	tabWidget->addTab(GroupsTab, tr("Groups"));
}

void BuddyDataWindow::createPersonalInfoTab(QTabWidget *tabWidget)
{
	PersonalInfoTab = new BuddyPersonalInfoConfigurationWidget(MyBuddy, this);
	tabWidget->addTab(PersonalInfoTab, tr("Personal Information"));
}

void BuddyDataWindow::createOptionsTab(QTabWidget *tabWidget)
{
	OptionsTab = new BuddyOptionsConfigurationWidget(MyBuddy, this);
	tabWidget->addTab(OptionsTab, tr("Options"));
}

void BuddyDataWindow::createButtons(QLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	OkButton = new QPushButton(IconsManager::instance()->iconByName("OkWindowButton"), tr("OK"), this);
	buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
	ApplyButton = new QPushButton(IconsManager::instance()->iconByName("OkWindowButton"), tr("Apply"), this);
	buttons->addButton(ApplyButton, QDialogButtonBox::ApplyRole);

	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->iconByName("CloseWindowButton"), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(updateBuddyAndClose()));
	connect(ApplyButton, SIGNAL(clicked(bool)), this, SLOT(updateBuddy()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(buttons);
}

void BuddyDataWindow::updateBuddy()
{
	if (isValid())
	{
		MyBuddy.blockUpdatedSignal();

		ContactTab->save();
		GroupsTab->save();
		OptionsTab->save();

		MyBuddy.unblockUpdatedSignal();
	}
}

void BuddyDataWindow::updateBuddyAndClose()
{
	if (isValid())
	{
		updateBuddy();
		close();
	}
}

void BuddyDataWindow::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

bool BuddyDataWindow::isValid()
{
	return ContactTab->isValid();
}

void BuddyDataWindow::updateButtons()
{
	bool valid = isValid();
	OkButton->setEnabled(valid);
	ApplyButton->setEnabled(valid);
}
