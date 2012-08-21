/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtXml/QDomDocument>

#include "dom/dom-processor.h"
#include "url-handlers/mail-url-expander.h"
#include "os/generic/url-opener.h"

#include "mail-url-handler.h"

MailUrlHandler::MailUrlHandler()
{
	MailRegExp = QRegExp("\\b[a-zA-Z0-9_\\.\\-]+@[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,4}\\b");
}

bool MailUrlHandler::isUrlValid(const QByteArray &url)
{
	return MailRegExp.exactMatch(QString::fromUtf8(url));
}

QString MailUrlHandler::convertUrlsToHtml(const QString &html, bool generateOnlyHrefAttr)
{
	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(html));

	MailUrlExpander urlExpander(MailRegExp, generateOnlyHrefAttr);

	DomProcessor domProcessor(domDocument);
	domProcessor.accept(&urlExpander);

	QString result = domDocument.toString(0);
	// remove <div></div>
	return result.mid(5, result.length() - 12);
}

void MailUrlHandler::openUrl(const QByteArray &url, bool disableMenu)
{
	Q_UNUSED(disableMenu)

	UrlOpener::openEmail(url);
}
