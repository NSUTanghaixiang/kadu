/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "modules.h"
#include "debug.h"
#include "message_box.h"
#include "config_file.h"
#include "kadu.h"
#include "kadu-config.h"

#include <qdir.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextcodec.h>

#include <dlfcn.h>

#include "modules_static.cpp"

Library::Library(const QString& file_name)
{
	FileName = file_name;
	Handle = NULL;
}

Library::~Library()
{
	if (Handle != NULL)
		dlclose(Handle);
}

bool Library::load()
{
	Handle = dlopen(FileName.local8Bit().data(), RTLD_NOW | RTLD_GLOBAL);
	return (Handle != NULL);
}

void* Library::resolve(const QString& symbol_name)
{
	if (Handle == NULL)
		return NULL;
	return dlsym(Handle, symbol_name.local8Bit().data());
}

QString Library::error()
{
	return QString(dlerror());
}

ModulesDialog::ModulesDialog()
	: QDialog(NULL,NULL)
{
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Manage Modules"));
	
	QHBoxLayout* layout=new QHBoxLayout(this);
	layout->setAutoAdd(true);
	
	QVBox* static_box=new QVBox(this);
	QVBox* installed_box=new QVBox(this);
	QVBox* loaded_box=new QVBox(this);	

	QLabel* StaticLabel = new QLabel(tr("Static"), static_box);
	QListBox* StaticListBox = new QListBox(static_box);
	StaticListBox->insertStringList(modules_manager->staticModules());

	QLabel* InstalledLabel = new QLabel(tr("Installed"), installed_box);
	InstalledListBox = new QListBox(installed_box);
	connect(InstalledListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(loadItem(QListBoxItem*)));
	
	QLabel* LoadedLabel = new QLabel(tr("Loaded"), loaded_box);
	LoadedListBox = new QListBox(loaded_box);
	connect(LoadedListBox,SIGNAL(doubleClicked(QListBoxItem*)),
		this,SLOT(unloadItem(QListBoxItem*)));

	QButton* InfoButton=new QPushButton(tr("Info"), static_box);
	connect(InfoButton, SIGNAL(clicked()),
		this, SLOT(getInfo()));

	QButton* LoadButton=new QPushButton(tr("Load"), installed_box);
	connect(LoadButton,SIGNAL(clicked()),
		this,SLOT(loadSelectedItem()));


	QButton* UnloadButton=new QPushButton(tr("Unload"), loaded_box);
	connect(UnloadButton,SIGNAL(clicked()),
		this,SLOT(unloadSelectedItem()));

	QRect def_rect(0, 0, 450, 400);
	config_file.addVariable("General", "ModulesDialogGeometry", def_rect);

	QRect geom=config_file.readRectEntry("General", "ModulesDialogGeometry");
	resize(geom.width(),geom.height());
	move(geom.x(),geom.y());

	refreshLists();
}

ModulesDialog::~ModulesDialog()
{
	kdebugf();
	QRect geom;
	geom.setX(pos().x());
	geom.setY(pos().y());
	geom.setWidth(size().width());
	geom.setHeight(size().height());
	
	config_file.writeEntry("General", "ModulesDialogGeometry",geom);
}

void ModulesDialog::loadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	if(modules_manager->activateModule(mod_name))
	{
		refreshLists();
		modules_manager->saveLoadedModules();
	}		
}

void ModulesDialog::unloadItem(QListBoxItem* item)
{
	QString mod_name=item->text();
	modules_manager->deactivateModule(mod_name);
	refreshLists();
	modules_manager->saveLoadedModules();
}

void ModulesDialog::loadSelectedItem()
{
	int current=InstalledListBox->currentItem();
	if(current>=0)
		loadItem(InstalledListBox->item(current));
}

void ModulesDialog::unloadSelectedItem()
{
	int current=LoadedListBox->currentItem();
	if(current>=0)
		unloadItem(LoadedListBox->item(current));
}

void ModulesDialog::refreshLists()
{
	LoadedListBox->clear();
	LoadedListBox->insertStringList(modules_manager->loadedModules());	
	InstalledListBox->clear();
	InstalledListBox->insertStringList(modules_manager->unloadedModules());
}

void ModulesDialog::getInfo()
{
	QListBoxItem *item;
	int current;

	if(InstalledListBox->hasFocus()){
		current=InstalledListBox->currentItem();
		item=InstalledListBox->item(current);
	}
	else{
		current=LoadedListBox->currentItem();
		item=LoadedListBox->item(current);
	}
	
	if(current>=0){
		ModuleInfo info;
		QString message;
		if(!modules_manager->moduleInfo(item->text(), info))
			return;
		message+=tr(
				"<b>Module:</b>"
				"<br>%1<br>"
				"<b>Depends on:</b><br>").arg(item->text());
		for (QStringList::Iterator it = info.depends.begin(); it != info.depends.end(); ++it)
			message+=QString("%1\n").arg((*it));
		message+=tr(
				"<br><b>Author:</b><br>"
				"%1<br>"
				"<b>Description</b>:<br>"
				"%2").arg(info.author).arg(info.description);
		MessageBox::msg(message);
	}
}

