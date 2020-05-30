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

#include <QtCore/QObject>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>
#include <QtGui/QImage>
#include <libusb.h>

#include "hdata.hpp"
#include "g19device.hpp"

using namespace std;

extern "C" void LIBUSB_CALL _GKeysCallback(libusb_transfer *transfer)
{
	unsigned int keys;
	G19Device *cthis = static_cast<G19Device *>(transfer->user_data);
	keys = 0x00;
	
    if (transfer->status == LIBUSB_TRANSFER_CANCELLED)
    {
        cthis->GKeys_transfer_cancelled = true;
        return;
    }

//    cthis->rawkey = transfer->buffer[0] << 24 | transfer->buffer[1] << 16 | transfer->buffer[2] << 8 | transfer->buffer[3];
	
	if (transfer->length < 3)
		return;
	
	if (transfer->buffer[0] < 1)
		return;
	
	if ((transfer->buffer[1] & 0x00) && (transfer->buffer[2] & 0x00))
		return;
	
	if (transfer->buffer[1] &0x01)
		keys |= G19_KEY_G1;

	if (transfer->buffer[1] &0x02)
		keys |= G19_KEY_G2;

	if (transfer->buffer[1] &0x04)
		keys |= G19_KEY_G3;

	if (transfer->buffer[1] &0x08)
		keys |= G19_KEY_G4;

	if (transfer->buffer[1] &0x10)
		keys |= G19_KEY_G5;

	if (transfer->buffer[1] &0x20)
		keys |= G19_KEY_G6;

	if (transfer->buffer[1] &0x40)
		keys |= G19_KEY_G7;

	if (transfer->buffer[1] &0x80)
		keys |= G19_KEY_G8;

	if (transfer->buffer[2] &0x01)
		keys |= G19_KEY_G9;

	if (transfer->buffer[2] &0x02)
		keys |= G19_KEY_G10;

	if (transfer->buffer[2] &0x04)
		keys |= G19_KEY_G11;

	if (transfer->buffer[2] &0x08)
		keys |= G19_KEY_G12;

	if (transfer->buffer[2] &0x10)
		keys |= G19_KEY_M1;

	if (transfer->buffer[2] &0x20)
		keys |= G19_KEY_M2;

	if (transfer->buffer[2] &0x40)
		keys |= G19_KEY_M3;

	if (transfer->buffer[2] &0x80)
		keys |= G19_KEY_MR;
	
	if (transfer->buffer[3] & 0x48)
		keys |= G19_KEY_LIGHT;

	cthis->GKeyCallback(keys);
	
}

