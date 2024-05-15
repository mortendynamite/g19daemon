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

#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include <libusb.h>

#include "g19device.hpp"
#include "hdata.hpp"

using namespace std;

static G19DeviceType deviceTypes[] = 
  {
    {0x046d, 0xc229, QList({G19_HAS_G_KEYS, G19_HAS_BACKLIGHT_CONTROL, G19_HAS_BRIGHTNESS_CONTROL, G19_HAS_M_KEYS}), 1},
    {0x046d, 0xc629, QList<Flags>(0), 0},
  };


extern "C" int LIBUSB_CALL _HotplugCallback(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data) {
  unsigned int keys;
  G19Device *cthis = static_cast<G19Device *>(user_data);
  if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
    qDebug() << "New Logitech device detected." <<  Qt::endl;
    cthis->probeDevice(device);
  } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
    if (cthis->isDevice(device)) {
      qDebug() << "Our device was unplugged." <<  Qt::endl;
      cthis->closeDevice();
    }
  }
  return 0;
}

extern "C" void LIBUSB_CALL _GKeysCallback(libusb_transfer *transfer) {
  unsigned int keys;
  G19Device *cthis = static_cast<G19Device *>(transfer->user_data);
  keys = 0x00;
  libusb_transfer_status status = transfer->status;

  if (status == LIBUSB_TRANSFER_CANCELLED) {
    cthis->gKeysTransferCancelled = true;
    return;
  }

  //    cthis->rawkey = transfer->buffer[0] << 24 | transfer->buffer[1] << 16 |
  //    transfer->buffer[2] << 8 | transfer->buffer[3];

  if(status == LIBUSB_TRANSFER_COMPLETED) {
      if (transfer->length < 3)
        return;

      if (transfer->buffer[0] < 1)
        return;

      if ((transfer->buffer[1] & 0x00) && (transfer->buffer[2] & 0x00))
        return;

      if (transfer->buffer[1] & 0x01)
        keys |= G19_KEY_G1;

      if (transfer->buffer[1] & 0x02)
        keys |= G19_KEY_G2;

      if (transfer->buffer[1] & 0x04)
        keys |= G19_KEY_G3;

      if (transfer->buffer[1] & 0x08)
        keys |= G19_KEY_G4;

      if (transfer->buffer[1] & 0x10)
        keys |= G19_KEY_G5;

      if (transfer->buffer[1] & 0x20)
        keys |= G19_KEY_G6;

      if (transfer->buffer[1] & 0x40)
        keys |= G19_KEY_G7;

      if (transfer->buffer[1] & 0x80)
        keys |= G19_KEY_G8;

      if (transfer->buffer[2] & 0x01)
        keys |= G19_KEY_G9;

      if (transfer->buffer[2] & 0x02)
        keys |= G19_KEY_G10;

      if (transfer->buffer[2] & 0x04)
        keys |= G19_KEY_G11;

      if (transfer->buffer[2] & 0x08)
        keys |= G19_KEY_G12;

      if (transfer->buffer[2] & 0x10)
        keys |= G19_KEY_M1;

      if (transfer->buffer[2] & 0x20)
        keys |= G19_KEY_M2;

      if (transfer->buffer[2] & 0x40)
        keys |= G19_KEY_M3;

      if (transfer->buffer[2] & 0x80)
        keys |= G19_KEY_MR;

      if (transfer->buffer[3] & 0x48)
        keys |= G19_KEY_LIGHT;

      cthis->gKeyCallback(keys);

  }
  else {
    cthis->gKeyCallback(0);
  }

}

