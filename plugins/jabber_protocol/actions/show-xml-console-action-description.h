/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SHOW_XML_CONSOLE_ACTION_DESCRIPTION_H
#define SHOW_XML_CONSOLE_ACTION_DESCRIPTION_H

#include "gui/actions/action-description.h"

class ShowXmlConsoleActionDescription : public ActionDescription
{
	Q_OBJECT

private slots:
	void insertMenuActionDescription();
	void updateShowXmlConsoleMenu();
	void menuActionTriggered(QAction *action);

protected:
	virtual void actionInstanceCreated(Action *action);
	virtual void actionTriggered(QAction *sender, bool toggled);

public:
	explicit ShowXmlConsoleActionDescription(QObject *parent);
	virtual ~ShowXmlConsoleActionDescription();

};

#endif // SHOW_XML_CONSOLE_ACTION_DESCRIPTION_H
