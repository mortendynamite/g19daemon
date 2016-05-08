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

#include "mpris.hpp"
#include "mprisfetcher2.hpp"
#include "../../g19daemon.hpp"
#include "../../gscreen.hpp"
#include <QtCore>
#include <QString>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QDebug>
#include <QtDBus>
#include <QFont>
#include <QFontMetrics>
#include <QtDBus/QtDBus>
#include <QTimer>
#include <iostream>

Mpris::Mpris()
{
	Q_INIT_RESOURCE(mpris);

	players = getPlayersList();
	
	settings = new QSettings("Dynamite", "G19Daemon");
	player = settings->value("Mpris.player").toString();
	if (player.isEmpty())
		player = "juk";
	
	isActive = false;
	QImage icon(":/mpris/icon.png");
	screen = new gScreen(icon, player.left(1).toUpper() + player.mid(1));
	mpris = new MprisFetcher2(player);
	mediadata = new MediaData();
	playerstatus = new PlayerStatus();
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

	playerstatus->Play = PlaybackStatus::Stopped;
	
	connect(mpris, SIGNAL(statusChanged(PlayerStatus)), this, SLOT(onStatusChanged(PlayerStatus)));
	connect(mpris, SIGNAL(trackChanged(MediaData)), this, SLOT(onTrackChanged(MediaData)));
	
	LastPos[0] = 0;
	LastPos[1] = 0;
	LastPos[2] = 0;
	LastPos[3] = 0;
	LastPosDir[0] = 0;
	LastPosDir[1] = 0;
	LastPosDir[2] = 0;
	LastPosDir[3] = 0;
	speed = 20;
	menuActive = false;
	menuSelect = 0;
}

Mpris::~Mpris()
{
	delete timer;
	delete mediadata;
	delete playerstatus;
	delete mpris;
	delete screen;
	delete settings;
}

void Mpris::onTimer()
{
	paint();
}

void Mpris::LKeys(int keys)
{
	if (menuActive)
	{
		if (keys & G19_KEY_LLEFT)
		{
		}
		
		if (keys & G19_KEY_LRIGHT)
		{
		}
		
		if (keys & G19_KEY_LUP)
		{
			menuSelect = max(menuSelect - 1, 0);
			menu();
		}
		
		if (keys & G19_KEY_LDOWN)
		{
			menuSelect = min(menuSelect + 1, players.count() - 1);
			menu();
		}
		
		if (keys & G19_KEY_LOK)
		{
			player = players[menuSelect];
			mpris->onPlayerNameChange(player);
			screen->setName(player.left(1).toUpper() + player.mid(1));
			settings->setValue("Mpris.player", player);
			menuActive = false;
			timer->start(1000);
			paint();
		}

		if (keys & G19_KEY_LMENU)
		{
			menuActive = false;
			timer->start(1000);
			paint();
		}

	}
	else
	{
		if (keys & G19_KEY_LLEFT)
		{
			emit mpris->playerPrev();
		}
		
		if (keys & G19_KEY_LRIGHT)
		{
			emit mpris->playerNext();
		}
		
		if (keys & G19_KEY_LUP)
		{
			emit mpris->playerSeek(10);
		}
		
		if (keys & G19_KEY_LDOWN)
		{
			emit mpris->playerSeek(-10);
		}
		
		if (keys & G19_KEY_LCANCEL)
		{
			emit mpris->playerStop();
		}

		if (keys & G19_KEY_LOK)
		{
			emit mpris->playerPlay();
		}

		if (keys & G19_KEY_LMENU)
		{
			menuActive = true;
			timer->stop();
			players = getPlayersList();
			menu();
		}
	}
}

void Mpris::setActive(bool active)
{
	if (active)
	{
		isActive = true;
		mpris->updateStatus();
		paint();
		timer->start(1000);
	}
	else
	{
		timer->stop();
		isActive = false;
	}
}

void Mpris::onStatusChanged(PlayerStatus pl)
{
	playerstatus->Play = pl.Play;
	playerstatus->PlayRandom = pl.PlayRandom;
	playerstatus->Repeat = pl.Repeat;
	playerstatus->RepeatPlaylist = pl.RepeatPlaylist;
	
	LastPos[0] = 0;
	LastPos[1] = 0;
	LastPos[2] = 0;
	LastPos[3] = 0;
	LastPosDir[0] = 0;
	LastPosDir[1] = 0;
	LastPosDir[2] = 0;
	LastPosDir[3] = 0;

	if (!menuActive)
		paint();
}

void Mpris::onTrackChanged(MediaData md)
{
	mediadata->artist = md.artist;
	mediadata->length = md.length;
	mediadata->title = md.title;
	mediadata->track = md.track;
	mediadata->album = md.album;
	mediadata->url = md.url;
	
	LastPos[0] = 0;
	LastPos[1] = 0;
	LastPos[2] = 0;
	LastPos[3] = 0;
	LastPosDir[0] = 0;
	LastPosDir[1] = 0;
	LastPosDir[2] = 0;
	LastPosDir[3] = 0;

	if (!menuActive)
		paint();
}

QStringList Mpris::getPlayersList()
{
	QStringList ret_list;
	QStringList services = QDBusConnection::sessionBus().interface()->registeredServiceNames().value().filter(QLatin1String("org.mpris."));

	foreach (QString service, services)
	{
		if (service.startsWith(QLatin1String("org.mpris.MediaPlayer2.")))
			ret_list << service.replace(QLatin1String("org.mpris.MediaPlayer2."), "");
	}

	return ret_list;
}