extern "C" void LIBUSB_CALL _LKeysCallback(libusb_transfer *transfer) {
  unsigned int keys;
  G19Device *cthis = static_cast<G19Device *>(transfer->user_data);

  keys = 0x00;

  //    cthis->rawkey = transfer->buffer[0] << 24 | transfer->buffer[1] << 16 |
  //    transfer->buffer[2] << 8 | transfer->buffer[3];

  if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
    cthis->lKeysTransferCancelled = true;
    return;
  }

  if (transfer->buffer[0] & 0x01)
    keys |= G19_KEY_LHOME;

  if (transfer->buffer[0] & 0x02)
    keys |= G19_KEY_LCANCEL;

  if (transfer->buffer[0] & 0x04)
    keys |= G19_KEY_LMENU;

  if (transfer->buffer[0] & 0x08)
    keys |= G19_KEY_LOK;

  if (transfer->buffer[0] & 0x10)
    keys |= G19_KEY_LRIGHT;

  if (transfer->buffer[0] & 0x20)
    keys |= G19_KEY_LLEFT;

  if (transfer->buffer[0] & 0x40)
    keys |= G19_KEY_LDOWN;

  if (transfer->buffer[0] & 0x80)
    keys |= G19_KEY_LUP;

  cthis->lKeyCallback(keys);
}

extern "C" void LIBUSB_CALL _TransferCallback(libusb_transfer *transfer) {
  bool *is;

  is = (bool *)transfer->user_data;
  *is = false;
}

G19Device::G19Device() {
  context = nullptr;
  deviceHandle = nullptr;
  isDeviceConnected = false;
  isInitialized = false;
  enableEventThread = true;
  gKeysTransfer = nullptr;
  lKeysTransfer = nullptr;
  lastkeys = 0;
  isTransfering = false;
  gKeysTransferCancelled = false;
  lKeysTransferCancelled = false;
  dataBuff = new unsigned char[BUFF_SIZE];
}

G19Device::~G19Device() { delete[] dataBuff; }

void G19Device::initialize() {
  int status;

  status = libusb_init(&context);

  if (status < 0) {
    cstatus = tr("Initialization Error!");
    qDebug() << cstatus << " : " << status << Qt::endl;
    isInitialized = false;
    context = nullptr;
    return;
  }

  isInitialized = true;

  future = QtConcurrent::run([this] {eventThread(); });

  libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

  // FIXME check for failure
  libusb_hotplug_register_callback(context, 
                                   LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                   LIBUSB_HOTPLUG_ENUMERATE,
                                   0x046d, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                   _HotplugCallback, this, &hotplugCallback);

}


bool G19Device::probeDevice(libusb_device *device) {
	struct libusb_device_descriptor desc;
  libusb_device_handle *handle;
  int ret;
	libusb_get_device_descriptor(device, &desc);

  for(G19DeviceType t: deviceTypes) {
    if (desc.idVendor == t.vid && desc.idProduct == t.pid) {
			ret = libusb_open(device, &handle);

      if (handle == nullptr) {
        cstatus = tr("Cannot open device: ");
        qDebug() << cstatus << libusb_error_name(ret) <<  Qt::endl;
        continue;
		  } else {
        type = t;
        openDevice(handle);
  		  return true;
      }
    }
  }
  return false;
}

bool G19Device::probeDevices() {
  libusb_device **devs;
  int count = libusb_get_device_list(context, &devs);
  int retVal = -1;

  if (count < 0) {
    cstatus = tr("Could not enumerate USB devices!");
    qDebug() << cstatus <<  Qt::endl;
    return false;
  }

  for (int i = 0; i < count; i++) {
		struct libusb_device* device = devs[i];

    if (probeDevice(device)) {
      libusb_free_device_list(devs, 1);
      return true;
    }
	}
  libusb_free_device_list(devs, 1);
  return false;
}