extern "C" void LIBUSB_CALL _LKeysCallback(libusb_transfer *transfer)
{
	unsigned int keys;
	G19Device *cthis = static_cast<G19Device *>(transfer->user_data);

	keys = 0x00;
	
//    cthis->rawkey = transfer->buffer[0] << 24 | transfer->buffer[1] << 16 | transfer->buffer[2] << 8 | transfer->buffer[3];

	if (transfer->status == LIBUSB_TRANSFER_CANCELLED)
    {
        cthis->LKeys_transfer_cancelled = true;
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

	cthis->LKeyCallback(keys);
}

extern "C" void LIBUSB_CALL _TransferCallback(libusb_transfer *transfer)
{
	bool *is;
	
	is = (bool *) transfer->user_data;
	*is = false;
}

G19Device::G19Device()
{
	context = NULL;
	devs = NULL;
	deviceHandle = NULL;
	isDeviceConnected = false;
	isInitialized = false;
	enableEventThread = true;
	GKeys_transfer = NULL;
	LKeys_transfer = NULL;
	lastkeys = 0;
	isTransfering = false;
    GKeys_transfer_cancelled = false;
    LKeys_transfer_cancelled = false;
	data_buff = new unsigned char [BUFF_SIZE];
}

G19Device::~G19Device()
{
    delete [] data_buff;
}

void G19Device::initializeDevice()
{
	int status;
	
	status = libusb_init(&context);
	
	if (status < 0)
	{
		cstatus = tr("Initialization Error!");
		qDebug() << cstatus << " : " << status << endl;
		isInitialized = false;
		context = NULL;
		return;
	}
	
	isInitialized = true;
	
	future = QtConcurrent::run (this, &G19Device::eventThread);
	
    libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
}

void G19Device::openDevice()
{
    int status;
	
	if (!isInitialized)
		return;
	
	status = libusb_get_device_list(context, &devs);
	
	if (status < 0)
	{
		cstatus = tr("Get Device Error!");
		qDebug() << cstatus << endl;
		return;
	}

    deviceHandle = libusb_open_device_with_vid_pid(context, 0x046d, 0xc229);

    libusb_free_device_list(devs, 1);

    if (deviceHandle == NULL)
	{
		cstatus = tr("Cannot open device");
		qDebug() << cstatus << endl;
		return;
	}
	
    libusb_set_auto_detach_kernel_driver(deviceHandle, 1);

    status = libusb_claim_interface(deviceHandle, 1);

	if (status == LIBUSB_SUCCESS)
	{
		isDeviceConnected = true;

		GKeys_transfer = libusb_alloc_transfer(0);
		libusb_fill_interrupt_transfer(GKeys_transfer, deviceHandle, LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_OTHER, GKeysBuffer, 4, _GKeysCallback, this, 0);
		libusb_submit_transfer(GKeys_transfer);

		LKeys_transfer = libusb_alloc_transfer(0);
		libusb_fill_interrupt_transfer(LKeys_transfer, deviceHandle, LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_INTERFACE, LKeysBuffer, 2, _LKeysCallback, this, 0);
		libusb_submit_transfer(LKeys_transfer);
	}
	else
	{
		qDebug() << "Cannot claim Interface" << endl;
		isDeviceConnected = false;
		libusb_close(deviceHandle);
	}
}

void G19Device::closeDevice()
{
    int status;

	if (isDeviceConnected)
	{
		
		while (isTransfering)
			;

		if (GKeys_transfer)
		{
            status = libusb_cancel_transfer(GKeys_transfer);
            if (status == 0)
            {
                while (!GKeys_transfer_cancelled)
                    ;
            }
			libusb_free_transfer(GKeys_transfer);
			GKeys_transfer = NULL;
		}
		
		if (LKeys_transfer)
		{
            status = libusb_cancel_transfer(LKeys_transfer);
            if (status == 0)
            {
                while (!LKeys_transfer_cancelled)
                    ;
            }
            libusb_free_transfer(LKeys_transfer);
			LKeys_transfer = NULL;
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
	}
	else if (isInitialized)
	{
		enableEventThread = false;
		future.waitForFinished();
		libusb_exit(context);
		context = NULL;
        isDeviceConnected = false;
        isInitialized = false;
	}
}

void G19Device::eventThread()
{
    struct timeval tv;

	while (enableEventThread)
	{
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        libusb_handle_events_timeout(context, &tv);
	}
}

void G19Device::GKeyCallback(unsigned int keys)
{
	lastkeys = keys;

/*	QString str = QString::number(rawkey, 16);
	while (str.length() < 16)
	{
		str.prepend('0');
	}
	qDebug() << "G-Keys: " << str; */
	
	emit GKey();

	if (GKeys_transfer != NULL)
		libusb_submit_transfer(GKeys_transfer);
}

void G19Device::LKeyCallback(unsigned int keys)
{
	lastkeys = keys;
	
/*	QString str = QString::number(rawkey, 16);
	while (str.length() < 16)
	{
		str.prepend('0');
	}
	qDebug() << "L-Keys: " << str; */

	emit LKey();
	
	if (LKeys_transfer != NULL)
		libusb_submit_transfer(LKeys_transfer);
}

unsigned int G19Device::getKeys()
{
	return lastkeys;
}


void G19Device::updateLcd(QImage* imgin)
{
	int index, x, y;
	int color;
	QRgb *rowData;
	QRgb pixelData;

	if (!isDeviceConnected)
	{
		return;
	}

	for (index = 0; index < 512; index++)
	{
		data_buff[index] = hdata[index];
	}

	for (x = 0; x < 320; x++)
	{
		for (y = 0; y < 240; y++)
		{
			rowData = (QRgb *) imgin->scanLine(y);
			pixelData = rowData[x];
			color = (qRed(pixelData) / 8) << 11;
			color |= (qGreen(pixelData) / 4) << 5;
			color |= (qBlue(pixelData) / 8);
			data_buff[index++] = (unsigned char) (color & 0xFF);
			data_buff[index++] = (unsigned char) (color >> 8); 
		}
	}

	while (isTransfering)
		;
	isTransfering = true;
	data_transfer = libusb_alloc_transfer(0);
	data_transfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
	libusb_fill_bulk_transfer(data_transfer, deviceHandle, 0x02, data_buff, BUFF_SIZE, _TransferCallback, &isTransfering, 0);
	libusb_submit_transfer(data_transfer);
}

void G19Device::setKeysBacklight(QColor color)
{
	if (!isDeviceConnected)
	{
		return;
	}
	
	BackLight = color;
	
	memset(data_buff, '\0', BUFF_SIZE);
	data_buff[8] = 255;
	data_buff[9] = (unsigned char) color.red();
	data_buff[10] = (unsigned char) color.green();
	data_buff[11] = (unsigned char) color.blue();
	
	while (isTransfering)
		;
	isTransfering = true;
	data_transfer = libusb_alloc_transfer(0);
	data_transfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
	libusb_fill_control_setup(data_buff, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 9, 0x307, 1, 4);
	libusb_fill_control_transfer(data_transfer, deviceHandle, data_buff, _TransferCallback, &isTransfering, 0);
	libusb_submit_transfer(data_transfer);
}

QColor G19Device::getKeysBacklight()
{
    return BackLight;
}


void G19Device::setMKeys(bool m1, bool m2, bool m3, bool mr)
{
	unsigned char mkeys = 0;

	if (!isDeviceConnected)
	{
		return;
	}

	if (m1)
	{
		mkeys |= 0x80;
        activeMKey = G19_KEY_M1;
	}
	
	if (m2)
	{
		mkeys |= 0x40;
        activeMKey = G19_KEY_M2;
	}
	
	if (m3)
	{
		mkeys |= 0x20;
        activeMKey = G19_KEY_M3;
	}
	
	if (mr)
	{
		mkeys |= 0x10;
	}
	
	memset(data_buff, '\0', BUFF_SIZE);
	data_buff[8] = 0x10;
	data_buff[9] = mkeys;
	
	while (isTransfering)
		;
	isTransfering = true;
	data_transfer = libusb_alloc_transfer(0);
	data_transfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
    libusb_fill_control_setup(data_buff, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 9, 0x305, 0x01, 2);
	libusb_fill_control_transfer(data_transfer, deviceHandle, data_buff, _TransferCallback, &isTransfering, 0);
	libusb_submit_transfer(data_transfer);
	
}

G19Keys G19Device::getActiveMKey()
{
    return activeMKey;    
}

void G19Device::setDisplayBrightness(unsigned char brightness)
{
	if (!isDeviceConnected)
	{
		return;
	}

	memset(data_buff, '\0', BUFF_SIZE);
	data_buff[8] = brightness;
	data_buff[10] = 0xE2;
	data_buff[11] = 0x12;
	data_buff[12] = 0x00;
	data_buff[13] = 0x8C;
	data_buff[14] = 0x11;
	data_buff[15] = 0x00;
	data_buff[16] = 0x10;
	data_buff[17] = 0x00;

	while (isTransfering)
		;
	isTransfering = true;
	data_transfer = libusb_alloc_transfer(0);
	data_transfer->flags = LIBUSB_TRANSFER_FREE_TRANSFER;
    libusb_fill_control_setup(data_buff, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE, 0x0A, 0x00, 0x00, 1);
	libusb_fill_control_transfer(data_transfer, deviceHandle, data_buff, _TransferCallback, &isTransfering, 0);
	libusb_submit_transfer(data_transfer);
}
