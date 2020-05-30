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

#ifndef g19daemon_H
#define g19daemon_H

#include <QtCore/QObject>
#include <QSettings>
#include <QLinearGradient>
#include <QFont>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>
#include <QWidget>
#include <QProcess>
#include <QMainWindow>
#include <QLineEdit>
#include "g19device.hpp"
#include "plugininterface.hpp"
#include "gscreen.hpp"

namespace Ui
{
	class g19daemon;
}

class g19daemon : public QMainWindow
{
	Q_OBJECT
	public:
		explicit g19daemon(QWidget *parent = 0);
		~g19daemon();

		void menu();
        void menuSettings();

	signals:
		// Signal to finish, this is connected to Application Quit
		void finished();

	public slots:
		// This is the slot that gets called from main to start everything
		// but, everthing is set up in the Constructor
		void run();

		// slot that get signal when that application is about to quit
		void aboutToQuitApp();
		void quit();

		void Show();
		void ResetLcdBacklight();
		void GKeys();
		void LKeys();
        void saveSettings(); 
		
		void doAction(gAction, void *data);
		
	private:
		Ui::g19daemon *ui;
        
		G19Device *device;
		QVector<PluginInterface *> plugins;
		QVector<PluginInterface *> PopupPlugins;
        QList<G19Keys> pressedKey;
		int activePlugin;
		int prevPlugin;
		
		int isActive;
		int menuSelect;
		bool menuActive;
        bool menuSettingsActive;
		
		gScreen *menuScreen;
		
		QMenu *trayIconMenu;
		QSystemTrayIcon *trayIcon;
		QAction *quitAction;
		QAction *showAction;
		
		QSettings *settings;
		
        QVector<PluginInterface *> getActivePlugins();
		void loadPlugins();
		void unloadPlugins();
        QString translateKey(G19Keys);
        void loadSettings();
		
	private slots:
};

#endif // g19daemon_H