void G19Device::openDevice(libusb_device_handle *handle) {
  int status;

  libusb_detach_kernel_driver(handle, type.interface);

  status = libusb_claim_interface(handle, type.interface);

  if (status == LIBUSB_SUCCESS) {
    isDeviceConnected = true;

    if (type.flags.contains(G19_HAS_G_KEYS)) {
      gKeysTransfer = libusb_alloc_transfer(0);
      libusb_fill_interrupt_transfer(gKeysTransfer, handle,
                                   LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_OTHER,
                                   gKeysBuffer, 4, _GKeysCallback, this, 0);
      libusb_submit_transfer(gKeysTransfer);
    }

    lKeysTransfer = libusb_alloc_transfer(0);
    libusb_fill_interrupt_transfer(lKeysTransfer, handle,
                                   LIBUSB_ENDPOINT_IN |
                                       LIBUSB_RECIPIENT_INTERFACE,
                                   lKeysBuffer, 2, _LKeysCallback, this, 0);
    libusb_submit_transfer(lKeysTransfer);
    deviceHandle = handle;
  } else {
    qDebug() << "Cannot claim Interface" <<  Qt::endl;
    isDeviceConnected = false;
    deviceHandle = nullptr;
    libusb_close(handle);
  }
}

void G19Device::uninitialize() {
  if (isInitialized) {
    libusb_hotplug_deregister_callback(context, hotplugCallback);
    enableEventThread = false;
    future.waitForFinished();
    libusb_exit(context);
    context = nullptr;
    isDeviceConnected = false;
    isInitialized = false;
  }
}

bool G19Device::isDevice(libusb_device *device) {
  if (!isDeviceConnected || deviceHandle == nullptr) { return false; }
  return device == libusb_get_device(deviceHandle);
}

void G19Device::closeDevice() {
  int status;

  if (isDeviceConnected) {

    while (isTransfering)
      ;

    if (gKeysTransfer) {
      status = libusb_cancel_transfer(gKeysTransfer);
      if (status == 0) {
        while (!gKeysTransferCancelled)
          ;
      }
      libusb_free_transfer(gKeysTransfer);
      gKeysTransfer = nullptr;
    }

    if (lKeysTransfer) {
      status = libusb_cancel_transfer(lKeysTransfer);
      if (status == 0) {
        while (!lKeysTransferCancelled)
          ;
      }
      libusb_free_transfer(lKeysTransfer);
      lKeysTransfer = nullptr;
    }

    libusb_reset_device(deviceHandle);
    libusb_release_interface(deviceHandle,  type.interface);
    libusb_attach_kernel_driver(deviceHandle, type.interface);
    libusb_close(deviceHandle);
    deviceHandle = nullptr;;
    isDeviceConnected = false;
  }
}

void G19Device::eventThread() {
  struct timeval tv;

  while (enableEventThread) {
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    libusb_handle_events_timeout(context, &tv);
  }
}

void G19Device::gKeyCallback(unsigned int keys) {

  lastkeys = keys;
  /*	QString str = QString::number(rawkey, 16);
          while (str.length() < 16)
          {
                  str.prepend('0');
          }
          qDebug() << "G-Keys: " << str; */

  if(keys != 0) {
    emit gKey();
  }

  if (gKeysTransfer != nullptr) {
    libusb_submit_transfer(gKeysTransfer);
  }
}

void G19Device::lKeyCallback(unsigned int keys) {
  lastkeys = keys;

  /*	QString str = QString::number(rawkey, 16);
          while (str.length() < 16)
          {
                  str.prepend('0');
          }
          qDebug() << "L-Keys: " << str; */

  emit lKey();

  if (lKeysTransfer != nullptr)
    libusb_submit_transfer(lKeysTransfer);
}

unsigned int G19Device::getKeys() { return lastkeys; }

void G19Device::updateLcd(QImage *imgin) {
  int index, x, y;
  int color;
  QRgb *rowData;
  QRgb pixelData;

  if (!isDeviceConnected) {
    return;
  }

  for (index = 0; index < 512; index++) {
    dataBuff[index] = hdata[index];
  }

  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
      rowData = (QRgb *)imgin->scanLine(y);
      pixelData = rowData[x];
      color = (qRed(pixelData) / 8) << 11;
      color |= (qGreen(pixelData) / 4) << 5;
      color |= (qBlue(pixelData) / 8);
      dataBuff[index++] = (unsigned char)(color & 0xFF);
      dataBuff[index++] = (unsigned char)(color >> 8);
    }
  }

  while (isTransfering) {
  }
  isTransfering = true;
  dataTransfer = libusb_alloc_transfer(0);
  dataTransfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
  libusb_fill_bulk_transfer(dataTransfer, deviceHandle, 0x02, dataBuff,
                            BUFF_SIZE, _TransferCallback, &isTransfering, 0);
  libusb_submit_transfer(dataTransfer);
}

