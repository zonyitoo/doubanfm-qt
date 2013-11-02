/* libnotify-qt - library for sending notifications implemented in Qt
 * Copyright (C) 2010-2011 Vojtech Drbohlav <vojta.d@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QList>
#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QtDBus>
#include <QVariant>
#include <QImage>

enum ServerInfo
{
	SERVER_INFO_NAME = 0,
	SERVER_INFO_VENDOR,
	SERVER_INFO_VERSION
};

enum NotificationUrgency
{
	NOTIFICATION_URGENCY_LOW,
	NOTIFICATION_URGENCY_NORMAL,
	NOTIFICATION_URGENCY_CRITICAL
};

class QDBusInterface;
class OrgFreedesktopNotificationsInterface;

namespace org
{
	namespace freedesktop
	{
		typedef OrgFreedesktopNotificationsInterface Notifications;
	}
}

class iiibiiay {
public:
    static iiibiiay fromImage(const QImage& img);
    int width;
    int height;
    int rowstride;
    bool hasAlpha;
    int bitsPerSample;
    int channels;
    QByteArray image;
};

Q_DECLARE_METATYPE(iiibiiay)

QDBusArgument &operator<<(QDBusArgument &a, const iiibiiay &i);
const QDBusArgument &operator>>(const QDBusArgument &a,  iiibiiay &i);

class Q_DECL_EXPORT Notification : public QObject
{
	Q_OBJECT

	typedef QList<Notification *> NotificationList;

	public:
		static bool init(const QString & appName);
		static void uninit();
		static bool isInitted();

		static const QString & getAppName();
		static QStringList getServerCaps();
		static bool getServerInfo(QString & name, QString & vendor, QString & version);

		Notification(const QString & summary, const QString & body = QString(),
					 const QString & iconName = QString(), QObject * parent = 0);
		~Notification();

		bool show();

		void setSummary(const QString & summary);
		void setBody(const QString & body);
		void setIconName(const QString & iconName);
		void setTimeout(qint32 timeout);

		void setUrgency(NotificationUrgency urgency);
		void setCategory(const QString & category);
		//void setIconFromPixmap(const QPixmap & pixmap);
		void setLocation(qint32 x, qint32 y);

		void setHint(const QString & key, const QVariant & value);
		void setHintInt32(const QString & key, qint32 value);
		void setHintDouble(const QString & key, double value);
		void setHintString(const QString & key, const QString & value);
		void setHintByte(const QString & key, char value);
		void setHintByteArray(const QString & key, const QByteArray & value);
		void clearHints();

		void addAction(const QString & actionKey, const QString & label);
		void clearActions();

		bool close();

		bool autoDelete() const;
		void setAutoDelete(bool autoDelete);

	private slots:
		void onNotificationClosed(quint32 id, quint32 reason);
		void onActionInvoked(quint32 id, const QString & actionKey);

	private:
		static void addNotification(Notification * n);
		static void removeNotification(Notification * n);

	private:
		static bool s_isInitted;
		static QString s_appName;
		static org::freedesktop::Notifications * s_notifications;

		quint32 m_id;
		QString m_summary;
		QString m_body;
		QString m_iconName;
		qint32 m_timeout;
		QStringList m_actions;
		QVariantMap m_hints;

		bool m_autoDelete;

	signals:
		void closed(quint32 reason);
		void actionInvoked(const QString & actionKey);
};

#endif // NOTIFICATION_H
