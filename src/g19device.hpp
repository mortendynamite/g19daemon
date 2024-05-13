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

#ifndef G19_H
#define G19_H

#include <QColor>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include <libusb.h>

using namespace std;

#define BUFF_SIZE                                                              \
  154112 //	154112 = 320x240 x 2 bytes/pixel + header of 512 bytes

enum G19Keys {
  G19_KEY_G1 = 1 << 0,
  G19_KEY_G2 = 1 << 1,
  G19_KEY_G3 = 1 << 2,
  G19_KEY_G4 = 1 << 3,
  G19_KEY_G5 = 1 << 4,
  G19_KEY_G6 = 1 << 5,
  G19_KEY_G7 = 1 << 6,
  G19_KEY_G8 = 1 << 7,
  G19_KEY_G9 = 1 << 8,
  G19_KEY_G10 = 1 << 9,
  G19_KEY_G11 = 1 << 10,
  G19_KEY_G12 = 1 << 11,

  G19_KEY_M1 = 1 << 12,
  G19_KEY_M2 = 1 << 13,
  G19_KEY_M3 = 1 << 14,
  G19_KEY_MR = 1 << 15,

  G19_KEY_LHOME = 1 << 16,
  G19_KEY_LCANCEL = 1 << 17,
  G19_KEY_LMENU = 1 << 18,
  G19_KEY_LOK = 1 << 19,
  G19_KEY_LRIGHT = 1 << 20,
  G19_KEY_LLEFT = 1 << 21,
  G19_KEY_LDOWN = 1 << 22,
  G19_KEY_LUP = 1 << 23,
  G19_KEY_LIGHT = 1 << 24
};

#define G19_HAS_G_KEYS (1 << 0)
#define G19_HAS_M_KEYS (1 << 1)
#define G19_HAS_BACKLIGHT_CONTROL (1 << 2)
#define G19_HAS_BRIGHTNESS_CONTROL (1 << 3)


typedef struct  {
  uint16_t vid;
  uint16_t pid;
  int flags;
  int interface;
} G19DeviceType;

typedef void (*G19KeysCallback)(unsigned int keys);

class G19Device : public QObject {
  Q_OBJECT
public:
  int rawkey{};

  G19Device();
  ~G19Device() override;

  void initialize();
  void uninitialize();
  void openDevice(libusb_device_handle* device);
  void closeDevice();
  void eventThread();
  bool probeDevice(libusb_device *device);
  bool isDevice(libusb_device *device);

  void updateLcd(QImage *img);
  void setKeysBacklight(QColor color);
  QColor getKeysBacklight();
  void setMKeys(bool m1, bool m2, bool m3, bool mr);
  void setDisplayBrightness(unsigned char brightness);

  void gKeyCallback(unsigned int keys);
  void lKeyCallback(unsigned int keys);

  G19Keys getActiveMKey();

  unsigned int getKeys();

  bool gKeysTransferCancelled;
  bool lKeysTransferCancelled;

  G19DeviceType type{};

private:
  unsigned int lastkeys;
  bool isDeviceConnected;
  bool isInitialized;
  bool enableEventThread;
  bool isTransfering;
  QString cstatus;
  G19Keys activeMKey;

  libusb_transfer *gKeysTransfer;
  libusb_transfer *lKeysTransfer;
  libusb_transfer *dataTransfer{};

  unsigned char gKeysBuffer[4]{};
  unsigned char lKeysBuffer[2]{};

  libusb_context *context;
  libusb_device_handle *deviceHandle;
  libusb_device_descriptor deviceDesc{};
  libusb_hotplug_callback_handle hotplugCallback{};
  bool probeDevices();

  QFuture<void> future;

  unsigned char *dataBuff;

  QColor backLight;

signals:
  void gKey();
  void lKey();

public slots:
  void changeKeysBacklight(const QColor& color);
};

#endif // G19_H
