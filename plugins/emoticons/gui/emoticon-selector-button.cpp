/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QMovie>

#include "configuration/configuration-file.h"

#include "emoticon-selector-button.h"

EmoticonSelectorButton::EmoticonSelectorButton(const Emoticon &emoticon, QWidget *parent) :
		QLabel(parent), DisplayEmoticon(emoticon)
{
	QPixmap p(DisplayEmoticon.staticFilePath());
	setPixmap(p.scaledToHeight(18, Qt::SmoothTransformation));
	setMouseTracking(true);
	setMargin(4);
	setFixedSize(sizeHint());
}

void EmoticonSelectorButton::buttonClicked()
{
	emit clicked(DisplayEmoticon.text());
}

void EmoticonSelectorButton::mouseMoveEvent(QMouseEvent *e)
{
	QLabel::mouseMoveEvent(e);
	MovieViewer *viewer = new MovieViewer(this);
	connect(viewer, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	viewer->show();
}

EmoticonSelectorButton::MovieViewer::MovieViewer(EmoticonSelectorButton *parent) :
		QLabel(parent, Qt::Popup)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(parent->sizeHint());
	setAlignment(Qt::AlignCenter);
	setMouseTracking(true);
	setToolTip(parent->DisplayEmoticon.text());

	QString style =
		"QLabel {"
		"	background-color: palette(highlight);"
		"	padding: 4px;"
		"}";
	setStyleSheet(style);

	QMovie *movie = new QMovie(this);
	movie->setFileName(parent->DisplayEmoticon.animatedFilePath());
	setMovie(movie);
	movie->start();

	// center on parent
	QPoint new_pos = parent->mapToGlobal(QPoint(0, 0));
	new_pos += QPoint(parent->sizeHint().width() / 2, parent->sizeHint().height() / 2);
	new_pos -= QPoint(sizeHint().width() / 2, sizeHint().height() / 2);
	move(new_pos);
}

void EmoticonSelectorButton::MovieViewer::mouseMoveEvent(QMouseEvent *e)
{
	QLabel::mouseMoveEvent(e);
	if (!rect().contains(e->globalPos() - mapToGlobal(QPoint(0, 0))))
		close();
}

void EmoticonSelectorButton::MovieViewer::mouseReleaseEvent(QMouseEvent *e)
{
	QLabel::mouseReleaseEvent(e);
	emit clicked();
}