void ModulesManager::initModule()
{
	new ModulesManager();
}

void ModulesManager::closeModule()
{
	delete modules_manager;
}

ModulesManager::ModulesManager() : QObject()
{
	// wazne, aby inicjalizacja tej zmiennej wystapila przed wczytaniem
	// modulow, bo niektore juz moga z niej korzystac, wiec dlatego
	// przenioslem to tutaj z funkcji ModulesManagert::initModule
	// to samo z menu - moduly powinny sie ladowac na samym koncu
	modules_manager=this;
	kadu->mainMenu()->insertItem(icons_manager.loadIcon("ManageModules"), tr("&Manage Modules"), this, SLOT(showDialog()), QKeySequence(), -1, 2);
	//
	Dialog=NULL;
	//
	registerStaticModules();
	QStringList static_list=staticModules();
	for (QStringList::ConstIterator i=static_list.begin(); i!=static_list.end(); i++)
		if(!moduleIsActive(*i))
			activateModule(*i);
	//
	QString loaded_str=config_file.readEntry("General", "LoadedModules");
	QStringList loaded_list=QStringList::split(',',loaded_str);
	bool all_loaded=true;
	for(QStringList::ConstIterator i=loaded_list.begin(); i!=loaded_list.end(); i++)
		if(!moduleIsActive(*i))
			if(!activateModule(*i))
				all_loaded=false;
	// jesli nie wszystkie moduly zostaly przy starcie prawidlowo
	// zaladowane to zapisz nowa liste zaladowanych modulow
	if(!all_loaded)
		saveLoadedModules();
}

ModulesManager::~ModulesManager()
{
	QStringList active=activeModules();
	for(QStringList::ConstIterator i=active.begin(); i!=active.end(); i++)
		deactivateModule(*i, true);
}

QTranslator* ModulesManager::loadModuleTranslation(const QString& module_name)
{
	QTranslator* translator=new QTranslator(0);
	if(translator->load(dataPath("kadu/modules/translations/"+module_name+QString("_") + config_file.readEntry("General", "Language", QTextCodec::locale())), "."))
	{
		qApp->installTranslator(translator);
		return translator;
	}
	else
	{
		delete translator;
		return NULL;
	}
}

bool ModulesManager::satisfyModuleDependencies(const ModuleInfo& module_info)
{
	for (QStringList::ConstIterator it = module_info.depends.begin(); it != module_info.depends.end(); ++it)
	{
		if(!moduleIsActive(*it))
		{
			if(moduleIsInstalled(*it))
			{
				if(!activateModule(*it))
					return false;
			}			
			else
			{
				MessageBox::msg(tr("Required module %1 was not found").arg(*it));
				return false;
			}
		}
	}
	return true;
}

void ModulesManager::incDependenciesUsageCount(const ModuleInfo& module_info)
{
	for (QStringList::ConstIterator it = module_info.depends.begin(); it != module_info.depends.end(); ++it)
		if(!moduleIsStatic(*it))
			moduleIncUsageCount(*it);
}

void ModulesManager::registerStaticModule(const QString& module_name,
	InitModuleFunc* init,CloseModuleFunc* close)
{
	StaticModule m;
	m.init=init;
	m.close=close;
	StaticModules.insert(module_name,m);
}

QStringList ModulesManager::staticModules()
{
	QStringList static_modules;
	for (QMap<QString,StaticModule>::const_iterator i=StaticModules.begin(); i!=StaticModules.end(); i++)
		static_modules.append(i.key());
	return static_modules;
}

QStringList ModulesManager::installedModules()
{
	QDir dir(dataPath("kadu/modules"),"*.so");
	dir.setFilter(QDir::Files);
	QStringList installed;
	for(int i=0; i<dir.count(); i++)
	{
		QString name=dir[i];
		installed.append(name.left(name.length()-3));
	}
	return installed;
}

QStringList ModulesManager::loadedModules()
{
	QStringList loaded;
	for (QMap<QString,Module>::const_iterator i=Modules.begin(); i!=Modules.end(); i++)
		if(i.data().lib!=NULL)
			loaded.append(i.key());
	return loaded;
}

QStringList ModulesManager::unloadedModules()
{
	QStringList installed=installedModules();
	QStringList loaded=loadedModules();
	QStringList unloaded;
	for(int i=0; i<installed.size(); i++)
	{
		QString name=installed[i];
		if(!loaded.contains(name))
			unloaded.append(name);
	}
	return unloaded;
}

