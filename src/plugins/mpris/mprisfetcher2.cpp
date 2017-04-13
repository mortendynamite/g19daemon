#include <QtDBus/QtDBus>
#include <QDBusError>
#include <QtDBus/QDBusConnection>
#include <QtDebug>
#include "mprisfetcher2.hpp"

MprisFetcher2::MprisFetcher2(const QString &APlayerName)
{
	QDBusConnection::sessionBus().connect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged", this, SLOT(onPlayersExistenceChanged(QString, QString, QString)));
	FPlayerInterface = nullptr;

	if (APlayerName.isNull() || APlayerName.isEmpty())
	{
		return;
	}

	FPlayerName = APlayerName;
	FPlayerInterface = createPlayerInterface();

	if (FPlayerInterface->isValid())
	{
		connectToBus();
		updateStatus();
	}
}

MprisFetcher2::~MprisFetcher2()
{
	QDBusConnection::sessionBus().disconnect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged", this, SLOT(onPlayersExistenceChanged(QString, QString, QString)));
	disconnectToBus();
	delete FPlayerInterface;
}

void MprisFetcher2::connectToBus()
{
	QDBusConnection::sessionBus().connect("org.mpris.MediaPlayer2." + FPlayerName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(onPropertyChange(QDBusMessage)));
}

void MprisFetcher2::disconnectToBus()
{
	QDBusConnection::sessionBus().disconnect("org.mpris.MediaPlayer2." + FPlayerName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(onPropertyChange(QDBusMessage)));
}

