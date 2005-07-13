/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qapplication.h>
#include <qtranslator.h>
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <qmessagebox.h>
#include <qtimer.h>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include "kadu.h"
#include "kadu-config.h"

#include "config_file.h"
#include "misc.h"
#include "debug.h"
#include "modules.h"
#include "emoticons.h"
#include "message_box.h"

//patrz komentarz w config_file.h
ConfigFile *config_file_ptr;

Kadu *kadu;	

#ifdef SIG_HANDLING_ENABLED
#include <qdatetime.h>
#include <signal.h>
#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif
static void kadu_signal_handler(int s)
{
	kdebugmf(KDEBUG_WARNING, "%d\n", s);
	
	if (s==SIGSEGV)
	{
		kdebugm(KDEBUG_PANIC, "Kadu crashed :(\n");
		QString f = QString("kadu.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
		QString debug_file = QString("kadu.backtrace.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));

		if(lockFile)
		{ // moze sie wywalic praktycznie po wylaczeniu i to tez trzeba uwzglednic	
			flock(lockFileHandle, LOCK_UN);
			kdebugm(KDEBUG_WARNING, "lock released\n");
			lockFile->close();
			kdebugm(KDEBUG_WARNING, "lockfile closed\n");
		}
#ifdef HAVE_EXECINFO
		void *bt_array[100];
		char **bt_strings;
		int num_entries;
		FILE *dbgfile;
		if ((num_entries = backtrace(bt_array, 100)) < 0) {
			kdebugm(KDEBUG_PANIC, "could not generate backtrace\n");
			abort();
		}
		if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
			kdebugm(KDEBUG_PANIC, "could not get symbol names for backtrace\n");
			abort();
		}

		fprintf(stderr, "\n======= BEGIN OF BACKTRACE =====\n");
		for (int i = 0; i < num_entries; ++i)
			fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
		fprintf(stderr, "======= END OF BACKTRACE  ======\n");
		fflush(stderr);

		dbgfile = fopen(ggPath(debug_file), "w");
		if (dbgfile)
		{
			fprintf(dbgfile, "======= BEGIN OF BACKTRACE =====\n");
			for (int i = 0; i < num_entries; ++i)
				fprintf(dbgfile, "[%d] %s\n", i, bt_strings[i]);
			fprintf(dbgfile, "======= END OF BACKTRACE  ======\n");
			fflush(dbgfile);

			fprintf(dbgfile, "static modules:\n");
			QStringList modules = modules_manager->staticModules();
			
			CONST_FOREACH(module, modules)
				fprintf(dbgfile, "> %s\n", (*module).local8Bit().data());
			fflush(dbgfile);

			fprintf(dbgfile, "loaded modules:\n");
			modules = modules_manager->loadedModules();
			CONST_FOREACH(module, modules)
				fprintf(dbgfile, "> %s\n", (*module).local8Bit().data());
			fflush(dbgfile);
			fprintf(dbgfile, "Qt compile time version: %d.%d.%d\nQt runtime version: %s\n", (QT_VERSION&0xff0000)>>16, (QT_VERSION&0xff00)>>8, QT_VERSION&0xff, qVersion());
			fprintf(dbgfile, "Kadu version: %s\n", VERSION);
			#ifdef __DATE__
			fprintf(dbgfile, "Compile time: %s %s\n", __DATE__, __TIME__);
			#endif
			#ifdef __GNUC__
				//w gcc < 3.0 nie ma __GNUC_PATCHLEVEL__
				#ifdef __GNUC_PATCHLEVEL__
					fprintf(dbgfile, "GCC version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
				#else
					fprintf(dbgfile, "GCC version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
				#endif
			#endif
			fprintf(dbgfile, "EOF\n");

			fclose(dbgfile);
		}

		free(bt_strings);
#else
		kdebugm(KDEBUG_PANIC, "backtrace not available\n");		
#endif
		xml_config_file->saveTo(ggPath(f.latin1()));
		abort();
	}
	else if (s==SIGINT || s==SIGTERM)
		qApp->postEvent(qApp, new QEvent(QEvent::Quit));
}
#endif

int main(int argc, char *argv[])
{


	xml_config_file = new XmlConfigFile();
	config_file_ptr = new ConfigFile(ggPath(QString("kadu.conf")));
	config_file.addVariable("General", "DEBUG_MASK", KDEBUG_ALL & ~KDEBUG_FUNCTION_END);
	debug_mask=config_file.readNumEntry("General", "DEBUG_MASK");
	char *d = getenv("DEBUG_MASK");
	if (d)
		debug_mask=atol(d);
	gg_debug_level=debug_mask | ~255;

#ifdef SIG_HANDLING_ENABLED
	bool sh_enabled=true;
	d=getenv("SIGNAL_HANDLING");
	if (d)
		sh_enabled=(atoi(d)!=0);
	if (sh_enabled)
	{
		signal(SIGSEGV, kadu_signal_handler);
		signal(SIGINT, kadu_signal_handler);
		signal(SIGTERM, kadu_signal_handler);
	}
#endif

	//op�nienie uruchomienia, przydatne w GNOME
	config_file.addVariable("General", "StartDelay", 0);
	sleep(config_file.readNumEntry("General", "StartDelay"));
	dataPath("", argv[0]);
	emoticons=new EmoticonsManager();
	
	new QApplication(argc, argv);
	defaultFontInfo = new QFontInfo(qApp->font());
	defaultFont = new QFont(defaultFontInfo->family(), defaultFontInfo->pointSize());
	// �adowanie t�umaczenia
	config_file.addVariable("General", "Language", QString(QTextCodec::locale()).mid(0,2));
	QTranslator qt_qm(0, "Translator_qt");
	QString lang = config_file.readEntry("General", "Language");
	qt_qm.load(dataPath(QString("kadu/translations/qt_") + lang), ".");
	qApp->installTranslator(&qt_qm);
	QTranslator kadu_qm(0, "Translator_kadu");
	kadu_qm.load(dataPath(QString("kadu/translations/kadu_") + lang), ".");
	qApp->installTranslator(&kadu_qm);
	qApp->setStyle(config_file.readEntry("Look", "QtStyle"));

	lockFile = new QFile(ggPath("lock"));
	if (lockFile->open(IO_ReadWrite))
	{
		lockFileHandle = lockFile->handle();
		if (flock(lockFileHandle, LOCK_EX | LOCK_NB) != 0)
		{
			kdebugm(KDEBUG_WARNING, "flock: %s\n", strerror(errno));
			if (QMessageBox::warning(NULL, "Kadu",
				qApp->translate("@default", QT_TR_NOOP("Another Kadu is running on this profile.")),
				qApp->translate("@default", QT_TR_NOOP("Force running Kadu (not recommended).")),
				qApp->translate("@default", QT_TR_NOOP("Quit.")), 0, 1, 1) == 1)
			{
			    delete defaultFont;
			    delete defaultFontInfo;
			    delete emoticons;
			    delete xml_config_file;
			    delete config_file_ptr;
			    lockFile->close();
			    delete lockFile;
			    delete qApp;
			    return 1;
			}
		}
	}

	IconsManager::initModule();
	kadu = new Kadu(0, "Kadu");

	qApp->setMainWidget(kadu);
	QPixmap pix;
#ifdef Q_OS_MACX
	pix = icons_manager.loadIcon("BigOffline");
#else
	pix = icons_manager.loadIcon("Offline");
#endif
	kadu->setMainWindowIcon(pix);

	ModulesManager::initModule();

	kadu->startupProcedure();

	QObject::connect(qApp, SIGNAL(aboutToQuit()), kadu, SLOT(quitApplication()));

	// je�li kto� uruchomi� kadu jako root to przypomnijmy mu, �e
	// tak nie nale�y post�powa� (leczymy nawyki z win32)
	if (geteuid() == 0)
		MessageBox::wrn(qApp->translate("@default", QT_TR_NOOP("Please do not run Kadu as a root!\nIt's a high security risk!")));
	QTimer::singleShot(15000, kadu, SLOT(deleteOldConfigFiles()));
	int ret=qApp->exec();
	delete qApp;
	return ret;
}
