/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking_exports.h"

#include <QtCore/QObject>
#include <QtCore/QPoint>

class QMovie;
class KaduIcon;
class KStatusNotifierItem;

class DOCKINGAPI StatusNotifierItem final : public QObject
{
	Q_OBJECT

	KStatusNotifierItem *m_statusNotifierItem;
	QMovie *m_movie;
	QPoint m_lastPosition;

private slots:
	//void trayActivated(QSystemTrayIcon::ActivationReason reason);

	void movieUpdate();

public:
	explicit StatusNotifierItem(QObject *parent = nullptr);
	virtual ~StatusNotifierItem();

	void changeTrayIcon(const KaduIcon &icon);
	void changeTrayMovie(const QString &moviePath);
	void changeTrayTooltip(const QString &tooltip);
	QPoint trayPosition();

signals:
	void messageClicked();

};