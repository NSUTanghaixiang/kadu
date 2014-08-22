/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GADU_CONTACT_PERSONAL_INFO_WIDGET_H
#define GADU_CONTACT_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "contacts/contact.h"

#include "exports.h"

class QComboBox;
class QLabel;

class GaduContactPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	QLabel *FirstNameText;
	QLabel *LastNameText;
	QLabel *NicknameText;
	QLabel *GenderText;
	QLabel *BirthdateText;
	QLabel *CityText;
	QLabel *StateProvinceText;
	QLabel *IpText;
	QLabel *PortText;
	QLabel *DnsNameText;
	QLabel *ProtocolVerText;

	Contact MyContact;
	void createGui();
	void reset();
	
private slots:
	void personalInfoAvailable(Buddy buddy);

public:
	explicit GaduContactPersonalInfoWidget(const Contact &contact, QWidget *parent = 0);
	virtual ~GaduContactPersonalInfoWidget();

};

#endif // GADU_CONTACT_PERSONAL_INFO_WIDGET_H
