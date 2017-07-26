/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "g19daemon.hpp"
#include "plugininterface.hpp"
#include "ui_g19daemon.h"
#include "gscreen.hpp"
#include "config.h"
#include <QFont>
#include <QPainter>
#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QLinearGradient>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>
#include <QWidget>
#include <QMainWindow>
#include <QtMath>
#include <QtCore/QSharedMemory>
#include <QErrorMessage>

g19daemon::g19daemon(QWidget *parent):
    QMainWindow(parent),
	ui(new Ui::g19daemon)
{
	QImage micon;
	QColor BackLight;
    
	ui->setupUi(this);

	qRegisterMetaType<gAction>("gAction");
	
	device = new G19Device;
	device->initializeDevice();
	device->openDevice();

	settings =  new QSettings("Dynamite", "G19Daemon");
	
	activePlugin = -1;
	isActive = true;
	
	connect(device, SIGNAL(GKey()), SLOT(GKeys()));
	connect(device, SIGNAL(LKey()), SLOT(LKeys()));

	micon = QImage(":/menu_icon.png");
	menuScreen = new gScreen(micon, tr("Logitech G19s Linux"));
	menuSelect = 0;
	
	loadPlugins();
	
	if (activePlugin != -1)
		menuActive = false;
	else
		menuActive = true;
	
	BackLight.setRed(settings->value("KeyBacklight_Red", "255").toInt());
	BackLight.setGreen(settings->value("KeyBacklight_Green", "255").toInt());
	BackLight.setBlue(settings->value("KeyBacklight_Blue", "255").toInt());
	
	device->setKeysBacklight(BackLight);
	device->setDisplayBrightness(settings->value("Backlight", "255").toInt());
	device->setMKeys(true, false, false, false);

	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(tr("Show"), this, SLOT(Show()));
	trayIconMenu->addAction(tr("Reset LCD Backlight"), this, SLOT(ResetLcdBacklight()));
	trayIconMenu->addAction(QIcon(":/off.png"), "&Quit", this, SLOT(quit()));
	

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Logitech G19 Daemon");
	trayIcon->setIcon(QIcon(":/tray_icon.png"));
	trayIcon->show();
}

g19daemon::~g19daemon()
{
	if (menuActive)
		settings->setValue("ActivePlugin", "menu");
	else if (activePlugin != -1)
        settings->setValue("ActivePlugin", plugins[activePlugin]->getName());
	
    settings->sync();

	unloadPlugins();
	
	delete settings;

	device->closeDevice();
	delete device;
}

// call this routine to quit the application
void g19daemon::quit()
{
	// you can do some cleanup here
    // then do emit finished to signal CoreApplication to quit

	disconnect(device, SIGNAL(GKey()), this, SLOT(GKeys()));
	disconnect(device, SIGNAL(LKey()), this, SLOT(LKeys()));

	emit finished();
}

// shortly after quit is called the CoreApplication will signal this routine
// this is a good place to delete any objects that were created in the
// constructor and/or to stop any threads
void g19daemon::aboutToQuitApp()
{
}

void g19daemon::Show()
{
//	configdialog *dlg;
	
//	dlg = new configdialog();
//	dlg->show();
	show();
}

void g19daemon::ResetLcdBacklight()
{
	device->setDisplayBrightness(255);
	settings->setValue("Backlight", 255);
	
}

void g19daemon::run()
{
	if (menuActive)
		menu();
	else
		plugins[activePlugin]->setActive(true);
}

void g19daemon::GKeys()
{
	unsigned int keys;
	
	keys = device->getKeys();
	
	if (keys & G19_KEY_M1)
	{
		device->setMKeys(true, false, false, false);
	}

	if (keys & G19_KEY_M2)
	{
		device->setMKeys(false, true, false, false);
	}

	if (keys & G19_KEY_M3)
	{
		device->setMKeys(false, false, true, false);
	}

	if (keys & G19_KEY_MR)
	{
		device->setMKeys(false, false, false, true);
	}
}

void g19daemon::LKeys()
{
	unsigned int keys;
	
	keys = device->getKeys();
	
	if (keys & G19_KEY_LHOME)
	{
		if (menuActive == false)
		{
			plugins[activePlugin]->setActive(false);
			menuActive = true;
		}
		menu();
	}
	
	if (menuActive)
	{
		if (keys & G19_KEY_LDOWN)
		{
			menuSelect++;
			
			if (menuSelect > plugins.count() - 1)
				menuSelect = 0;
		
			menu();
		}
		
		if (keys & G19_KEY_LUP)
		{
			menuSelect--;

			if (menuSelect < 0)
				menuSelect = plugins.count() - 1;

			menu();
		}
		
		if (keys & G19_KEY_LOK)
		{
			menuActive = false;
			activePlugin = menuSelect;
			plugins[activePlugin]->setActive(true);
		}
	}
	else if (plugins[activePlugin])
	{
		plugins[activePlugin]->LKeys(keys);
	}
}