void G19Device::changeKeysBacklight(const QColor& color)
{
    setKeysBacklight(color);
}

void G19Device::setKeysBacklight(QColor color) {
  if (!isDeviceConnected) {
    return;
  }

  if (!(type.flags.contains(G19_HAS_BACKLIGHT_CONTROL))) {
    return;
  }

  backLight = color;

  memset(dataBuff, '\0', BUFF_SIZE);
  dataBuff[8] = 255;
  dataBuff[9] = (unsigned char)color.red();
  dataBuff[10] = (unsigned char)color.green();
  dataBuff[11] = (unsigned char)color.blue();

  while (isTransfering) {
  }
  isTransfering = true;
  dataTransfer = libusb_alloc_transfer(0);
  dataTransfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
  libusb_fill_control_setup(
      dataBuff, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 9,
      0x307, 1, 4);
  libusb_fill_control_transfer(dataTransfer, deviceHandle, dataBuff,
                               _TransferCallback, &isTransfering, 0);
  libusb_submit_transfer(dataTransfer);
}

QColor G19Device::getKeysBacklight() { return backLight; }

void G19Device::setMKeys(bool m1, bool m2, bool m3, bool mr) {
  unsigned char mkeys = 0;

  if (!isDeviceConnected) {
    return;
  }

  if (!(type.flags.contains(G19_HAS_M_KEYS))) {
    return;
  }

  if (m1) {
    mkeys |= 0x80;
    activeMKey = G19_KEY_M1;
  }

  if (m2) {
    mkeys |= 0x40;
    activeMKey = G19_KEY_M2;
  }

  if (m3) {
    mkeys |= 0x20;
    activeMKey = G19_KEY_M3;
  }

  if (mr) {
    mkeys |= 0x10;
    activeMKey = G19_KEY_MR;
  }

  memset(dataBuff, '\0', BUFF_SIZE);
  dataBuff[8] = 0x10;
  dataBuff[9] = mkeys;

  while (isTransfering)
    ;
  isTransfering = true;
  dataTransfer = libusb_alloc_transfer(0);
  dataTransfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
  libusb_fill_control_setup(
      dataBuff, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 9,
      0x305, 0x01, 2);
  libusb_fill_control_transfer(dataTransfer, deviceHandle, dataBuff,
                               _TransferCallback, &isTransfering, 0);
  libusb_submit_transfer(dataTransfer);
}

G19Keys G19Device::getActiveMKey() { return activeMKey; }

void G19Device::setDisplayBrightness(unsigned char brightness) {
  if (!isDeviceConnected) {
    return;
  }

  if (!(type.flags.contains(G19_HAS_BRIGHTNESS_CONTROL))) {
    return;
  }

  memset(dataBuff, '\0', BUFF_SIZE);
  dataBuff[8] = brightness;
  dataBuff[10] = 0xE2;
  dataBuff[11] = 0x12;
  dataBuff[12] = 0x00;
  dataBuff[13] = 0x8C;
  dataBuff[14] = 0x11;
  dataBuff[15] = 0x00;
  dataBuff[16] = 0x10;
  dataBuff[17] = 0x00;

  while (isTransfering) {
  }
  isTransfering = true;
  dataTransfer = libusb_alloc_transfer(0);
  dataTransfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
  libusb_fill_control_setup(
      dataBuff, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE, 0x0A,
      0x00, 0x00, 1);
  libusb_fill_control_transfer(dataTransfer, deviceHandle, dataBuff,
                               _TransferCallback, &isTransfering, 0);
  libusb_submit_transfer(dataTransfer);
}
