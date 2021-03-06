/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style/engine/chat-style-renderer.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatStyleManager;
class KaduChatSyntax;
class Parser;

class KaduStyleRenderer : public ChatStyleRenderer
{
    Q_OBJECT

public:
    explicit KaduStyleRenderer(
        ChatStyleRendererConfiguration configuration, std::shared_ptr<KaduChatSyntax> style, QObject *parent = nullptr);
    virtual ~KaduStyleRenderer();

    virtual void clearMessages() override;
    virtual void appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo) override;
    virtual void removeFirstMessage() override;
    virtual void displayMessageStatus(const QString &id, MessageStatus) override;
    virtual void displayChatState(ChatState, const QString &, const QString &) override;
    virtual void displayChatImage(const ChatImage &chatImage, const QString &fileName) override;

private:
    QPointer<ChatStyleManager> m_chatStyleManager;
    QPointer<Parser> m_parser;

    std::shared_ptr<KaduChatSyntax> m_style;

    QString formatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo);

private slots:
    INJEQT_SET void setChatStyleManager(ChatStyleManager *chatStyleManager);
    INJEQT_SET void setParser(Parser *parser);
    INJEQT_INIT void init();
};
