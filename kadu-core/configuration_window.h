#ifndef KADU_CONFIGURATION_WINDOW_H
#define KADU_CONFIGURATION_WINDOW_H

#include <qdom.h>
#include <qgroupbox.h>

#include "color_button.h"
#include "hot_key.h"
#include "path_list_edit.h"
#include "select_font.h"
#include "syntax_editor.h"
#include "userbox.h"

class ConfigGroupBox;
class ConfigLineEdit;
class ConfigSection;
class ConfigTab;
class ConfigWidget;

class QGridLayout;
class QGroupBox;
class QVBox;

class ConfigGroupBox
{
	QString name;
	ConfigTab *configTab;

	QGroupBox *groupBox;
	QWidget *container;
	QGridLayout *gridLayout;

public:
	ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox);
	~ConfigGroupBox();

	QWidget * widget() { return container; }

	void addWidget(QWidget *widget, bool fullSpace = false);
	void addWidgets(QWidget *widget, QWidget *label);

	bool empty();

};

class ConfigurationWindow : public QVBox
{
	Q_OBJECT

	QString Name;

	QVBox *left;
	QMap<QString, ConfigSection *> configSections;
	ConfigSection *currentSection;

	QMap<QString, QWidget *> widgets;

	QListBox *sectionsListBox;
	QWidget *container;

	void loadConfiguration(QObject *object);
	void saveConfiguration(QObject *object);

	ConfigSection *configSection(const QString &name, bool create = true);

	QValueList<ConfigWidget *> processUiFile(const QString &fileName, bool append = true);
	QValueList<ConfigWidget *> processUiSectionFromDom(QDomNode sectionNode, bool append = true);
	QValueList<ConfigWidget *> processUiTabFromDom(QDomNode tabNode, const QString &sectionName, bool append = true);
	QValueList<ConfigWidget *> processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append = true);
	ConfigWidget * appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);
	void removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);

private slots:
	void updateAndCloseConfig();
	void updateConfig();

	void changeSection(const QString &newSectionName);

public:
	ConfigurationWindow(const QString &name);
	virtual ~ConfigurationWindow();

	QString name() { return Name; }

	ConfigGroupBox * configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create = true);

	virtual void show();

	QValueList<ConfigWidget *> appendUiFile(const QString &fileName, bool load = true);
	void removeUiFile(const QString &fileName);

	QWidget *widgetById(const QString &id);

	void removedConfigSection(const QString &sectionName);

signals:
	void configurationWindowApplied();

};

#endif // KADU_CONFIGURATION_WINDOW_H
