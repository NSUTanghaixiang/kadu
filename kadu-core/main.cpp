/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include <errno.h>
#include <time.h>
#ifndef Q_OS_WIN32
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif // !Q_OS_WIN32

#include "configuration/configuration-api.h"
#include "configuration/configuration-factory.h"
#include "configuration/configuration-module.h"
#include "configuration/configuration-path-provider.h"
#include "configuration/configuration-unusable-exception.h"
#include "configuration/configuration-writer.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "core/core-module.h"
#include "execution-arguments/execution-arguments-parser.h"
#include "execution-arguments/execution-arguments.h"
#include "gui/widgets/chat-widget/chat-widget-module.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "misc/date-time.h"
#include "misc/paths-provider.h"
#include "os/qtsingleapplication/qtlocalpeer.h"
#include "os/win/wsa-exception.h"
#include "os/win/wsa-handler.h"
#include "protocols/protocols-manager.h"
#include "debug.h"
#include "kadu-config.h"

#include <injeqt/injector.h>

#ifndef Q_OS_WIN32
#if HAVE_EXECINFO
#include <execinfo.h>
#endif

static void printBacktrace(const QString &header)
{
	if (header.isEmpty())
		fprintf(stderr, "\nbacktrace:\n");
	else
		fprintf(stderr, "\nbacktrace: ('%s')\n", qPrintable(header));
#if HAVE_EXECINFO
	void *bt_array[100];
	char **bt_strings;
	int num_entries;
	if ((num_entries = backtrace(bt_array, 100)) < 0) {
		fprintf(stderr, "could not generate backtrace\n");
		return;
	}
	if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
		fprintf(stderr, "could not get symbol names for backtrace\n");
		return;
	}
	fprintf(stderr, "======= BEGIN OF BACKTRACE =====\n");
	for (int i = 0; i < num_entries; ++i)
		fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
	fprintf(stderr, "======= END OF BACKTRACE  ======\n");
	free(bt_strings);
#else
	fprintf(stderr, "backtrace not available\n");
#endif
	fflush(stderr);
}

