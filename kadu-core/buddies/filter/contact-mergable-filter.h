/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONACT_MERGABLE_FILTER_H
#define CONACT_MERGABLE_FILTER_H

#include "buddies/buddy.h"
#include "buddies/filter/abstract-contact-filter.h"

class ContactMergableFilter : public AbstractContactFilter
{
	Q_OBJECT

	QSet<Account> Accounts;
	Buddy MyContact;

public:
	explicit ContactMergableFilter(Buddy contact, QObject *parent = 0);
	virtual ~ContactMergableFilter();

	virtual bool acceptContact(Buddy contact);

	void setContact(Buddy contact);

};

#endif // CONACT_MERGABLE_FILTER_H
