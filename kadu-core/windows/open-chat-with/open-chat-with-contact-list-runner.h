/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "open-chat-with-runner.h"

class BuddyManager;

class KADUAPI OpenChatWithContactListRunner : public QObject, public OpenChatWithRunner
{
    Q_OBJECT

public:
    explicit OpenChatWithContactListRunner(QObject *parent = nullptr);
    virtual ~OpenChatWithContactListRunner();

    virtual BuddyList matchingContacts(const QString &query);

private:
    QPointer<BuddyManager> m_buddyManager;

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
};
