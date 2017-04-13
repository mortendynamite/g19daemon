#ifndef MPRISFETCHER2_H
#define MPRISFETCHER2_H

#include <QString>
#include <QUrl>
#include <QtDBus/QtDBus>

namespace PlaybackStatus
{
	enum PlaybackStatus
	{
		Playing = 0,
		Paused,
		Stopped
	};
}

struct PlayerStatus
{
	unsigned short Play;
	unsigned short PlayRandom;
	unsigned short Repeat;
	unsigned short RepeatPlaylist;
};

struct MediaData
{
	QString artist;
	QString album;
	QString title;
	QString track;
	QString url;
	unsigned short length;
};

class MprisFetcher2 : public QObject
{
		Q_OBJECT
	public:
		MprisFetcher2(const QString &APlayerName = QString::Null());
		~MprisFetcher2();

		void updateStatus();
		long getPlayerPosition();
		
		PlayerStatus getPlayerStatus() const;
		QString getPlayerName() const;
		bool isSpotify();

	signals:
		void statusChanged(PlayerStatus);
		void trackChanged(MediaData);

	public slots:
		void playerPlay() const;
		void playerStop() const;
		void playerPrev() const;
		void playerNext() const;
		void playerSeek(int sec) const;
		void onPlayerNameChange(const QString &);

	private slots:
		void onPropertyChange(QDBusMessage);
		void onPlayersExistenceChanged(QString, QString, QString);

	private:
		bool spotify;
		void connectToBus();
		void disconnectToBus();
		QDBusInterface *createPlayerInterface();
		void parseTrackInfo(const QVariantMap &);
		void parsePlaybackStatus(const QString &);

	protected:
		QString FPlayerName;
		QDBusInterface *FPlayerInterface;
};

#endif // MPRISFETCHER2_H

