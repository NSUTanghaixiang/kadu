/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "icons/icons-manager.h"
#include "notification/notification/aggregate-notification.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification/notification.h"
#include "parser/parser.h"
#include "debug.h"

#include "hint.h"

/**
 * @ingroup hints
 * @{
 */
Hint::Hint(QWidget *parent, Notification *xnotification)
	: QFrame(parent), vbox(0), callbacksBox(0), icon(0), label(0), bcolor(), notification(xnotification)
{
}

Hint::~Hint()
{
}

void Hint::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void Hint::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void Hint::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void Hint::setParser(Parser *parser)
{
	m_parser = parser;
}

void Hint::init()
{
	kdebugf();

	AggregateNotification *aggregateNotification = qobject_cast<AggregateNotification *>(notification);
	if (aggregateNotification)
	{
		notification = aggregateNotification->notifications().first();
	}

	CurrentChat = notification->data()["chat"].value<Chat>();

	startSecs = secs = m_configuration->deprecatedApi()->readNumEntry("Hints", "Event_" + notification->key() + "_timeout", 10);

	createLabels(m_iconsManager->iconByPath(notification->icon()).pixmap(m_configuration->deprecatedApi()->readNumEntry("Hints", "AllEvents_iconSize", 32)));

	auto callbacks = notification->getCallbacks();
	bool showButtons = !callbacks.isEmpty();
	if (showButtons)
		if (m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowOnlyNecessaryButtons"))
			showButtons = false;

	if (showButtons)
	{
		callbacksBox = new QHBoxLayout();
		callbacksBox->addStretch(10);
		vbox->addLayout(callbacksBox);

		for (auto &&callbackName : callbacks)
		{
			auto callback = m_notificationCallbackRepository->callback(callbackName);
			auto button = new QPushButton(callback.title(), this);
			button->setProperty("notify:callback", callbackName);
			connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

			callbacksBox->addWidget(button);
			callbacksBox->addStretch(1);
		}

		callbacksBox->addStretch(9);
	}

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed()));

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	configurationUpdated();
	updateText();
	show();

	kdebugf2();
}

void Hint::buttonClicked()
{
	auto callbackNotification = notification;
	if (qobject_cast<AggregateNotification *>(callbackNotification))
		callbackNotification = qobject_cast<AggregateNotification *>(callbackNotification)->notifications()[0];

	auto callbackName = sender()->property("notify:callback").toString();
	if (!callbackName.isEmpty())
	{
		auto callback = m_notificationCallbackRepository->callback(callbackName);
		callback.call(callbackNotification);
	}

	notification->close();
	close();
}

void Hint::configurationUpdated()
{
	QFont font(qApp->font());
	QPalette palette(qApp->palette());

	bcolor = m_configuration->deprecatedApi()->readColorEntry("Hints", "Event_" + notification->key() + "_bgcolor", &palette.window().color());
	fcolor = m_configuration->deprecatedApi()->readColorEntry("Hints", "Event_" + notification->key() + "_fgcolor", &palette.windowText().color());
	label->setFont(m_configuration->deprecatedApi()->readFontEntry("Hints", "Event_" + notification->key() + "_font", &font));
	setMinimumWidth(m_configuration->deprecatedApi()->readNumEntry("Hints", "MinimumWidth", 100));
	setMaximumWidth(m_configuration->deprecatedApi()->readNumEntry("Hints", "MaximumWidth", 500));
	mouseOut();
	updateText();
}

void Hint::createLabels(const QPixmap &pixmap)
{
	int margin = m_configuration->deprecatedApi()->readNumEntry("Hints", "MarginSize", 0);

	vbox = new QVBoxLayout(this);
	vbox->setSpacing(0);
	labels = new QHBoxLayout();
	labels->setSpacing(0);
	labels->setContentsMargins(margin + 4, margin + 2, margin + 4, margin + 2);
	vbox->addLayout(labels);

	if (!pixmap.isNull())
	{
		icon = new QLabel(this);
		icon->setPixmap(pixmap);
		icon->setContentsMargins(0, 0, margin + 4, 0);
		icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		// Without setting fixed size right margin is sometimes not respected on Windows.
		// If you cannot reproduce, try setting MarginSize to 5 px.
		icon->setFixedSize(icon->sizeHint());
		labels->addWidget(icon, 0, Qt::AlignTop);
	}

	label = new QLabel(this);
	label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
	label->setTextInteractionFlags(Qt::NoTextInteraction);
	label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	label->setWordWrap(true);
	labels->addWidget(label);
}