static void kaduQtMessageHandler(QtMsgType type, const char *msg)
{
	switch (type)
	{
		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", msg);
			fflush(stderr);
			break;
		case QtWarningMsg:
			fprintf(stderr, "\033[34mWarning: %s\033[0m\n", msg);
			fflush(stderr);
			if (strstr(msg, "no mimesource for") == 0)
				printBacktrace("warning from Qt (above)");
			break;
		case QtCriticalMsg:
			fprintf(stderr, "\033[31;1mCritical: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("critical error from Qt (above)");
			break;
		case QtFatalMsg:
			fprintf(stderr, "\033[31;1mFatal: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("fatal error from Qt (above)");
			abort();
			break;
	}
}
#endif // !Q_OS_WIN32

#ifdef DEBUG_OUTPUT_ENABLED
extern KADUAPI bool showTimesInDebug;
#endif

// defined in main_unix.cpp and main_win32.cpp
void enableSignalHandling();

static void printVersion()
{
	printf(
		"Kadu %s Copyright (c) 2001-2012 Kadu Team\n"
		"Compiled with Qt %s\nRunning on Qt %s\n",
		qPrintable(Core::version()), QT_VERSION_STR, qVersion());
}

static void printUsage()
{
	printf(
		"Usage: kadu [General Options] [Options]\n\n"
		"Kadu Instant Messenger\n"
		"\nGeneral Options:\n"
		"  --help                     Print Kadu options\n"
		"  --version                  Print Kadu and Qt version\n"
		"\nOptions:\n"
		"  --debug <mask>             Set debugging mask\n"
		"  --config-dir <path>        Set configuration directory\n"
		"                             (overwrites CONFIG_DIR variable)\n");
}

int main(int argc, char *argv[]) try
{
	WSAHandler wsaHandler;

	QApplication application{argc, argv};
	application.setApplicationName("Kadu");
	application.setQuitOnLastWindowClosed(false);

	auto executionArgumentsParser = ExecutionArgumentsParser{};
	// do not parse program name
	auto executionArguments = executionArgumentsParser.parse(QCoreApplication::arguments().mid(1));

	if (executionArguments.queryVersion())
	{
		printVersion();
		return 0;
	}

	if (executionArguments.queryUsage())
	{
		printUsage();
		return 0;
	}

	if (!executionArguments.debugMask().isEmpty())
	{
		bool ok;
		executionArguments.debugMask().toInt(&ok);
		if (ok)
			qputenv("DEBUG_MASK", executionArguments.debugMask().toUtf8());
		else
			fprintf(stderr, "Ignoring invalid debug mask '%s'\n", executionArguments.debugMask().toUtf8().constData());
	}

	auto profileDirectory = executionArguments.profileDirectory().isEmpty()
			? QString::fromUtf8(qgetenv("CONFIG_DIR"))
			: executionArguments.profileDirectory();

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(make_unique<ChatWidgetModule>());
	modules.emplace_back(make_unique<CoreModule>(std::move(profileDirectory)));
	modules.emplace_back(make_unique<ConfigurationModule>());

	auto injector = injeqt::injector{std::move(modules)};

	try
	{
		injector.get<Application>(); // force creation of Application object
	}
	catch (ConfigurationUnusableException &)
	{
		auto profilePath = injector.get<ConfigurationPathProvider>()->configurationDirectoryPath();
		auto errorMessage = QCoreApplication::translate("@default", "We're sorry, but Kadu cannot be loaded. "
				"Profile is inaccessible. Please check permissions in the '%1' directory.")
				.arg(profilePath.left(profilePath.length() - 1));
		QMessageBox::critical(0, QCoreApplication::translate("@default", "Profile Inaccessible"), errorMessage, QMessageBox::Abort);

		throw;
	}

#ifndef Q_OS_WIN32
	// Qt version is better on win32
	qInstallMsgHandler(kaduQtMessageHandler);
#endif

#ifdef DEBUG_OUTPUT_ENABLED
	showTimesInDebug = (0 != qgetenv("SHOW_TIMES").toInt());
#endif

	enableSignalHandling();

	const QString lang = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Language", QLocale::system().name().left(2));
	QTranslator qt_qm, kadu_qm;
	qt_qm.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	kadu_qm.load("kadu_" + lang, Application::instance()->pathsProvider()->dataPath() + QLatin1String("translations"));
	QCoreApplication::installTranslator(&qt_qm);
	QCoreApplication::installTranslator(&kadu_qm);

	QtLocalPeer *peer = new QtLocalPeer(&application, Application::instance()->pathsProvider()->profilePath());
	if (peer->isClient())
	{
		if (!executionArguments.openIds().isEmpty())
			for (auto const &id : executionArguments.openIds())
				peer->sendMessage(id, 1000);
		else
			peer->sendMessage("activate", 1000);

		return 1;
	}

	Core::createInstance(injector);
	Core::instance()->createGui();
	Core::instance()->runGuiServices();
	QObject::connect(peer, SIGNAL(messageReceived(const QString &)),
			Core::instance(), SLOT(receivedSignal(const QString &)));

	Core::instance()->activatePlugins();

	for (auto const &id : executionArguments.openIds())
		Core::instance()->receivedSignal(id);

	// it has to be called after loading modules (docking might want to block showing the window)
	Core::instance()->showMainWindow();
	Core::instance()->initialized();

	int ret = QApplication::exec();
	kdebugm(KDEBUG_INFO, "after exec\n");

	kdebugm(KDEBUG_INFO, "exiting main\n");

	return ret;
}
#if defined(Q_OS_WIN32)
catch (WSAException &)
{
	return 2;
}
#endif
catch (ConfigurationUnusableException &)
{
	// already handled
}
catch (...)
{
	throw;
}
