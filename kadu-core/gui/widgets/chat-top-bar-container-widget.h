/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_TOP_BAR_CONTAINER_WIDGET_H
#define CHAT_TOP_BAR_CONTAINER_WIDGET_H

#include <QtGui/QWidget>

#include "chat/chat.h"

class QVBoxLayout;

class ChatTopBarWidgetFactory;
class ChatTopBarWidgetFactoryRepository;

class ChatTopBarContainerWidget : public QWidget
{
	Q_OBJECT

	ChatTopBarWidgetFactoryRepository *MyChatTopBarWidgetFactoryRepository;
	Chat MyChat;
	QVBoxLayout *Layout;
	QMap<ChatTopBarWidgetFactory *, QWidget *> TopBarWidgets;

	void createGui();

private slots:
	void factoryRegistered(ChatTopBarWidgetFactory *factory);
	void factoryUnregistered(ChatTopBarWidgetFactory *factory);

public:
	explicit ChatTopBarContainerWidget(ChatTopBarWidgetFactoryRepository *chatTopBarWidgetFactoryRepository, const Chat &chat, QWidget *parent = 0);
	virtual ~ChatTopBarContainerWidget();

};

#endif // CHAT_TOP_BAR_CONTAINER_WIDGET_H
