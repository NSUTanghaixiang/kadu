/*
 * %kadu copyright begin%
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtWebKit/QWebFrame>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "configuration/chat-configuration-holder.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/webkit-messages-view.h"
#include "gui/widgets/preview.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "message/message-html-renderer-service.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/date-time.h"
#include "misc/kadu-paths.h"
#include "misc/misc.h"
#include <misc/memory.h>
#include "parser/parser.h"
#include "protocols/protocol-factory.h"

#include "adium-style.h"
#include "adium-time-formatter.h"

#include "chat-engine-adium.h"
#include "adium-chat-messages-renderer.h"

RefreshViewHack::RefreshViewHack(AdiumChatMessagesRenderer *engine, HtmlMessagesRenderer *renderer, QObject *parent) :
		QObject(parent), Engine(engine), Renderer(renderer)
{
	connect(Engine, SIGNAL(destroyed()), this, SLOT(cancel()));
	connect(Renderer, SIGNAL(destroyed()), this, SLOT(cancel()));
}

RefreshViewHack::~RefreshViewHack()
{
}

void RefreshViewHack::cancel()
{
	Engine = 0;
	Renderer = 0;

	deleteLater();
}

void RefreshViewHack::loadFinished()
{
	if (!Engine || !Renderer)
	{
		deleteLater();
		return;
	}

	emit finished(Renderer);

	Renderer->setLastMessage(Message::null);

	for (auto const &message : Renderer->messages())
		Engine->appendChatMessage(Renderer, message);

	deleteLater();
}

PreviewHack::PreviewHack(AdiumChatMessagesRenderer *engine, Preview *preview, const QString &baseHref, const QString &outgoingHtml,
                         const QString &incomingHtml, QObject *parent) :
		QObject(parent), Engine(engine), CurrentPreview(preview), BaseHref(baseHref), OutgoingHtml(outgoingHtml), IncomingHtml(incomingHtml)
{
	connect(Engine, SIGNAL(destroyed()), this, SLOT(cancel()));
	connect(CurrentPreview, SIGNAL(destroyed()), this, SLOT(cancel()));
}

PreviewHack::~PreviewHack()
{
}

void PreviewHack::cancel()
{
	Engine = 0;
	CurrentPreview = 0;

	deleteLater();
}

void PreviewHack::loadFinished()
{
	if (!Engine || !CurrentPreview)
		return;

	auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
	auto message = CurrentPreview->messages().at(0);
	auto info1 = messageRenderInfoFactory->messageRenderInfo(Message::null, message);

	QString outgoingHtml(replacedNewLine(Engine->replaceKeywords(BaseHref, OutgoingHtml, message, info1.nickColor()), QLatin1String(" ")));
	outgoingHtml.replace('\'', QLatin1String("\\'"));
	if (!message.id().isEmpty())
		outgoingHtml.prepend(QString("<span id=\"message_%1\">").arg(message.id()));
	else
		outgoingHtml.prepend("<span>");
	outgoingHtml.append("</span>");
	CurrentPreview->webView()->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + outgoingHtml + "\')");

	message = CurrentPreview->messages().at(1);
	auto info2 = messageRenderInfoFactory->messageRenderInfo(Message::null, message);

	QString incomingHtml(replacedNewLine(Engine->replaceKeywords(BaseHref, IncomingHtml, message, info2.nickColor()), QLatin1String(" ")));
	incomingHtml.replace('\'', QLatin1String("\\'"));
	if (!message.id().isEmpty())
		incomingHtml.prepend(QString("<span id=\"message_%1\">").arg(message.id()));
	else
		incomingHtml.prepend("<span>");
	incomingHtml.append("</span>");
	CurrentPreview->webView()->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + incomingHtml + "\')");

	/* in Qt 4.6.3 / WebKit there is a bug making the following call not working */
	/* according to: https://bugs.webkit.org/show_bug.cgi?id=35633 */
	/* the proper refreshing behaviour should occur once the bug is fixed */
	/* possible temporary solution: use QWebElements API to randomly change */
	/* URLs in the HTML/CSS content. */
	CurrentPreview->webView()->page()->triggerAction(QWebPage::ReloadAndBypassCache, false);

	deleteLater();
}


AdiumChatStyleEngine::AdiumChatStyleEngine(QObject *parent) :
		QObject(parent)
{
	// Load required javascript functions
	QFile file(KaduPaths::instance()->dataPath() + QLatin1String("scripts/chat-scripts.js"));
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		jsCode = file.readAll();
}

AdiumChatStyleEngine::~AdiumChatStyleEngine()
{
}

void AdiumChatStyleEngine::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	CurrentMessageHtmlRendererService = messageHtmlRendererService;
}