QDBusInterface *MprisFetcher2::createPlayerInterface()
{
	return new QDBusInterface("org.mpris.MediaPlayer2." + FPlayerName, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", QDBusConnection::sessionBus(), this);
}

void MprisFetcher2::playerPlay() const
{
	if (!FPlayerInterface || !FPlayerInterface->isValid())
	{
		return;
	}

	FPlayerInterface->call(QLatin1String("PlayPause"));
}

void MprisFetcher2::playerStop() const
{
	if (!FPlayerInterface || !FPlayerInterface->isValid())
	{
		return;
	}

	FPlayerInterface->call(QLatin1String("Stop"));
}

void MprisFetcher2::playerPrev() const
{
	if (!FPlayerInterface || !FPlayerInterface->isValid())
	{
		return;
	}

	FPlayerInterface->call(QLatin1String("Previous"));
}

void MprisFetcher2::playerNext() const
{
	if (!FPlayerInterface || !FPlayerInterface->isValid())
	{
		return;
	}

	FPlayerInterface->call(QLatin1String("Next"));
}

void MprisFetcher2::playerSeek(int sec) const
{
	if (!FPlayerInterface || !FPlayerInterface->isValid())
	{
		return;
	}

	FPlayerInterface->call("Seek", (sec * 1000000));
}

void MprisFetcher2::updateStatus()
{
	if (FPlayerInterface && FPlayerInterface->isValid())
	{
		QDBusInterface interface("org.mpris.MediaPlayer2." + FPlayerName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", QDBusConnection::sessionBus(), this);

		QDBusReply<QVariant> trackInfo = interface.call("Get", "org.mpris.MediaPlayer2.Player", "Metadata");

		if (trackInfo.isValid())
		{
			QDBusArgument argument = static_cast<QVariant>(trackInfo.value()).value<QDBusArgument>();
			parseTrackInfo(qdbus_cast<QVariantMap>(argument));
		}

        QDBusReply<QVariant> playbackStatus = interface.call("Get", "org.mpris.MediaPlayer2.Player", "PlaybackStatus");

        if (playbackStatus.isValid())
        {
            parsePlaybackStatus(playbackStatus.value().toString());
        }
	}
}

long MprisFetcher2::getPlayerPosition()
{
	if (FPlayerInterface && FPlayerInterface->isValid())
	{
		QDBusInterface interface("org.mpris.MediaPlayer2." + FPlayerName, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", QDBusConnection::sessionBus(), this);
		QDBusReply<QVariant> reply = interface.call("Get", "org.mpris.MediaPlayer2.Player", "Position");

		if (reply.isValid())
		{
			return qdbus_cast<int>(reply.value()) / 1000000;
		}
	}
	return 0;
}

void MprisFetcher2::onPlayerNameChange(const QString &AName)
{
	if (AName.isNull() || AName.isEmpty())
	{
		return;
	}

	FPlayerName = AName;

	if (FPlayerInterface)
	{
		disconnectToBus();
		delete FPlayerInterface;
		FPlayerInterface = nullptr;
	}

	FPlayerInterface = createPlayerInterface();

	if (FPlayerInterface->isValid())
	{
		connectToBus();
		updateStatus();
	}
}

void MprisFetcher2::parseTrackInfo(const QVariantMap &ATrackInfo)
{
	MediaData data;
	
	data.album.clear();
	data.artist.clear();
	data.length = 0;
	data.title.clear();
	data.track.clear();
	data.url.clear();
	
	if (ATrackInfo.contains("xesam:artist"))
	{
		data.artist = ATrackInfo["xesam:artist"].toString();
	}
	else if (ATrackInfo.contains("xesam:composer"))
	{
		data.artist = ATrackInfo["xesam:composer"].toString();
	}

	if (ATrackInfo.contains("mpris:length"))
	{
		data.length = ATrackInfo["mpris:length"].toULongLong() / 1000000;
	}

	if (ATrackInfo.contains("xesam:album"))
	{
		data.album = ATrackInfo["xesam:album"].toString();
	}

	if (ATrackInfo.contains("xesam:title"))
	{
		data.title = ATrackInfo["xesam:title"].toString();
	}

	if (ATrackInfo.contains("xesam:trackNumber"))
	{
		data.track = ATrackInfo["xesam:trackNumber"].toString();
	}

	if (ATrackInfo.contains("mpris:artUrl"))
	{
		data.url = ATrackInfo["mpris:artUrl"].toString();
	}

	emit trackChanged(data);
}

void MprisFetcher2::parsePlaybackStatus(const QString &AStatus)
{

	PlayerStatus pStatus;

	if (AStatus == "Playing")
	{
		pStatus.Play = PlaybackStatus::Playing;
	}
	else if (AStatus == "Paused")
	{
		pStatus.Play = PlaybackStatus::Paused;
	}
	else if (AStatus == "Stopped")
	{
		pStatus.Play = PlaybackStatus::Stopped;
	}

	emit statusChanged(pStatus);
}

void MprisFetcher2::onPropertyChange(QDBusMessage AMsg)
{
	QVariantMap map;

	const QList<QVariant> arguments = AMsg.arguments();

	// find argument in received message
	for (QList<QVariant>::const_iterator iter = arguments.constBegin(); iter != arguments.constEnd(); ++iter)
	{
		if (iter->canConvert<QDBusArgument>())
		{
			map.unite(qdbus_cast<QVariantMap>(iter->value<QDBusArgument>()));
		}
	}

	if (map.contains(QLatin1String("Metadata")))
	{
		const QVariantMap trackInfo = qdbus_cast<QVariantMap>(map[QLatin1String("Metadata")]);
		parseTrackInfo(trackInfo);
	}

	if (map.contains(QLatin1String("PlaybackStatus")))
	{
		const QString sStatus = map.value(QLatin1String("PlaybackStatus")).toString();
		parsePlaybackStatus(sStatus);
	}
}

void MprisFetcher2::onPlayersExistenceChanged(QString AName, QString /*empty*/, QString ANewOwner)
{
	if (!AName.startsWith("org.mpris.MediaPlayer2."))
	{
		return;
	}

	const QStringRef thisPlayer = AName.midRef(QString("org.mpris.MediaPlayer2.").length());

	if (thisPlayer == FPlayerName)
	{
		// player did not closed and did not opened before
		if (!ANewOwner.isEmpty() && !FPlayerInterface)
		{
			onPlayerNameChange(thisPlayer.toString());
		}
		else
		{
			disconnectToBus();
			delete FPlayerInterface;
			FPlayerInterface = nullptr;

			PlayerStatus status;
			status.Play = PlaybackStatus::Stopped;
			emit statusChanged(status);
		}
	}
}
