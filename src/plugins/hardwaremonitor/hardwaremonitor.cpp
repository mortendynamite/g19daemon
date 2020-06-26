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

#include "hardwaremonitor.hpp"

HardwareMonitor::HardwareMonitor() {
  Q_INIT_RESOURCE(hardwaremonitor);

  watcher = new QFileSystemWatcher(this);
  startScreen = new StartScreen("StartSreen");
  loadsettings();

  isActive = false;
  screen =
      new Gscreen(QImage(":/hardwaremonitor/icon.png"), tr("Hardware Monitor"));

  watcher->addPath(QDir::home().absolutePath() + "/.config/HWA/settings.ini");
  connect(watcher, SIGNAL(fileChanged(const QString &)), this,
          SLOT(reloadSettings()));

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this,
          QOverload<>::of(&HardwareMonitor::paint));
  timer->start(5000);

  qDebug() << "Plugin initialized";
}

HardwareMonitor::~HardwareMonitor() {
  if (watcher != nullptr) {
    delete watcher;
  }
  if (startScreen != nullptr) {
    delete startScreen;
  }
  delete screen;
}
void HardwareMonitor::loadsettings() {
  qDebug() << "Load Settings";

  HwaSettings::getInstance()->loadSettings();
  screens = HwaSettings::getInstance()->getScreenList();

  QList<Screen *> mainorder = HwaSettings::getInstance()->getMainOrder();

  if (mainorder.isEmpty()) {
    screens.append(startScreen);
    currentScreen_ = screens[0];
    currentMainScreen_ = currentScreen_;
    qDebug() << "Load StartScreen";
  } else {
    qDebug() << "Activate " + mainorder[0]->getName();
    currentScreen_ = mainorder[0];
    currentMainScreen_ = currentScreen_;
  }
}

void HardwareMonitor::reloadSettings() {
  qDebug() << "Reload settings";

  screens.append(startScreen);
  currentScreen_ = screens[0];
  currentMainScreen_ = currentScreen_;

  HwaSettings::getInstance()->removeSettings();

  loadsettings();

  paint();
}

QString HardwareMonitor::getName() { return tr("Hardware Monitor"); }

void HardwareMonitor::lKeys(int keys) {
  if (!isActive)
    return;

  QList<Screen *> mainOrder_ = HwaSettings::getInstance()->getMainOrder();
  QMap<QString, QList<Screen *>> subOrder_ =
      HwaSettings::getInstance()->getSubOrder();

  if (keys & G19Keys::G19_KEY_LRIGHT) {
    int position = mainOrder_.indexOf(currentMainScreen_) + 1;

    if (position >= mainOrder_.size()) {
      position = 0;
    }

    currentScreen_ = mainOrder_.at(position);
    currentMainScreen_ = mainOrder_.at(position);
  }

  else if (keys & G19Keys::G19_KEY_LLEFT) {
    int position = mainOrder_.indexOf(currentMainScreen_) - 1;

    if (position < 0) {
      position = mainOrder_.size() - 1;
    }

    currentScreen_ = mainOrder_.at(position);
    currentMainScreen_ = mainOrder_.at(position);
  }

  else if (keys & G19Keys::G19_KEY_LUP) {
    QList<Screen *> subScreen = subOrder_.value(currentMainScreen_->getName());

    int currentPosition = subScreen.indexOf(currentScreen_) - 1;

    if (currentPosition < 0) {
      currentPosition = subScreen.size() - 1;
    }

    currentScreen_ = subScreen.at(currentPosition);
  }

  else if (keys & G19Keys::G19_KEY_LDOWN) {
    QList<Screen *> subScreen = subOrder_.value(currentMainScreen_->getName());

    int currentPosition = subScreen.indexOf(currentScreen_) + 1;

    if (currentPosition >= subScreen.size()) {
      currentPosition = 0;
    }

    currentScreen_ = subScreen.at(currentPosition);
  } else if (keys & G19Keys::G19_KEY_LOK) {
    currentScreen_->okPressed();
  }

  paint();
}

void HardwareMonitor::setActive(bool active) {
  isActive = active;

  if (active) {
    paint();
  }
}

void HardwareMonitor::paint() {
  if (isActive) {

    if (currentScreen_ != nullptr) {
      currentScreen_->update();
      currentScreen_->draw(screen);
    }

    emit doAction(displayFullScreen, screen);
  }
}

bool HardwareMonitor::isPopup() { return false; }

QObject *HardwareMonitor::getQObject() { return this; }

QImage HardwareMonitor::getIcon() {
  return QImage(":/hardwaremonitor/menu_icon.png");
}
