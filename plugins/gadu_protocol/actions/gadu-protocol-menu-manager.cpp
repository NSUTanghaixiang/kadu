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

#include "gadu-protocol-menu-manager.h"

#include "actions/gadu-send-gift-action.h"

#include "misc/memory.h"

GaduProtocolMenuManager::GaduProtocolMenuManager(QObject *parent) :
		QObject{parent}
{
	m_sendGiftAction = make_unique<GaduSendGiftAction>();
	m_actions << m_sendGiftAction.get();
}

GaduProtocolMenuManager::~GaduProtocolMenuManager()
{
}

const QString GaduProtocolMenuManager::protocolName() const
{
	return QString{"gadu"};
}

const QList<ActionDescription *> & GaduProtocolMenuManager::protocolActions() const
{
	return m_actions;
}

#include "moc_gadu-protocol-menu-manager.cpp"
