/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "idle.h"

#include "idle-plugin.h"

IdlePlugin::~IdlePlugin()
{
}

bool IdlePlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	m_idle = make_owned<Idle>(this);
	return true;
}

void IdlePlugin::done()
{
	m_idle.reset();
}

Idle * IdlePlugin::idle() const
{
	return m_idle.get();
}

Q_EXPORT_PLUGIN2(idle, IdlePlugin)

#include "moc_idle-plugin.cpp"
