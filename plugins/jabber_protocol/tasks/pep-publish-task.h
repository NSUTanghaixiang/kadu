/*
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef PEP_PUBLISH_TASK_H
#define PEP_PUBLISH_TASK_H

#include <QtXml/QDomElement>

#include "iris/xmpp_pubsubitem.h"
#include "iris/xmpp_task.h"
#include "services/jabber-pep-service.h"

class PEPPublishTask : public XMPP::Task
{
	Q_OBJECT

	QDomElement iq_;
	QString node_;
	XMPP::PubSubItem item_;

public:
	PEPPublishTask(Task *parent, const QString &node, const XMPP::PubSubItem &it, JabberPepService::Access access);
	virtual ~PEPPublishTask();

	bool take(const QDomElement &x);
	void onGo();

	const XMPP::PubSubItem & item() const;
	const QString & node() const;

};

#endif // PEP_PUBLISH_TASK_H