void Mpris::menu()
{
	QPainter *p;
	int y, menuStart, menuEnd, pages, page;

	p = screen->Begin();
	
	// How many pages will there be
	pages = qCeil(players.count() / 4);

	page = menuSelect / 4;
	// Calculate the offset for the query

	// Some information to display to the user
	menuStart = page * 4;
	menuEnd = min(menuStart + 4, players.count() - 1);
	y = 0;
	
	for (int i = menuStart; i <= menuEnd ; i++)
	{
		if (menuSelect == i)
		{
			p->fillRect(0, y, 304, 40, screen->getTextRgb());
			p->setPen(screen->getBackgroundRgb());
		}
		else
		{
			p->setPen(screen->getTextRgb());
		}

		p->drawText(5, y, 299, 40, Qt::AlignVCenter | Qt::AlignLeft, players[i]);
		y += 40;
	}
	
	screen->DrawScrollbar(page, pages + 1);
	screen->End();
	
	emit doAction(displayScreen, screen);
}

void Mpris::paint()
{
	QPainter *p;
	QString s;
	QImage bkg;
	int pos, pos2;
	int w, y = 30;
	
	if (!isActive)
		return;
	
	p = screen->Begin();

	p->setCompositionMode(QPainter::CompositionMode_Xor);

	if (!mpris->isSpotify())
	{
		switch (playerstatus->Play)
		{
			case PlaybackStatus::Playing:
				bkg.load(":/mpris/play.png");
				break;
			case PlaybackStatus::Paused:
				bkg.load(":/mpris/pause.png");
				break;
			case PlaybackStatus::Stopped:
				bkg.load(":/mpris/stop.png");
				break;
		}
		
		p->drawImage((320 - bkg.width()) / 2, (171 - bkg.height()) / 2, bkg);
	}

	if (mediadata->album.isEmpty())
	{
		w = screen->getTextFontMetrics().width(mediadata->album);
		if (w > 320)
		{
			if (!LastPosDir[0])
			{
				LastPos[0] -= speed;
				if (LastPos[0] < (320 - w))
					LastPosDir[0] = 1;
			}
			else
			{
				LastPos[0] += speed;
				if (LastPos[0] > 0)
				{
					LastPosDir[0] = 0;
					LastPos[0] = 0;
				}
			}
				
		}
		p->drawText(LastPos[0], y, mediadata->album);
		y += 40;
	}
	
	if (!mediadata->title.isEmpty())
	{
		if (!mediadata->track.isEmpty())
			s = mediadata->track + " " + mediadata->title;
		else
			s = mediadata->title;
		
		w = screen->getTextFontMetrics().width(s);
		if (w > 320)
		{
			if (LastPosDir[1] == 0)
			{
				LastPos[1] -= speed;
				if (LastPos[1] < (320 - w - speed))
					LastPosDir[1] = 1;
			}
			else
			{
				LastPos[1] += speed;
				if (LastPos[1] > 0)
				{
					LastPosDir[1] = 0;
					LastPos[1] = 0;
				}
			}
				
		}
		p->drawText(LastPos[1], y, s);
		y += 40;
	}


	if (!mediadata->artist.isEmpty())
	{
		w = screen->getTextFontMetrics().width(mediadata->artist);
		if (w > 320)
		{
			if (!LastPosDir[2])
			{
				LastPos[2] -= speed;
				if (LastPos[2] < (320 - w))
					LastPosDir[2] = 1;
			}
			else
			{
				LastPos[2] += speed;
				if (LastPos[2] > 0)
				{
					LastPosDir[2] = 0;
					LastPos[2] = 0;
				}
			}
				
		}
		p->drawText(LastPos[2], y, mediadata->artist);
		y += 40;
	}
	
	if (!mediadata->url.isEmpty() && mediadata->title.isEmpty())
	{
		w = screen->getTextFontMetrics().width(mediadata->url);
		if (w > 320)
		{
			if (!LastPosDir[3])
			{
				LastPos[3] -= speed;
				if (LastPos[3] < (320 - w))
					LastPosDir[3] = 1;
			}
			else
			{
				LastPos[3] += speed;
				if (LastPos[3] > 0)
				{
					LastPosDir[3] = 0;
					LastPos[3] = 0;
				}
			}
				
		}
		p->drawText(LastPos[3], y, mediadata->url);
	}
	
	p->setCompositionMode(QPainter::CompositionMode_SourceOver);

	if (!mpris->isSpotify())
	{
		pos2 = mpris->getPlayerPosition();
		pos = (100.0 / mediadata->length) * pos2;
		screen->drawVGuage(5, 171, 310, 30, pos);
		s = QDateTime::fromTime_t(pos2).toString("mm:ss") + " / " + QDateTime::fromTime_t(mediadata->length).toString("mm:ss");
		p->setPen(screen->getBackgroundRgb());
		p->drawText(5, 171, 310, 30, Qt::AlignCenter, s);
	}
	else
	{
		s = QDateTime::fromTime_t(mediadata->length).toString("mm:ss");
		p->drawText(5, 171, 310, 30, Qt::AlignCenter, s);
	}

	screen->End();
	
	emit doAction(displayScreen, screen);
}

bool Mpris::isPopup()
{
	return false;
}

QImage Mpris::getIcon()
{
	return QImage(":/mpris/menu_icon.png");
}

QObject *Mpris::getQObject()
{
	return this;
}

QString Mpris::getName()
{
    return tr("Media Player");
}