void Hint::updateText()
{
	QString text;

	QString syntax = m_configuration->deprecatedApi()->readEntry("Hints", "Event_" + notification->key() + "_syntax", QString());
	if (syntax.isEmpty())
		text = notification->text();
	else
	{
		kdebug("syntax is: %s, text is: %s\n", qPrintable(syntax), qPrintable(notification->text()));

		if (CurrentChat)
		{
			Contact contact = *CurrentChat.contacts().constBegin();
			text = m_parser->parse(syntax, Talkable(contact), notification, ParserEscape::HtmlEscape);
		}
		else
			text = m_parser->parse(syntax, notification, ParserEscape::HtmlEscape);

		/* Dorr: the file:// in img tag doesn't generate the image on hint.
		 * for compatibility with other syntaxes we're allowing to put the file://
		 * so we have to remove it here */
		text = text.remove("file://");
	}

	if (m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowContentMessage"))
	{
		QStringList details;
		if (!notification->details().isEmpty())
			details = notification->details();
		int count = details.count();

		if (count)
		{
			int i = (count > 5) ? count - 5 : 0;

			int citeSign = m_configuration->deprecatedApi()->readNumEntry("Hints","CiteSign");

			QString defaultSyntax;
			if (notification->type() == "NewMessage" || notification->type() == "NewChat")
				defaultSyntax = "\n&bull; <small>%1</small>";
			else
				defaultSyntax = "\n <small>%1</small>";
			QString itemSyntax = m_configuration->deprecatedApi()->readEntry("Hints", "Event_" + notification->key() + "_detailSyntax", defaultSyntax);
			for (; i < count; i++)
			{
				const QString &message = details[i].replace("<br/>", QLatin1String(""));

				if (message.length() > citeSign)
					text += itemSyntax.arg(details[i].left(citeSign) + "...");
				else
					text += itemSyntax.arg(details[i]);
			}
		}
	}

	label->setText(QString("<div style='width:100%; height:100%; vertical-align:middle;'>")
		+ text.replace('\n', QLatin1String("<br />"))
		+ "</div>"
		);

	adjustSize();
	updateGeometry();
	emit updated(this);
}

void Hint::resetTimeout()
{
	secs = startSecs;
}

void Hint::notificationClosed()
{
	emit closing(this);
}

void Hint::nextSecond(void)
{
	if (startSecs == 0)
		return;

	if (secs == 0)
	{
		kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
	}
	else if (secs > 2000000000)
	{
		kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");
	}

	if (secs > 0)
		--secs;
}

bool Hint::isDeprecated()
{
	return startSecs != 0 && secs == 0;
}

void Hint::notificationUpdated()
{
	resetTimeout();
	updateText();
}

void Hint::mouseReleaseEvent(QMouseEvent *event)
{
	switch (event->button())
	{
		case Qt::LeftButton:
			emit leftButtonClicked(this);
			break;

		case Qt::RightButton:
			emit rightButtonClicked(this);
			break;

		case Qt::MidButton:
			emit midButtonClicked(this);
			break;

		default:
			break;
	}
}

void Hint::enterEvent(QEvent *)
{
	mouseOver();
}

void Hint::leaveEvent(QEvent *)
{
	mouseOut();
}

void Hint::mouseOver()
{
	QString style = QString("* {color:%1; background-color:%2;}").arg(fcolor.name(), bcolor.lighter().name());
	setStyleSheet(style);
}

void Hint::mouseOut()
{
	QString style = QString("* {color:%1; background-color:%2;}").arg(fcolor.name(), bcolor.name());
	setStyleSheet(style);
}

void Hint::getData(QString &text, QPixmap &pixmap, int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor)
{
	text = label->text().remove(' ');

	if (icon)
		pixmap = *(icon->pixmap());
	else
		pixmap = QPixmap();

	timeout = secs;
	font = label->font();
	fgcolor = fcolor;
	bgcolor = bcolor;
}

void Hint::acceptNotification()
{
	notification->callbackAccept();
}

void Hint::discardNotification()
{
	notification->callbackDiscard();
}

/** @} */


#include "moc_hint.cpp"