QString AdiumChatStyleEngine::isStyleValid(QString stylePath)
{
	return AdiumStyle::isStyleValid(stylePath) ? QDir(stylePath).dirName() : QString();
}

bool AdiumChatStyleEngine::styleUsesTransparencyByDefault(QString styleName)
{
	AdiumStyle style(styleName);
	return style.defaultBackgroundIsTransparent();
}

QString AdiumChatStyleEngine::defaultVariant(const QString &styleName)
{
	AdiumStyle style(styleName);
	return style.defaultVariant();
}

QString AdiumChatStyleEngine::currentStyleVariant()
{
	return CurrentStyle.currentVariant();
}

QStringList AdiumChatStyleEngine::styleVariants(QString styleName)
{
	QDir dir;
	QString styleBaseHref = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	if (!dir.exists(styleBaseHref))
		styleBaseHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList();
}

QString AdiumChatStyleEngine::preprocessStyleBaseHtml(AdiumStyle &style, const Chat &chat)
{
	QString styleBaseHtml = style.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, KaduPaths::webKitPath(style.baseHref()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(chat, style.baseHref(), style.footerHtml()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(chat, style.baseHref(), style.headerHtml()));

	if (style.usesCustomTemplateHtml() && style.styleViewVersion() < 3)
	{
		if (style.currentVariant() != style.defaultVariant())
			styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "Variants/" + style.currentVariant());
		else
			styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, style.mainHref());
	}
	else
	{
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (style.styleViewVersion() < 3 && style.defaultVariant() == style.currentVariant()) ? style.mainHref() : "Variants/" + style.currentVariant());
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (style.styleViewVersion() < 3) ? QString() : QString("@import url( \"" + style.mainHref() + "\" );"));
	}

	return styleBaseHtml;
}

std::unique_ptr<ChatMessagesRenderer> AdiumChatStyleEngine::createRenderer(const QString &styleName, const QString &variantName)
{
	CurrentStyle = AdiumStyle(styleName);
	CurrentStyle.setCurrentVariant(variantName);
	CurrentStyleName = styleName;

	m_renderer = make_unique<AdiumChatMessagesRenderer>(CurrentStyle);
	m_renderer.get()->setMessageHtmlRendererService(CurrentMessageHtmlRendererService);

	auto result = make_unique<AdiumChatMessagesRenderer>(CurrentStyle);
	result.get()->setMessageHtmlRendererService(CurrentMessageHtmlRendererService);
	return std::move(result);
}

void AdiumChatStyleEngine::prepareStylePreview(Preview *preview, QString styleName, QString variantName)
{
	AdiumStyle style(styleName);

	style.setCurrentVariant(variantName);
	if (preview->messages().count() != 2)
		return;

	auto message = preview->messages().at(0);

	QString styleBaseHtml = preprocessStyleBaseHtml(style, message.messageChat());

	PreviewHack *previousHack = CurrentPreviewHack.data();
	CurrentPreviewHack = new PreviewHack(m_renderer.get(), preview, style.baseHref(), style.outgoingHtml(), style.incomingHtml(), this);
	delete previousHack;

	// lets wait a while for all javascript to resolve and execute
	// we dont want to get to the party too early
	connect(preview->webView()->page()->mainFrame(), SIGNAL(loadFinished(bool)),
	        CurrentPreviewHack.data(), SLOT(loadFinished()),  Qt::QueuedConnection);

	preview->webView()->page()->mainFrame()->setHtml(styleBaseHtml);
	preview->webView()->page()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	preview->webView()->page()->mainFrame()->evaluateJavaScript("initStyle()");
}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatStyleEngine::replaceKeywords(const Chat &chat, const QString &styleHref, const QString &style)
{
	if (!chat)
		return QString();

	QString result = style;

	//TODO: get Chat name (contacts' nicks?)
	//Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
	int contactsCount = chat.contacts().count();

	QString chatName;
	if (!chat.display().isEmpty())
		chatName = chat.display();
	else if (contactsCount > 1)
		chatName = tr("Conference [%1]").arg(contactsCount);
	else
		chatName = chat.name();

	result.replace(QString("%chatName%"), chatName);

	// Replace %sourceName%
	result.replace(QString("%sourceName%"), chat.chatAccount().accountIdentity().name());
	// Replace %destinationName%
	result.replace(QString("%destinationName%"), chat.name());
	// For %timeOpened%, display the date and time. TODO: get real time
	result.replace(QString("%timeOpened%"), printDateTime(QDateTime::currentDateTime()));

	//TODO 0.10.0: get real time!!!
	QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos=timeRegExp.indexIn(result, pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), AdiumTimeFormatter::convertTimeDate(timeRegExp.cap(1), QDateTime::currentDateTime()));

	QString photoIncoming;
	QString photoOutgoing;

	int contactsSize = chat.contacts().size();
	if (contactsSize == 1)
	{
		const Avatar &avatar = chat.contacts().toContact().avatar(true);
		if (!avatar.isEmpty())
			photoIncoming = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoIncoming = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}
	else
		photoIncoming = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));

	const Avatar &avatar = chat.chatAccount().accountContact().avatar(true);
	if (!avatar.isEmpty())
		photoOutgoing = KaduPaths::webKitPath(avatar.smallFilePath());
	else
		photoOutgoing = KaduPaths::webKitPath(styleHref + QLatin1String("Outgoing/buddy_icon.png"));

	result.replace(QString("%incomingIconPath%"), photoIncoming);
	result.replace(QString("%outgoingIconPath%"), photoOutgoing);

	return result;
}

