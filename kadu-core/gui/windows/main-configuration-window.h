#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

#include <QtCore/QWeakPointer>

#include "gui/windows/configuration-window.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QCheckBox;
class QSlider;

class BuddyListBackgroundColorsWidget;
class ConfigComboBox;
class ConfigLineEdit;
class PluginListWidget;
class Preview;
class SyntaxEditorWindow;

class MainConfigurationWindow;
/**
	@class ConfigurationUiHandler
	@author Vogel
	@short Klasa bazowa dla klas obs�uguj�cych okno konfiguracyjne.

	Klasa dziedzicz�ca z tej b�dzie informowana o stworzeniu nowego g��wnego okna
	konfiguracyjnego co da jej mo�liwo�� podpi�cia si� pod sygna�y odpowiednich
	kontrolek i odpowiedniej reakcji na nie.
 **/
class KADUAPI ConfigurationUiHandler : public QObject
{
	Q_OBJECT

public:
	explicit ConfigurationUiHandler(QObject *parent = 0) : QObject(parent) {}
	virtual ~ConfigurationUiHandler() {}

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) = 0;

};

class ConfigFileDataManager;

/**
	@class MainConfigurationWindow
	@author Vogel
	@short G��wne okno konfiguracyjne.
 **/
class KADUAPI MainConfigurationWindow : public ConfigurationWindow, CompositingAwareObject
{
	Q_OBJECT

	static MainConfigurationWindow *Instance;
	static ConfigFileDataManager *InstanceDataManager;

	static QList<QString> UiFiles;
	static QList<ConfigurationUiHandler *> ConfigurationUiHandlers;

	static void instanceCreated();

	QWeakPointer<ConfigurationWindow> lookChatAdvanced;
	QWeakPointer<ConfigurationWindow> lookColorsAdvanced;

	QCheckBox *onStartupSetLastDescription;
	QCheckBox *userboxTransparency;
	QSlider *userboxAlpha;
	QCheckBox *userboxBlur;
	BuddyListBackgroundColorsWidget *buddyColors;
	PluginListWidget *PluginList;

	explicit MainConfigurationWindow();

	void setLanguages();

	void setToolTipClasses();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

private slots:
	void onChangeStartupStatus(int index);

	void showLookChatAdvanced();
	void showColorsAdvanced();

	void setIconThemes();

	void chatPreviewSyntaxChanged(const QString &syntaxName);

public:
	static MainConfigurationWindow * instance();
	static bool hasInstance();
	static ConfigFileDataManager * instanceDataManager();

	/**
		Rejestracja nowego pliku *.ui (dokonywana przez modu�u).
		Plik uiFile zostanie wczytany wraz z otwarciem g��wnego okna konfiguracyjnego.
		Obiekt uiHandle zostanie poinformowany o stworzeniu okna i b�dzie m�g� doda�
		do niego w�asne interakcje.
		@note Należy pamiętać, aby wywołać tę metodę zawsze przed korespondującym
		      wywołaniem metody \c registerUiHandler() (o ile takie istnieje).
	 **/
	static void registerUiFile(const QString &uiFile);
	/**
		Wyrejestrowanie pliku *.ui i klasy obs�uguj�cej okno konfiguracyjne.
		@note Należy pamiętać, aby wywołać tę metodę zawsze po korespondującym
		      wywołaniu metody \c unregisterUiHandler() (o ile takie istnieje).
	 **/
	static void unregisterUiFile(const QString &uiFile);

	/**
	 * @note Always remember to call this method after correspoding
	 *       \c registerUiHandler() method call (if such exists).
	 */
	static void registerUiHandler(ConfigurationUiHandler *uiHandler);
	/**
	 * @note Always remember to call this method before correspoding
	 *       \c unregisterUiFile() method call (if such exists).
	 */
	static void unregisterUiHandler(ConfigurationUiHandler *uiHandler);

	static const char *SyntaxText;
	static const char *SyntaxTextNotify;

	virtual ~MainConfigurationWindow();

	virtual void show();

};

#endif // MAIN_CONFIGURATION_WINDOW_H
