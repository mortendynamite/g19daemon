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

#ifndef PAVOLUME_H
#define PAVOLUME_H

#include "../../gscreen.hpp"
#include "../../plugininterface.hpp"
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include <QtCore>
#include <QtPlugin>
#include <pulse/pulseaudio.h>

class PAVolume : public QObject, public PluginInterface {
  Q_OBJECT
  Q_INTERFACES(PluginInterface)

  Q_PLUGIN_METADATA(IID "your-string-here")

public:
  pa_threaded_mainloop *pa_ml;
  pa_mainloop_api *pa_mlapi;
  pa_operation *pa_op;
  pa_context *pa_ctx;

  PAVolume();
  ~PAVolume() override;
  void lKeys(int keys) override;
  QString getName() override;
  QImage getIcon() override;
  void setActive(bool active) override;
  void setVolume(int vol, bool mute);
  bool isPopup() override;
  QObject *getQObject() override;
  void mKeys(int keys) override;
  void eventThread();

private:
  QSettings *settings;
  Gscreen *screen;
  bool isActive;
  int volume;
  bool isMuted;
  QFuture<void> future;
  QTime ftime;
  QTimer *timer;

  void paint();
  void setKeybackground();

private slots:
  void BlinkKeyBackground();

public slots:
  void doVolumeChanged();
  void doRelease();

signals:
  void doAction(gAction action, void *data); // Signal to draw img on screen
  void volumeChanged();
  void Release();
};

#endif // PAVOLUME_H
