/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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


#ifndef SHORTCUT_DISPATCHER_H
#define SHORTCUT_DISPATCHER_H

#include <QtCore/QString>

class QKeyEvent;

class ActionContext;
class Shortcut;
class ShortcutProvider;

class ShortcutDispatcher
{
	static ShortcutDispatcher *Instance;
	
protected:
	ShortcutDispatcher();
	virtual ~ShortcutDispatcher();

public:
	static ShortcutDispatcher * instance();

	virtual bool registerShortcut(const Shortcut &shortcut) = 0;
	virtual void unregisterShortcut(const QString &name, ShortcutProvider *provider) = 0;

	virtual bool registerShortcutProvider(ShortcutProvider *provider) = 0;
	virtual void unregisterShortcutProvider(ShortcutProvider *provider) = 0;

	virtual bool handleKeyEvent(ActionContext *actionSource, QKeyEvent *e) = 0;
};

#endif // SHORTCUT_DISPATCHER_H