/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin.h"

#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/plugin-error-dialog.h"
#include "misc/kadu-paths.h"
#include "plugins/generic-plugin.h"
#include "plugins/plugin-info.h"
#include "debug.h"

#include <QtCore/QFileInfo>
#include <QtCore/QPluginLoader>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#ifdef Q_OS_MAC
	#define SO_PREFIX "lib"
	#define SO_EXT "dylib"
#elif defined(Q_OS_WIN)
	#define SO_PREFIX ""
	#define SO_EXT "dll"
#else
	#define SO_PREFIX "lib"
	#define SO_EXT "so"
#endif

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Creates new Plugin object and loads PluginInfo data.
 * @param name name of plugin
 * @param parent parent object, almost always PluginManager instance
 *
 * This contructor load data for plugin with given name. New instance of PluginInfo class
 * is created if dataDir/kadu/plugins/name.desc is found. If this file is not found, plugin
 * is marked as invalid and will be unable to be activated.
 */
Plugin::Plugin(const QString &name, QObject *parent) :
		QObject{parent},
		m_name{name}, m_active{false}, m_state{PluginStateNew}, m_pluginLoader{nullptr}, m_pluginObject{nullptr},
		m_translator{nullptr}, m_usageCounter{0}
{
	auto descFilePath = KaduPaths::instance()->dataPath() + QLatin1String("plugins/") + name + QLatin1String(".desc");
	auto descFileInfo = QFileInfo{descFilePath};

	if (descFileInfo.exists())
		m_info = new PluginInfo(descFilePath);
	else
		m_info = nullptr;

	StorableObject::setState(StateNotLoaded);
}

