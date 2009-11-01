/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_OPTIONS_CONFIGURATION_WIDGET
#define CONTACT_OPTIONS_CONFIGURATION_WIDGET

#include <QtGui/QDialog>
#include <QtGui/QCheckBox>

#include "buddies/buddy.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ContactAccountData;
class ContactAccountDataManager;

class KADUAPI ContactOptionsConfigurationWidget : public QWidget
{
	Q_OBJECT

	QCheckBox *BlockCheckBox;
	QCheckBox *OfflineToCheckBox;
	QCheckBox *NotifyCheckBox;

	Buddy CurrentContact;
	void createGui();

public:
	ContactOptionsConfigurationWidget(Buddy &contact, QWidget *parent = 0);
	~ContactOptionsConfigurationWidget();
	void saveConfiguration();

};

#endif // CONTACT_OPTIONS_CONFIGURATION_WIDGET