QStringList ModulesManager::activeModules()
{
	QStringList active;
	for (QMap<QString,Module>::const_iterator i=Modules.begin(); i!=Modules.end(); i++)
		active.append(i.key());
	return active;
}

bool ModulesManager::moduleInfo(const QString& module_name, ModuleInfo& info)
{
	if(Modules.contains(module_name))
	{
		info=Modules[module_name].info;
		return true;
	}

	ConfigFile desc_file(dataPath("kadu/modules/"+module_name+".desc"));

	QString lang=config_file.readEntry("General", "Language", "en");

	info.description = desc_file.readEntry("Module", "Description["+lang+"]");
	if(info.description.isEmpty())
		info.description = desc_file.readEntry("Module", "Description");

	info.author = desc_file.readEntry("Module", "Author");

	info.depends = QStringList::split(" ",
		desc_file.readEntry("Module", "Dependencies"));

	return true;
}

bool ModulesManager::moduleIsStatic(const QString& module_name)
{
	return staticModules().contains(module_name);
}

bool ModulesManager::moduleIsInstalled(const QString& module_name)
{
	return installedModules().contains(module_name);
}

bool ModulesManager::moduleIsLoaded(const QString& module_name)
{
	return loadedModules().contains(module_name);
}

bool ModulesManager::moduleIsActive(const QString& module_name)
{
	return Modules.contains(module_name);
}

void ModulesManager::saveLoadedModules()
{
	config_file.writeEntry("General", "LoadedModules",loadedModules().join(","));
	config_file.sync();
}

bool ModulesManager::activateModule(const QString& module_name)
{
	Module m;
	kdebug(QString("activateModule %1\n").arg(module_name));
	
	if(moduleIsActive(module_name))
	{
		MessageBox::msg(tr("Module %1 is already active").arg(module_name));
		return false;
	}

	if(moduleInfo(module_name,m.info))
		if(!satisfyModuleDependencies(m.info))
			return false;

	typedef int InitModuleFunc();
	InitModuleFunc* init;
	
	if(moduleIsStatic(module_name))
	{
		m.lib=NULL;
		StaticModule sm=StaticModules[module_name];
		init=sm.init;
		m.close=sm.close;
	}
	else
	{
		m.lib=new Library(dataPath("kadu/modules/"+module_name+".so"));
		if(!m.lib->load())
		{
			MessageBox::msg(tr("Cannot load %1 module library.:\n%2").arg(module_name).arg(m.lib->error()));
			return false;
		}	
		init=(InitModuleFunc*)m.lib->resolve(module_name+"_init");
		m.close=(CloseModuleFunc*)m.lib->resolve(module_name+"_close");
		if(init==NULL||m.close==NULL)
		{
			MessageBox::msg(tr("Cannot find required functions.\nMaybe it's not Kadu-compatible Module."));
			delete m.lib;
			return false;
		}
	}
	
	m.translator = loadModuleTranslation(module_name);

	int res=init();
	if(res!=0)
	{
		MessageBox::msg(tr("Module initialization routine failed."));
		if(m.lib!=NULL)
			delete m.lib;
		return false;		
	}
	
	incDependenciesUsageCount(m.info);
	
	m.usage_counter=0;
	Modules.insert(module_name,m);
	return true;
}

bool ModulesManager::deactivateModule(const QString& module_name, bool force)
{
	Module m=Modules[module_name];

	if(m.usage_counter>0 && !force)
	{
		MessageBox::msg(tr("Module %1 cannot be deactivated because it is used now").arg(module_name));
		return false;
	}
	
	for (QStringList::Iterator i = m.info.depends.begin(); i != m.info.depends.end(); i++)
		moduleDecUsageCount(*i);
	
	m.close();
	if(m.translator!=NULL)
	{
		qApp->removeTranslator(m.translator);
		delete m.translator;
	}
	
	if(m.lib!=NULL)
		delete m.lib;
	Modules.remove(module_name);
}

void ModulesManager::showDialog()
{
	if(Dialog==NULL)
	{
		Dialog=new ModulesDialog();
		connect(Dialog,SIGNAL(destroyed()),this,SLOT(dialogDestroyed()));
		Dialog->show();
	}
	else
		Dialog->setActiveWindow();
}

void ModulesManager::dialogDestroyed()
{
	kdebugf();
	Dialog=NULL;
}

void ModulesManager::moduleIncUsageCount(const QString& module_name)
{
	Modules[module_name].usage_counter++;	
}

void ModulesManager::moduleDecUsageCount(const QString& module_name)
{
	Modules[module_name].usage_counter--;
}

ModulesManager* modules_manager;

