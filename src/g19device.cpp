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

extern "C" void LIBUSB_CALL _GKeysCallback(libusb_transfer *transfer) {
  unsigned int keys;
  G19Device *cthis = static_cast<G19Device *>(transfer->user_data);
  keys = 0x00;

  if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
    cthis->gKeysTransferCancelled = true;
    return;
  }

  //    cthis->rawkey = transfer->buffer[0] << 24 | transfer->buffer[1] << 16 |
  //    transfer->buffer[2] << 8 | transfer->buffer[3];

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
  context = NULL;
  devs = NULL;
  deviceHandle = NULL;
  isDeviceConnected = false;
  isInitialized = false;
  enableEventThread = true;
  gKeysTransfer = NULL;
  lKeysTransfer = NULL;
  lastkeys = 0;
  isTransfering = false;
  gKeysTransferCancelled = false;
  lKeysTransferCancelled = false;
  dataBuff = new unsigned char[BUFF_SIZE];
}

G19Device::~G19Device() { delete[] dataBuff; }

void G19Device::initializeDevice() {
  int status;

  status = libusb_init(&context);

  if (status < 0) {
    cstatus = tr("Initialization Error!");
    qDebug() << cstatus << " : " << status << endl;
    isInitialized = false;
    context = NULL;
    return;
  }

  isInitialized = true;

  future = QtConcurrent::run(this, &G19Device::eventThread);

  libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
}

void G19Device::openDevice() {
  int status;

  if (!isInitialized)
    return;

  status = libusb_get_device_list(context, &devs);

  if (status < 0) {
    cstatus = tr("Get Device Error!");
    qDebug() << cstatus << endl;
    return;
  }

  deviceHandle = libusb_open_device_with_vid_pid(context, 0x046d, 0xc229);

  libusb_free_device_list(devs, 1);

  if (deviceHandle == NULL) {
    cstatus = tr("Cannot open device");
    qDebug() << cstatus << endl;
    return;
  }

  libusb_set_auto_detach_kernel_driver(deviceHandle, 1);

  status = libusb_claim_interface(deviceHandle, 1);

  if (status == LIBUSB_SUCCESS) {
    isDeviceConnected = true;

    gKeysTransfer = libusb_alloc_transfer(0);
    libusb_fill_interrupt_transfer(gKeysTransfer, deviceHandle,
                                   LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_OTHER,
                                   gKeysBuffer, 4, _GKeysCallback, this, 0);
    libusb_submit_transfer(gKeysTransfer);

    lKeysTransfer = libusb_alloc_transfer(0);
    libusb_fill_interrupt_transfer(lKeysTransfer, deviceHandle,
                                   LIBUSB_ENDPOINT_IN |
                                       LIBUSB_RECIPIENT_INTERFACE,
                                   lKeysBuffer, 2, _LKeysCallback, this, 0);
    libusb_submit_transfer(lKeysTransfer);
  } else {
    qDebug() << "Cannot claim Interface" << endl;
    isDeviceConnected = false;
    libusb_close(deviceHandle);
  }
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
      gKeysTransfer = NULL;
    }

    if (lKeysTransfer) {
      status = libusb_cancel_transfer(lKeysTransfer);
      if (status == 0) {
        while (!lKeysTransferCancelled)
          ;
      }
      libusb_free_transfer(lKeysTransfer);
      lKeysTransfer = NULL;
    }

    enableEventThread = false;
    future.cancel();
    future.waitForFinished();

    libusb_reset_device(deviceHandle);
    libusb_release_interface(deviceHandle, 1);
    libusb_close(deviceHandle);
    deviceHandle = NULL;
    libusb_exit(context);
    context = NULL;
    isDeviceConnected = false;
    isInitialized = false;
  } else if (isInitialized) {
    enableEventThread = false;
    future.waitForFinished();
    libusb_exit(context);
    context = NULL;
    isDeviceConnected = false;
    isInitialized = false;
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

  emit gKey();

  if (gKeysTransfer != NULL)
    libusb_submit_transfer(gKeysTransfer);
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

  if (lKeysTransfer != NULL)
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

void G19Device::setKeysBacklight(QColor color) {
  if (!isDeviceConnected) {
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