Plugin::~Plugin()
{
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Loadas plugin data from storage.
 *
 * This method load plugin data (for now it is only State) from /Plugins/Plugin[\@name=pluginName]/ storage node.
 */
void Plugin::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	auto stateString = loadValue<QString>("State");
	if (stateString == "Loaded")
		m_state = PluginStateEnabled;
	else if (stateString == "NotLoaded")
		m_state = PluginStateDisabled;
	else
		m_state = PluginStateNew;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Stores plugin data to storage.
 *
 * This method stores plugin data (for now it is only State) into /Plugins/Plugin[\@name=pluginName]/ storage node.
 */
void Plugin::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	switch (m_state)
	{
		case PluginStateEnabled:
			storeValue("State", "Loaded");
			break;
		case PluginStateDisabled:
			storeValue("State", "NotLoaded");
			break;
		default:
			Q_ASSERT(false);
			break;
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Reimplemented from StorableObject::shouldStore().
 *
 * Reimplemented from StorableObject::shouldStore(). Returns false if State is PluginStateNew.
 */
bool Plugin::shouldStore()
{
	ensureLoaded();

	return NamedStorableObject::shouldStore() && PluginStateNew != state();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Returns true if this plugin should be activated.
 * @return true if this plugin should be activated
 *
 * Module should be activated only if:
 * <ul>
 *   <li>it is valid (has .desc file associated with it)
 *   <li>is either PluginStateEnabled or PluginStateNew with PluginInfo::loadByDefault() set to true
 * </ul>
 */
bool Plugin::shouldBeActivated()
{
	ensureLoaded();

	if (!isValid() || isActive())
		return false;

	if (PluginStateEnabled == m_state)
		return true;
	if (PluginStateDisabled == m_state)
		return false;
	return m_info->loadByDefault();
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Activates plugin and retursn true if plugin is active.
 * @param reason plugin activation reason
 * @return true if plugin is active after method return
 *
 * This method loads plugin library file (if exists) and set up  GenericPlugin object from this library.
 * Then translations file is loaded. Next GenericPlugin::init() method is called with firstLoad
 * paramer set to true if this plugin's state  if PluginStateNew. If this methods returns value different
 * than 0, plugin is deactivated and false value is returned.
 *
 * Translations must be loaded before the root component of the plugin is instantiated.
 *
 * This method returns true if plugin is active after method returns - especially when plugin was active
 * before this call.
 */
bool Plugin::activate(PluginActivationReason reason)
{
	if (m_active)
		return true;

	ensureLoaded();

	m_pluginLoader = new QPluginLoader(KaduPaths::instance()->pluginsLibPath() + QLatin1String(SO_PREFIX) + m_name + QLatin1String("." SO_EXT));
	m_pluginLoader->setLoadHints(QLibrary::ExportExternalSymbolsHint);

	if (!m_pluginLoader->load())
	{
		QString err = m_pluginLoader->errorString();
		kdebugm(KDEBUG_ERROR, "cannot load %s because of: %s\n", qPrintable(m_name), qPrintable(err));
		activationError(tr("Cannot load %1 plugin library:\n%2").arg(m_name, err), reason);

		delete m_pluginLoader;
		m_pluginLoader = nullptr;

		kdebugf2();
		return false;
	}

	// Load translations before the root component of the plugin is instantiated (it is done by instance() method).
	loadTranslations();

	m_pluginObject = qobject_cast<GenericPlugin *>(m_pluginLoader->instance());
	if (!m_pluginObject)
	{
		activationError(tr("Cannot find required object in module %1.\nMaybe it's not Kadu-compatible plugin.").arg(m_name), reason);

		// Refer to deactivate() method for reasons to this.
		QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
		m_pluginLoader->unload();
		delete m_pluginLoader;
		m_pluginLoader = nullptr;

		unloadTranslations();

		kdebugf2();
		return false;
	}

	auto res = m_pluginObject->init(PluginStateNew == m_state);

	if (res != 0)
	{
		activationError(tr("Module initialization routine for %1 failed.").arg(m_name), reason);

		// Refer to deactivate() method for reasons to this.
		QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
		m_pluginLoader->unload();
		delete m_pluginLoader;
		m_pluginLoader = nullptr;
		m_pluginObject = nullptr;

		unloadTranslations();

		return false;
	}

	m_usageCounter = 0;

	/* This is perfectly intentional. We have to set state to either enabled or disabled, as new
	 * means that it was never loaded. If the only reason to load the plugin was because some other
	 * plugin depended upon it, set state to disabled as we don't want that plugin to be loaded
	 * next time when its reverse dependency will not be loaded. Otherwise set state to enabled.
	 */
	if (PluginActivationReasonDependency != reason)
		setState(PluginStateEnabled);
	else
		setState(PluginStateDisabled);

	m_active = true;

	kdebugf2();

	return true;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Deactivates plugin.
 *
 * If plugin is active, its GenericPlugin::done() method is called. Then all deferred delete
 * events are sent so that we will not end up trying to delete objects belonging to unloaded
 * plugins. Finally all data is removed from memory - plugin library file and plugin translations.
 */
void Plugin::deactivate(PluginDeactivationReason reason)
{
	if (!m_active)
		return;

	if (m_pluginObject)
		m_pluginObject->done();

	// We need this because plugins can call deleteLater() just before being
	// unloaded. In this case control would not return to the event loop before
	// unloading the plugin and the event loop would try to delete objects
	// belonging to already unloaded plugins, which can result in segfaults.
	QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

	if (m_pluginLoader)
	{
		m_pluginLoader->unload();
		m_pluginLoader->deleteLater();
		m_pluginLoader = nullptr;
	}
	m_pluginObject = nullptr;

	// We cannot unload translations before calling PluginObject->done(), see #2177.
	unloadTranslations();

	m_active = false;

	if (PluginDeactivationReasonUserRequest == reason)
		setState(PluginStateDisabled);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Loads translations for plugin.
 *
 * This method loads translations for current Kadu's language. Translations are loaded from
 * dataDir/kadu/plugins/translations/pluginName_lang.qm file.
 */
void Plugin::loadTranslations()
{
	m_translator = new QTranslator{this};
	auto const lang = config_file.readEntry("General", "Language");

	if (m_translator->load(m_name + '_' + lang, KaduPaths::instance()->dataPath() + QLatin1String("plugins/translations")))
		qApp->installTranslator(m_translator);
	else
	{
		delete m_translator;
		m_translator = nullptr;
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @author Bartosz 'beevvy' Brachaczek
 * @short Unloads translations for plugin.
 *
 * This method unloads existing translations for this plugin. Translations are removed
 * from QApplication instance and \c Translator is deleted.
 */
void Plugin::unloadTranslations()
{
	if (m_translator)
	{
		qApp->removeTranslator(m_translator);
		delete m_translator;
		m_translator = nullptr;
	}
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Sets state of plugin.
 *
 * This method changes state of plugin. Set state to PluginStateEnabled to make this plugin
 * activate at every Kadu run.
 *
 * Please do not call this method unless you are absolutely sure the plugin had been loaded
 * at least once.
 */
void Plugin::setState(Plugin::PluginState state)
{
	ensureLoaded();

	m_state = state;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets state enablement of plugin if it is inactive.
 *
 * If this plugin is active or its state is PluginStateNew, this method does nothing.
 *
 * Otherwise, this method sets its state to PluginStateEnabled if \p enable is true.
 * If \p enable is false, this method sets the plugin's state to PluginStateDisabled.
 */
void Plugin::setStateEnabledIfInactive(bool enable)
{
	if (isActive())
		return;

	// It is necessary to not break firstLoad.
	if (PluginStateNew == state())
		return;

	setState(enable ? PluginStateEnabled : PluginStateDisabled);
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Shows activation error to the user.
 * @param errorMessage error message that will be displayer to the user
 * @param activationReason plugin activation reason
 * @todo it really shouldn't call gui classes directly
 *
 * This method creates new PluginErrorDialog with message \p errorMessage and opens it. Depending on
 * \p activationReason, it also intructs the dialog wheter to offer the user choice wheter to try
 * to load this plugin automatically in future.
 */
void Plugin::activationError(const QString &errorMessage, PluginActivationReason activationReason)
{
	auto offerLoadInFutureChoice = (PluginActivationReasonKnownDefault == activationReason);

	// TODO: set parent to MainConfigurationWindow is it exists
	auto errorDialog = new PluginErrorDialog(errorMessage, offerLoadInFutureChoice, 0);
	if (offerLoadInFutureChoice)
		connect(errorDialog, SIGNAL(accepted(bool)), this, SLOT(setStateEnabledIfInactive(bool)));

	QTimer::singleShot(0, errorDialog, SLOT(open()));
}

#include "moc_plugin.cpp"