void g19daemon::menu()
{
	QPainter *p;
	QString name;
	int y = 0;
	int menuStart, menuEnd, pages, page;

	p = menuScreen->Begin();
	// How many pages will there be
	pages = qCeil(plugins.count() / 3);

	page = menuSelect / 3;
	// Calculate the offset for the query

	// Some information to display to the user
	menuStart = page * 3;
	menuEnd = min(menuStart + 3, plugins.count() - 1);
	
	for (int i = menuStart; i <= menuEnd ; i++)
	{
		name = plugins[i]->getName();

		if (menuSelect == i)
		{
			p->fillRect(0, y, 304, 68, qRgb(0, 165, 219));
            p->setPen(qRgb(0, 0, 0));
		}
		else
        {
            p->setPen(qRgb(255, 255, 255));
        }

		p->drawImage(3, y + 10, plugins[i]->getIcon());
		p->drawText(59, y, 245, 68, Qt::AlignVCenter | Qt::AlignLeft, name);
		y += 68;
	}
	
	// Scroll Bar
	menuScreen->DrawScrollbar(page, pages + 1);
	menuScreen->End();
	device->updateLcd(menuScreen->Draw());

}

void g19daemon::loadPlugins()
{
	PluginInterface *pluginint;
	QDir pluginsDir(qApp->applicationDirPath());
	QString name;
	
	name = settings->value("ActivePlugin").toString();
	pluginsDir.cd(PLUGINS_DIR);
	QDirIterator iterator(pluginsDir.absolutePath(), QStringList() << "*.so", QDir::Files, QDirIterator::NoIteratorFlags);

	while (iterator.hasNext())
	{
		iterator.next();
		QString filename = iterator.fileName();
		
		QPluginLoader pluginLoader(iterator.filePath());
		pluginLoader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
		QObject *plugin = pluginLoader.instance();
		
		if (plugin)
		{
			pluginint = qobject_cast<PluginInterface *>(plugin);
			
			if (pluginint)
			{
				QObject *signalsource = pluginint->getQObject();
				connect(signalsource, SIGNAL(doAction(gAction, void *)), this, SLOT(doAction(gAction, void *)));

				if (pluginint->isPopup())
				{
					PopupPlugins.append(pluginint);
				}
				else
				{
					plugins.append(pluginint);

					if (pluginint->getName().compare(name) == 0)
					{
						activePlugin = plugins.indexOf(pluginint);
					}
				}
			}
		}
		else
		{
			QErrorMessage err(this);
			err.showMessage(pluginLoader.errorString());
			qDebug() << "\tError: " << pluginLoader.errorString();
		}
	}
}

void g19daemon::unloadPlugins()
{
	PluginInterface *pl;

	while (plugins.count())
	{
		pl = plugins[0];
		plugins.remove(0);
		if (pl)
		{
			delete pl;
		}
	}

	while (PopupPlugins.count())
	{
		pl = PopupPlugins[0];
		PopupPlugins.remove(0);
		if (pl)
		{
			delete pl;
		}
	}

}

void g19daemon::doAction(gAction action, void *data)
{
	int b;
	
	switch (action)
	{
		case displayScreen:
			device->updateLcd(((gScreen *) data)->Draw());
			break;
		case setKeyBackground:
			device->setKeysBacklight(*((QColor *) data));
			break;
		case grabFocus:
			if (!menuActive)
				plugins[activePlugin]->setActive(false);
			
			break;
		case releaseFocus:
			if (!menuActive)
				plugins[activePlugin]->setActive(true);
			else
				menu();
			break;
		case setLcdBrightness:
			b = *((int *) data);
			device->setDisplayBrightness(b);
			break;
        case restoreKeyBackground:
            QColor BackLight;
            BackLight.setRed(settings->value("KeyBacklight_Red", "255").toInt());
            BackLight.setGreen(settings->value("KeyBacklight_Green", "255").toInt());
            BackLight.setBlue(settings->value("KeyBacklight_Blue", "255").toInt());
            device->setKeysBacklight(BackLight);
            break;
	}
	
}
