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

#include "pavolume.hpp"
#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include <QtCore>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QDebug>
#include <QSettings>
#include <pulse/pulseaudio.h>
#include <pulse/thread-mainloop.h>
#include <QtConcurrent/QtConcurrent>
#include <iostream>

int lastvol = 0;
int lastmute = 0;

void sink_cb(pa_context *c, const pa_sink_info *info, int eol, void *userdata)
{
	PAVolume *pav = static_cast<PAVolume *>(userdata);
	int vol;
	
	if (eol > 0)
		return;
	
	vol = info->volume.values[0] / 655;
	
	if ((vol != lastvol) || (info->mute != lastmute))
	{
		pav->setVolume(vol, (info->mute != 0));
		lastvol = vol;
		lastmute = info->mute;
	}
}

void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata)
{
	PAVolume *pav = static_cast<PAVolume *>(userdata);
	pa_operation *o;
	
	switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
	{
		case PA_SUBSCRIPTION_EVENT_SINK:
			if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
			{
			}
			else
			{
				if (!(o = pa_context_get_sink_info_by_index(c, index, sink_cb, pav)))
				{
					qDebug() << "pa_context_get_source_info_by_index() failed";
				}
				pa_operation_unref(o);
			}
			break;
	}
}

void state_cb(pa_context *c, void *userdata)
{
	PAVolume *pav = static_cast<PAVolume *>(userdata);
	pa_operation *o;
	
	switch (pa_context_get_state(c))
	{
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;
			
		case PA_CONTEXT_READY:
			pa_context_set_subscribe_callback(c, subscribe_cb, pav);
			if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t) (PA_SUBSCRIPTION_MASK_SINK), NULL, NULL)))
			{
				qDebug() << "pa_context_subscribe() failed";
				return;
			}
			
			pa_operation_unref(o);
			
			
			break;
			
		case PA_CONTEXT_FAILED:
			pa_context_unref(pav->pa_ctx);
			pav->pa_ctx = NULL;
			
			break;
			
		case PA_CONTEXT_TERMINATED:
		default:
			break;
	}
}

PAVolume::PAVolume()
{
	Q_INIT_RESOURCE(pavolume);

	QImage icon(":/pavolume/icon.png");

	isMuted = false;
	isActive = false;
	screen = new gScreen(icon, tr("Volume"));
	
	// Create a mainloop API and connection to the default server
	pa_ml = pa_threaded_mainloop_new();
	pa_mlapi = pa_threaded_mainloop_get_api(pa_ml);
	pa_ctx = pa_context_new(pa_mlapi, "test");
	
	// This function connects to the pulse server
	pa_context_connect(pa_ctx, NULL, PA_CONTEXT_NOFLAGS, NULL);

	// This function defines a callback so the server will tell us it's state.
	// Our callback will wait for the state to be ready.  The callback will
	// modify the variable to 1 so we know when we have a connection and it's
	// ready.
	// If there's an error, the callback will set pa_ready to 2
	pa_context_set_state_callback(pa_ctx, state_cb, this);
	pa_threaded_mainloop_start(pa_ml);	
	
	connect(this, SIGNAL(volumeChanged()), this, SLOT(doVolumeChanged()));
	connect(this, SIGNAL(Release()), this, SLOT(doRelease()));

	settings = new QSettings("Dynamite", "G19Daemon");
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(BlinkKeyBackground()));
	
}

PAVolume::~PAVolume()
{
	timer->stop();
	
	if (pa_ctx)
	{
		pa_context_disconnect(pa_ctx);
		pa_context_unref(pa_ctx);
	}

	pa_threaded_mainloop_free(pa_ml);

	delete screen;
	delete timer;
	delete settings;
}

void PAVolume::setVolume(int vol, bool mute)
{
	volume = vol;
	isMuted = mute;

	emit volumeChanged();
	
	if (!future.isRunning())
		future = QtConcurrent::run (this, &PAVolume::eventThread);
	
	ftime = QTime::currentTime().addSecs(2);
}

void PAVolume::doVolumeChanged()
{
	emit doAction(grabFocus, NULL);

	if (!isMuted)
	{
		if (timer->isActive())
		{
			timer->stop();
		}
		setKeybackground();
	}
	paint();
	
	if (isMuted)
	{
		timer->start(100);
	}
}
	
void PAVolume::setKeybackground()
{
	QColor color;

	color.setBlue(0);

	if (volume < 50)
	{
		color.setRed(volume * 5.1);
		color.setGreen(255);
	}
	else if (volume > 50)
	{
		color.setRed(255);
		color.setGreen(255 - ((volume - 50) * 5.1));
	}
	else
	{
		color.setRed(255);
		color.setGreen(255);
	}

	emit doAction(setKeyBackground, &color);

}

QString PAVolume::getName()
{
    return tr("Volume");
}

void PAVolume::LKeys(int keys)
{
}

void PAVolume::setActive(bool active)
{
	isActive = active;

	if (isActive)
		paint();
}

void PAVolume::paint()
{
	QPainter *p;
	QFont header("DejaVuSans", 20);
	QString s;
	int pos;
	
	QImage sound(":/pavolume/sound.png");
	
	p = screen->begin();
	p->drawImage((320 - sound.width()) / 2, (146 - sound.height()) / 2, sound);
	p->setFont(header);

	if (isMuted)
	{
		s = "Muted";
		pos = 0;
	}
	else
	{
		s = QString::number(volume) + "%";
		pos = volume;
	}
	
	screen->drawVGuage(5, 171, 310, 30, pos);
	p->setPen(qRgb(255, 255, 255));
	p->drawText(5, 171, 310, 30, Qt::AlignCenter, s);
	screen->end();
	emit doAction(displayScreen, screen);
}

void PAVolume::eventThread()
{
	QTime t = QTime::currentTime();
	QColor color;

	while (t < ftime)
	{
		t = QTime::currentTime();
	}
	future.cancel();
	emit Release();
	emit doAction(restoreKeyBackground, NULL);
}

void PAVolume::BlinkKeyBackground()
{
	static int red = 0;
	static bool up = true;
	QColor color;
	
	if (up & (red < 255))
		red += 20;
	else
		red -= 20;
	
	if (red >= 255)
	{
		up = false;
		red = 255;
	}
	
	if (red <= 0)
	{
		up = true;
		red = 0;
	}
		
	color.setRed(red);
	color.setGreen(0);
	color.setBlue(0);
	emit doAction(setKeyBackground, &color);
}


void PAVolume::doRelease()
{
	emit doAction(releaseFocus, NULL);
}

bool PAVolume::isPopup()
{
	return true;
}

QImage PAVolume::getIcon()
{
	return QImage(":/pavolume/menu_icon.png");
}

QObject * PAVolume::getQObject()
{
	return this;
}
