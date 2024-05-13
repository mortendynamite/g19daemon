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
#include "config.h"
#include "gscreen.hpp"
#include "plugininterface.hpp"
#include "ui_g19daemon.h"
#include <QAction>
#include <QDir>
#include <QErrorMessage>
#include <QFile>
#include <QFont>
#include <QLinearGradient>
#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QPluginLoader>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QWidget>
#include <QtCore/QSharedMemory>
#include <QtMath>

G19daemon::G19daemon(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::G19daemon) {
    QImage micon;
    QColor BackLight;

    ui->setupUi(this);

    qRegisterMetaType<gAction>("gAction");

    device = new G19Device;
    device->initialize();

    settings = new QSettings("G19Daemon", "G19Daemon");

    activePlugin = nullptr;
    isActive = true;
    menuSettingsActive = false;
    unsavedSettings = false;

    connect(device, SIGNAL(gKey()), SLOT(gKeys()));
    connect(device, SIGNAL(lKey()), SLOT(lKeys()));

    connect(ui->actionSave_settings, SIGNAL(triggered()), SLOT(saveSettings()));

    connect(ui->m1BackgroundColorButton, SIGNAL(clicked()), SLOT(changeBackgroundColor()));
    connect(ui->m2BackgroundColorButton, SIGNAL(clicked()), SLOT(changeBackgroundColor()));
    connect(ui->m3BackgroundColorButton, SIGNAL(clicked()), SLOT(changeBackgroundColor()));
    connect(ui->mrBackgroundColorButton, SIGNAL(clicked()), SLOT(changeBackgroundColor()));
    connect(ui->actionDisable_plugin_profile, SIGNAL(changed()), SLOT(disablePluginProfile()));

    connect(ui->mKeyTabWidget, SIGNAL(currentChanged(int)), SLOT(swithProfile(int)));

    ui->actionDisable_plugin_profile->setIcon(QIcon("://plugin.png"));
    micon = QImage(":/menu_icon.png");
    menuScreen = new Gscreen(micon, tr("Logitech G19s Linux"));
    menuSelect = 0;

    loadPlugins();

    if (activePlugin != nullptr)
        menuActive = false;
    else
        menuActive = true;

    device->setMKeys(true, false, false, false);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(tr("Show"), this, SLOT(show()));
    trayIconMenu->addAction(tr("Reset LCD Backlight"), this,
                            SLOT(resetLcdBacklight()));
    trayIconMenu->addAction(QIcon(":/off.png"), "&Quit", this, SLOT(quit()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Logitech G19 Daemon");
    trayIcon->setIcon(QIcon(":/g19daemon.png"));
    trayIcon->show();

    loadPluginsIntoMenubar();
    loadSettings();
    disablePluginProfile();
    sendProfileToPlugins(G19_KEY_M1);
}

G19daemon::~G19daemon() {
    if (menuActive)
        settings->setValue("ActivePlugin", "menu");
    else if (activePlugin != nullptr)
        settings->setValue("ActivePlugin", activePlugin->getName());

    settings->sync();

    unloadPlugins();

    delete settings;

    device->uninitialize();
    delete device;
}

// call this routine to quit the application
void G19daemon::quit() {
    // you can do some cleanup here
    // then do emit finished to signal CoreApplication to quit

    disconnect(device, SIGNAL(gKey()), this, SLOT(gKeys()));
    disconnect(device, SIGNAL(lKey()), this, SLOT(lKeys()));

    emit finished();
}

// shortly after quit is called the CoreApplication will signal this routine
// this is a good place to delete any objects that were created in the
// constructor and/or to stop any threads
void G19daemon::aboutToQuitApp() {}

void G19daemon::loadSettings() {

    ui->actionDisable_plugin_profile->setChecked(
            settings->value(ui->actionDisable_plugin_profile->objectName(), false).toBool());

    for (QLineEdit *lineEdit: ui->mKeyTabWidget->findChildren<QLineEdit *>()) {

        lineEdit->setText(settings->value(lineEdit->objectName()).toString());
    }

    for (QPushButton *button: {ui->m1BackgroundColorButton, ui->m2BackgroundColorButton, ui->m3BackgroundColorButton,
                               ui->mrBackgroundColorButton}) {
        QPalette palette = button->palette();

        QColor color = settings->value(button->objectName(), qRgb(183, 184, 187)).value<QColor>();

        if (color.isValid()) {
            palette.setColor(QPalette::Button, color);

            button->setPalette(palette);

            if (button == ui->m1BackgroundColorButton) {
                this->device->setKeysBacklight(color);
            }
        }
    }

    for (QComboBox *button: {ui->m1DefaultPlugin, ui->m2DefaultPlugin, ui->m3DefaultPlugin, ui->mrDefaultPlugin}) {
        button->addItem("");
        for (PluginInterface *plugin: plugins) {
            button->addItem(plugin->getName());
        }

        QString defaultPlugin = settings->value(button->objectName()).toString();

        if (!defaultPlugin.isEmpty()) {
            button->setCurrentText(defaultPlugin);
        }

    }

    for (QSpinBox *button: {ui->m1Brightness, ui->m2Brightness, ui->m3Brightness, ui->mrBrightness}) {
        button->setValue(settings->value(button->objectName(), 255).toInt());
    }

}

void G19daemon::Show() {
    for (QLineEdit *lineEdit: ui->mKeyTabWidget->findChildren<QLineEdit *>()) {

        lineEdit->setText(settings->value(lineEdit->objectName()).toString());
    }
}

void G19daemon::saveSettings() {

    qDebug() << "Save Settings";

    settings->setValue(ui->actionDisable_plugin_profile->objectName(), ui->actionDisable_plugin_profile->isChecked());

    for (QLineEdit *lineEdit: ui->mKeyTabWidget->findChildren<QLineEdit *>()) {

        settings->setValue(lineEdit->objectName(), lineEdit->text());
    }

    for (QPushButton *button: {ui->m1BackgroundColorButton, ui->m2BackgroundColorButton, ui->m3BackgroundColorButton,
                               ui->mrBackgroundColorButton}) {
        settings->setValue(button->objectName(), button->palette().color(QPalette::Button));
    }

    for (QComboBox *button: {ui->m1DefaultPlugin, ui->m2DefaultPlugin, ui->m3DefaultPlugin, ui->mrDefaultPlugin}) {
        settings->setValue(button->objectName(), button->currentText());
    }

    for (QSpinBox *button: {ui->m1Brightness, ui->m2Brightness, ui->m3Brightness, ui->mrBrightness}) {
        settings->setValue(button->objectName(), button->value());
    }

    unsavedSettings = false;
}

void G19daemon::resetLcdBacklight() {
    device->setDisplayBrightness(127);

    settings->setValue("m1Brightness", 127);
    settings->setValue("m2Brightness", 127);
    settings->setValue("m3Brightness", 127);
    settings->setValue("mrBrightness", 127);
}

void G19daemon::run() {
    if (menuActive) {
        menu();
    } else if (activePlugin != nullptr) {
        activePlugin->setActive(true);
    }
}

void G19daemon::gKeys() {
    unsigned int keys;

    keys = device->getKeys();

    if (keys & G19_KEY_M1) {
        swithProfile(0);
    } else if (keys & G19_KEY_M2) {
        swithProfile(1);
    } else if (keys & G19_KEY_M3) {
        swithProfile(2);

    } else if (keys & G19_KEY_MR) {
        swithProfile(3);
    } else {
        QString gKey = translateKey((G19Keys) keys);

        if (!gKey.isEmpty()) {
            QString command =
                    settings->value(translateKey(device->getActiveMKey()) + "_" + gKey)
                            .toString();
            qDebug() << translateKey(device->getActiveMKey()) << ":" << gKey << " Pressed, Run command: " << command;
            QProcess::startDetached(command);
        }
    }
}

QString G19daemon::translateKey(G19Keys keys) {
    if (keys & G19_KEY_G1) {
        return QString("G1");
    } else if (keys & G19_KEY_G2) {
        return QString("G2");
    } else if (keys & G19_KEY_G3) {
        return QString("G3");
    } else if (keys & G19_KEY_G4) {
        return QString("G4");
    } else if (keys & G19_KEY_G5) {
        return QString("G5");
    } else if (keys & G19_KEY_G6) {
        return QString("G6");
    } else if (keys & G19_KEY_G7) {
        return QString("G7");
    } else if (keys & G19_KEY_G8) {
        return QString("G8");
    } else if (keys & G19_KEY_G9) {
        return QString("G9");
    } else if (keys & G19_KEY_G10) {
        return QString("G10");
    } else if (keys & G19_KEY_G11) {
        return QString("G11");
    } else if (keys & G19_KEY_G12) {
        return QString("G12");
    } else if (keys & G19_KEY_M1) {
        return QString("m1");
    } else if (keys & G19_KEY_M2) {
        return QString("m2");
    } else if (keys & G19_KEY_M3) {
        return QString("m3");
    } else if (keys & G19_KEY_MR) {
        return QString("mr");
    }

    return QString("");
}

void G19daemon::lKeys() {
    unsigned int keys;

    keys = device->getKeys();

    QVector<PluginInterface *> activePlugins = getActivePlugins();

    int pluginCount = activePlugins.count();

    if (menuSettingsActive) {
        pluginCount = plugins.count();
    }

    if (keys & G19_KEY_LHOME) {
        if (menuActive == false) {
            activePlugin->setActive(false);
            menuActive = true;
            menuSettingsActive = false;
        }
        menu();
    } else if (keys & G19_KEY_LMENU && menuActive) {
        menuActive = false;
        menuSettingsActive = true;
        menuSettings();
    } else if ((keys & G19_KEY_LMENU || keys & G19_KEY_LCANCEL) &&
               menuSettingsActive) {
        menuActive = true;
        menuSettingsActive = false;
        menuSelect = 0;
        menu();
    }

    if (menuActive || menuSettingsActive) {
        if (keys & G19_KEY_LDOWN) {
            menuSelect++;

            if (menuSelect > pluginCount - 1)
                menuSelect = 0;

            if (menuSettingsActive) {
                menuSettings();
            } else {
                menu();
            }
        }

        if (keys & G19_KEY_LUP) {
            menuSelect--;

            if (menuSelect < 0)
                menuSelect = pluginCount - 1;

            if (menuSettingsActive) {
                menuSettings();
            } else {
                menu();
            }
        }

        if (keys & G19_KEY_LOK && menuActive) {
            menuActive = false;
            activePlugin = activePlugins[menuSelect];

            deactiveAllPlugins();

            activePlugin->setActive(true);
        } else if (keys & G19_KEY_LOK && menuSettingsActive) {
            bool currentPluginEnabled =
                    settings->value(plugins[menuSelect]->getName() + "-enabled", true)
                            .toBool();

            settings->setValue(plugins[menuSelect]->getName() + "-enabled",
                               !currentPluginEnabled);

            if (getActivePlugins().isEmpty()) {
                settings->setValue(plugins[menuSelect]->getName() + "-enabled", true);
            }

            menuSettings();
        }
    } else if (activePlugin != nullptr) {
        activePlugin->lKeys(keys);
    }
}

void G19daemon::deactiveAllPlugins() {
    for (int i = 0; i < plugins.size(); i++) {
        plugins[i]->setActive(false);
    }
}

void G19daemon::menu() {
    QPainter *p;
    QString name;
    int y = 0;
    int menuStart, menuEnd, pages, page;

    QVector<PluginInterface *> activePlugins = getActivePlugins();

    p = menuScreen->begin();
    // How many pages will there be
    pages = qCeil(activePlugins.count() / 3);

    page = menuSelect / 3;
    // Calculate the offset for the query

    // Some information to display to the user
    menuStart = page * 3;
    menuEnd = min(qsizetype(menuStart + 3), activePlugins.count() - 1);

    for (int i = menuStart; i <= menuEnd; i++) {
        name = activePlugins[i]->getName();

        if (menuSelect == i) {
            p->fillRect(0, y, 304, 68, qRgb(0, 165, 219));
            p->setPen(qRgb(0, 0, 0));
        } else {
            p->setPen(qRgb(255, 255, 255));
        }

        p->drawImage(3, y + 10, activePlugins[i]->getIcon());
        p->drawText(59, y, 245, 68, Qt::AlignVCenter | Qt::AlignLeft, name);
        y += 68;
    }

    // Scroll Bar
    menuScreen->drawScrollbar(page, pages + 1);
    menuScreen->end();
    device->updateLcd(menuScreen->draw());
}

void G19daemon::menuSettings() {
    QPainter *p;
    QString name;
    int y = 0;
    int menuStart, menuEnd, pages, page;

    p = menuScreen->begin();
    // How many pages will there be
    pages = qCeil(plugins.count() / 3);

    page = menuSelect / 3;
    // Calculate the offset for the query

    // Some information to display to the user
    menuStart = page * 3;
    menuEnd = min(qsizetype(menuStart + 3), plugins.count() - 1);

    for (int i = menuStart; i <= menuEnd; i++) {
        name = plugins[i]->getName();

        if (menuSelect == i) {
            p->fillRect(0, y, 304, 68, qRgb(0, 165, 219));
            p->setPen(qRgb(0, 0, 0));
        } else {
            p->setPen(qRgb(255, 255, 255));
        }

        if (settings->value(plugins[i]->getName() + "-enabled", true).toBool()) {
            p->drawPixmap(3, y + 10, QIcon(":/checked.png").pixmap(48, 48));
        } else {
            p->drawPixmap(3, y + 10, QIcon(":/unchecked.png").pixmap(48, 48));
        }

        p->drawText(59, y, 245, 68, Qt::AlignVCenter | Qt::AlignLeft, name);
        y += 68;
    }

    // Scroll Bar
    menuScreen->drawScrollbar(page, pages + 1);
    menuScreen->end();
    device->updateLcd(menuScreen->draw());
}

QVector<PluginInterface *> G19daemon::getActivePlugins() {
    QVector<PluginInterface *> activePlugins;

    for (int i = 0; i < plugins.count(); i++) {
        if (settings->value(plugins[i]->getName() + "-enabled", true).toBool()) {
            activePlugins.append(plugins[i]);
        }
    }

    return activePlugins;
}

void G19daemon::loadPlugins() {
    PluginInterface *pluginint;
    QDir pluginsDir(qApp->applicationDirPath());
    QString name;

    name = settings->value("ActivePlugin").toString();
    pluginsDir.cd(PLUGINS_DIR);
    QDirIterator iterator(pluginsDir.absolutePath(), QStringList() << "*.so",
                          QDir::Files, QDirIterator::NoIteratorFlags);

    while (iterator.hasNext()) {
        iterator.next();
        QString filename = iterator.fileName();

        QPluginLoader pluginLoader(iterator.filePath());
        pluginLoader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
        QObject *plugin = pluginLoader.instance();

        if (plugin) {
            pluginint = qobject_cast<PluginInterface *>(plugin);

            if (pluginint) {
                QObject *signalsource = pluginint->getQObject();
                connect(signalsource, SIGNAL(doAction(gAction, void * )), this,
                        SLOT(doAction(gAction, void * )));

                if (pluginint->isPopup()) {
                    PopupPlugins.append(pluginint);
                } else {
                    plugins.append(pluginint);

                    if (pluginint->getName().compare(name) == 0) {
                        activePlugin =
                                getActivePlugins()[getActivePlugins().indexOf(pluginint)];
                    }
                }
            }
        } else {
            QErrorMessage err(this);
            err.showMessage(pluginLoader.errorString());
            qDebug() << "\tError: " << pluginLoader.errorString();
        }
    }
}

void G19daemon::unloadPlugins() {
    PluginInterface *pl;

    while (plugins.count()) {
        pl = plugins[0];
        plugins.remove(0);
        if (pl) {
            delete pl;
        }
    }

    while (PopupPlugins.count()) {
        pl = PopupPlugins[0];
        PopupPlugins.remove(0);
        if (pl) {
            delete pl;
        }
    }
}

void G19daemon::doAction(gAction action, void *data) {
    int b;

    switch (action) {
        case displayFullScreen:
            device->updateLcd(((Gscreen *) data)->drawFullScreen());

            break;
        case displayScreen:
            device->updateLcd(((Gscreen *) data)->draw());
            break;
        case setKeyBackground:
            device->setKeysBacklight(*((QColor *) data));
            break;
        case grabFocus:
            if (!menuActive)
                deactiveAllPlugins();
            break;
        case releaseFocus:
            if (!menuActive) {
                deactiveAllPlugins();
                activePlugin->setActive(true);
            } else
                menu();
            break;
        case setLcdBrightness:
            b = *((int *) data);
            device->setDisplayBrightness(b);
            break;
        case restoreKeyBackground:
            QColor BackLight = qvariant_cast<QColor>(settings->value(ui->m1BackgroundColorButton->objectName()));

            device->setKeysBacklight(BackLight);
            break;
    }
}


void G19daemon::changeBackgroundColor() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    QPalette palette = button->palette();

    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(palette.color(QPalette::Button));

    connect(
            dialog, SIGNAL(currentColorChanged(const QColor&)),
            device, SLOT(changeKeysBacklight(const QColor&)));

    if (dialog->exec() == QColorDialog::Accepted) {
        QColor color = dialog->currentColor();

        if (color.isValid()) {
            palette.setColor(QPalette::Button, color);

            button->setPalette(palette);

            unsavedSettings = true;
        }
    } else {
        device->setKeysBacklight(palette.color(QPalette::Button));
    }
}

void G19daemon::disablePluginProfile() {
    for (QComboBox *button: {ui->m1DefaultPlugin, ui->m2DefaultPlugin, ui->m3DefaultPlugin, ui->mrDefaultPlugin}) {
        button->setEnabled(ui->actionDisable_plugin_profile->isChecked());
    }

}

void G19daemon::sendProfileToPlugins(G19Keys key) {

    for (int i = 0; i < plugins.size(); i++) {
        plugins[i]->mKeys(key);
    }

}

void G19daemon::swithProfile(int index) {
    ui->mKeyTabWidget->setCurrentWidget(ui->mKeyTabWidget->widget(index));

    if (index == 0) {

        device->setMKeys(true, false, false, false);

        if (unsavedSettings) {
            device->setKeysBacklight(ui->m1BackgroundColorButton->palette().color(QPalette::Button));
        } else {
            device->setDisplayBrightness(settings->value(ui->m1Brightness->objectName(), 255).toInt());
            device->setKeysBacklight(
                    settings->value(ui->m1BackgroundColorButton->objectName(), qRgb(183, 184, 187)).value<QColor>());

            switchActivePlugin(G19Keys::G19_KEY_M1);
        }
    } else if (index == 1) {
        device->setMKeys(false, true, false, false);

        if (unsavedSettings) {
            device->setKeysBacklight(ui->m2BackgroundColorButton->palette().color(QPalette::Button));
        } else {
            device->setDisplayBrightness(settings->value(ui->m2Brightness->objectName(), 255).toInt());
            device->setKeysBacklight(
                    settings->value(ui->m2BackgroundColorButton->objectName(), qRgb(183, 184, 187)).value<QColor>());
            switchActivePlugin(G19Keys::G19_KEY_M2);
        }
    } else if (index == 2) {
        device->setMKeys(false, false, true, false);

        if (unsavedSettings) {
            device->setKeysBacklight(ui->m3BackgroundColorButton->palette().color(QPalette::Button));
        } else {
            device->setDisplayBrightness(settings->value(ui->m3Brightness->objectName(), 255).toInt());
            device->setKeysBacklight(
                    settings->value(ui->m3BackgroundColorButton->objectName(), qRgb(183, 184, 187)).value<QColor>());
            switchActivePlugin(G19Keys::G19_KEY_M3);
        }
    } else if (index == 3) {
        device->setMKeys(false, false, false, true);

        if (unsavedSettings) {
            device->setKeysBacklight(ui->mrBackgroundColorButton->palette().color(QPalette::Button));
        } else {
            device->setDisplayBrightness(settings->value(ui->mrBrightness->objectName(), 255).toInt());
            device->setKeysBacklight(
                    settings->value(ui->mrBackgroundColorButton->objectName(), qRgb(183, 184, 187)).value<QColor>());
            switchActivePlugin(G19Keys::G19_KEY_MR);
        }
    }

}

void G19daemon::switchActivePlugin(G19Keys key) {
    sendProfileToPlugins(key);
    QString defaultPlugin = "";

    switch (key) {
        case G19_KEY_M1:
            defaultPlugin = settings->value(ui->m1DefaultPlugin->objectName(), "").toString();
            break;

        case G19_KEY_M2:
            defaultPlugin = settings->value(ui->m2DefaultPlugin->objectName(), "").toString();
            break;

        case G19_KEY_M3:
            defaultPlugin = settings->value(ui->m3DefaultPlugin->objectName(), "").toString();
            break;

        case G19_KEY_MR:
            defaultPlugin = settings->value(ui->mrDefaultPlugin->objectName(), "").toString();
            break;

        default:
            defaultPlugin = "";
    }

    if (!defaultPlugin.isEmpty() && settings->value(ui->actionDisable_plugin_profile->objectName(), true).toBool()) {

        activePlugin->setActive(false);

        for (int i = 0; i < getActivePlugins().size(); i++) {
            if (getActivePlugins()[i]->getName() == defaultPlugin) {
                getActivePlugins()[i]->setActive(true);
                activePlugin = getActivePlugins()[i];
                break;
            }
        }
    }

}

void G19daemon::loadPluginsIntoMenubar() {
    QVector<PluginInterface *> activeplugins = getActivePlugins();

    for (int i = 0; i < plugins.size(); i++) {
        QAction *action = new QAction(plugins[i]->getName(), ui->menuPlugins);

        connect(action, &QAction::changed, this, [=]() {
            QAction *currentSender = dynamic_cast<QAction *>(sender());
            settings->setValue(currentSender->text() + "-enabled", currentSender->isChecked());
        });

        action->setCheckable(true);

        for (int j = 0; j < activeplugins.size(); j++) {
            if (plugins[i] == activeplugins[j]) {
                action->setChecked(true);
            }
        }
        ui->menuPlugins->addAction(action);
    }

}
