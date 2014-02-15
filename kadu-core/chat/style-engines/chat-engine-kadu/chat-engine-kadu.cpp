/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat-engine-kadu.h"

#include "chat/chat-styles-manager.h"
#include "core/core.h"
#include "chat/style-engines/chat-engine-kadu/kadu-chat-messages-renderer.h"
#include "gui/widgets/kadu-web-view.h"
#include "gui/widgets/preview.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/memory.h"
#include "misc/syntax-list.h"
#include "parser/parser.h"

#include <QtCore/QFileInfo>

KaduChatStyleEngine::KaduChatStyleEngine(QObject *parent) :
		QObject(parent)
{
	EngineName = "Kadu";
	syntaxList = QSharedPointer<SyntaxList>(new SyntaxList("chat"));
}

KaduChatStyleEngine::~KaduChatStyleEngine()
{
}

QString KaduChatStyleEngine::isStyleValid(QString stylePath)
{
	QFileInfo fi;
	fi.setFile(stylePath);
	return fi.suffix() == "syntax" ? fi.completeBaseName() : QString();
}

std::unique_ptr<ChatMessagesRenderer> KaduChatStyleEngine::createRenderer(const QString &styleName, const QString &variantName)
{
	Q_UNUSED(variantName)

	QString chatSyntax = SyntaxList::readSyntax("chat", styleName,
		"<p style=\"background-color: #{backgroundColor};\">#{separator}"
		  "<font color=\"#{fontColor}\"><kadu:header><b><font color=\"#{nickColor}\">%a</font> :: "
			"#{receivedDate}[ / S #{sentDate}]</b><br /></kadu:header>"
		"#{message}</font></p>"
	);

	CurrentStyleName = styleName;
	return make_unique<KaduChatMessagesRenderer>(KaduChatSyntax{chatSyntax});
}

void KaduChatStyleEngine::prepareStylePreview(Preview *preview, QString styleName, QString variantName)
{
	Q_UNUSED(variantName)

	KaduChatSyntax syntax(SyntaxList::readSyntax("chat", styleName, QString()));

	QString text = Parser::parse(syntax.top(), Talkable(), true);

	int count = preview->messages().count();
	if (count)
	{
		auto previous = Message::null;
		for (int i = 0; i < count; i++)
		{
			auto message = preview->messages().at(i);
			auto info = Core::instance()->messageRenderInfoFactory()->messageRenderInfo(previous, message);
			previous = message;

			text += Parser::parse(syntax.withHeader(), Talkable(message.messageSender()), &info);
		}
	}
	preview->webView()->setHtml(QString("<html><head><style type='text/css'>%1</style></head><body>%2</body>").arg(ChatStylesManager::instance()->mainStyle(), text));
}

#include "moc_chat-engine-kadu.cpp"