QString AdiumChatStyleEngine::replaceKeywords(const QString &styleHref, const QString &source, const Message &message, const QString &nickColor)
{
	QString result = source;

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), message.messageSender().display(true));
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), message.messageSender().id());
	// Replace service name (protocol name)
	if (message.messageChat().chatAccount().protocolHandler() && message.messageChat().chatAccount().protocolHandler()->protocolFactory())
	{
		result.replace(QString("%service%"), message.messageChat().chatAccount().protocolHandler()->protocolFactory()->displayName());
		// Replace protocolIcon (sender statusIcon). TODO:
		result.replace(QString("%senderStatusIcon%"), message.messageChat().chatAccount().protocolHandler()->protocolFactory()->icon().fullPath());
	}
	else
	{
		result.replace(QString("%service%"), message.messageChat().chatAccount().accountIdentity().name());
		result.remove("%senderStatusIcon%");
	}

	// Replace time
	QDateTime time = message.sendDate().isNull() ? message.receiveDate(): message.sendDate();
	result.replace(QString("%time%"), printDateTime(time));
	// Look for %time{X}%
	QRegExp timeRegExp("%time\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos = timeRegExp.indexIn(result , pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), AdiumTimeFormatter::convertTimeDate(timeRegExp.cap(1), time));

	result.replace("%shortTime%", printDateTime(time));

	// Look for %textbackgroundcolor{X}%
	// TODO: highlight background color: use the X value.
	QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
	int textPos = 0;
	while ((textPos=textBackgroundRegExp.indexIn(result, textPos)) != -1)
		result.replace(textPos, textBackgroundRegExp.cap(0).length(), "inherit");

	// Replace userIconPath
	QString photoPath;
	if (message.type() == MessageTypeReceived)
	{
		result.replace(QString("%messageClasses%"), "message incoming");

		const Avatar &avatar = message.messageSender().avatar(true);
		if (!avatar.isEmpty())
			photoPath = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoPath = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}
	else if (message.type() == MessageTypeSent)
	{
		result.replace(QString("%messageClasses%"), "message outgoing");
		const Avatar &avatar = message.messageChat().chatAccount().accountContact().avatar(true);
		if (!avatar.isEmpty())
			photoPath = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoPath = KaduPaths::webKitPath(styleHref + QLatin1String("Outgoing/buddy_icon.png"));
	}
	else
		result.remove(QString("%messageClasses%"));

	result.replace(QString("%userIconPath%"), photoPath);

	//Message direction ("rtl"(Right-To-Left) or "ltr"(Left-to-right))
	result.replace(QString("%messageDirection%"), "ltr");

	// Replace contact's color
	QString lightColorName;
	QRegExp senderColorRegExp("%senderColor(?:\\{([^}]*)\\})?%");
	textPos = 0;
	while ((textPos = senderColorRegExp.indexIn(result, textPos)) != -1)
	{
		int light = 100;
		bool doLight = false;
		if (senderColorRegExp.captureCount() >= 1)
			light = senderColorRegExp.cap(1).toInt(&doLight);

		if (doLight && lightColorName.isNull())
			lightColorName = QColor(nickColor).light(light).name();

		result.replace(textPos, senderColorRegExp.cap(0).length(), doLight ? lightColorName : nickColor);
	}

	QString messageText = CurrentMessageHtmlRendererService
			? CurrentMessageHtmlRendererService.data()->renderMessage(message)
			: message.htmlContent();

	if (!message.id().isEmpty())
		messageText.prepend(QString("<span id=\"message_%1\">").arg(message.id()));
	else
		messageText.prepend("<span>");
	messageText.append("</span>");

	result.replace(QString("%messageId%"), message.id());
	result.replace(QString("%messageStatus%"), QString::number(message.status()));

	result.replace(QString("%message%"), messageText);

	return result;
}

#include "moc_chat-engine-adium.cpp"
