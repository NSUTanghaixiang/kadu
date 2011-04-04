/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
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

#include <QtGui/QMenu>

#include "gui/actions/action.h"
#include "gui/actions/action-description.h"

#include "custom-input-menu-manager.h"

CustomInputMenuManager * CustomInputMenuManager::Instance = 0;

CustomInputMenuManager * CustomInputMenuManager::instance()
{
	if (!Instance)
		Instance = new CustomInputMenuManager();

	return Instance;
}

CustomInputMenuManager::CustomInputMenuManager() :
		InputContextMenuSorted(true)
{
}

void CustomInputMenuManager::sortInputContextMenu()
{
	if (!InputContextMenuSorted)
	{
		qSort(InputContextMenu);
		InputContextMenuSorted = true;
	}
}

void CustomInputMenuManager::addActionDescription(ActionDescription *actionDescription, CustomInputMenuItem::CustomInputMenuCategory category, int priority)
{
	InputContextMenu.append(CustomInputMenuItem(actionDescription, category, priority));
	InputContextMenuSorted = false;
}

void CustomInputMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	QList<CustomInputMenuItem>::iterator i = InputContextMenu.begin();

	while (i != InputContextMenu.end())
	{
		if ((*i).actionDescription() == actionDescription)
			i = InputContextMenu.erase(i);
		else
			++i;
	}
}

QMenu * CustomInputMenuManager::menu(QWidget *parent)
{
	QMenu *menu = new QMenu(parent);

	sortInputContextMenu();
	QList<CustomInputMenuItem>::const_iterator i = InputContextMenu.constBegin();

	while (i != InputContextMenu.constEnd())
	{
		Action *action = i->actionDescription()->createAction(0, parent);

		menu->addAction(action);
		action->checkState();

		++i;
	}

	return menu;
}
