/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HINTS_CONFIGURATION_UI_HANDLER_H
#define HINTS_CONFIGURATION_UI_HANDLER_H

#include <QtCore/QWeakPointer>

#include "gui/windows/main-configuration-window.h"

#include "hint_manager.h"

class QComboBox;
class QSpinBox;

class HintsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

private:
	QWeakPointer<ConfigurationWindow> AdvancedWindow;

	QFrame *previewHintsFrame;
	QVBoxLayout *previewHintsLayout;

	QList<Hint *> previewHints;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;
	QSpinBox *xPosition;
	QSpinBox *yPosition;
	QCheckBox *ownPosition;
	QComboBox *ownPositionCorner;
	QComboBox *newHintUnder;
	
	QWeakPointer<HintOverUserConfigurationWindow> overUserConfigurationWindow;
	QFrame *overUserConfigurationPreview;
	QLabel *overUserConfigurationTipLabel;
	
	QPushButton *configureOverUserHint;
	void setPreviewLayoutDirection();

private slots:
	void showAdvanced();

	void minimumWidthChanged(int value);
	void maximumWidthChanged(int value);
	
	void toolTipClassesHighlighted(const QString &value);

	void mainConfigurationWindowDestroyed();
	void showOverUserConfigurationWindow();
	void updateOverUserPreview();
	void addHintsPreview();
	void updateHintsPreview();
	void deleteHintsPreview(Hint *hint);
	void deleteAllHintsPreview();

signals:
	void searchingForTrayPosition(QPoint &pos);

public:
	explicit HintsConfigurationUiHandler(const QString &style, QObject *parent = 0);
	virtual ~HintsConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif // HINTS_CONFIGURATION_UI_HANDLER_H
