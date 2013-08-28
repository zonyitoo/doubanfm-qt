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

#include "OrgFreedesktopNotificationsInterface.h"

OrgFreedesktopNotificationsInterface::OrgFreedesktopNotificationsInterface(const QString & service,
																		   const QString & path,
																		   const QDBusConnection & connection,
																		   QObject * parent) :
	QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

OrgFreedesktopNotificationsInterface::~OrgFreedesktopNotificationsInterface()
{
}

QDBusPendingReply<QStringList> OrgFreedesktopNotificationsInterface::getCapabilities()
{
	return asyncCall("GetCapabilities");
}

QDBusPendingReply<quint32> OrgFreedesktopNotificationsInterface::notify(const QString & appName, quint32 replacesId,
																		const QString & appIcon,
																		const QString & summary, const QString & body,
																		const QStringList & actions,
																		const QVariantMap & hints,
																		qint32 timeout)
{
	return asyncCall("Notify", appName, replacesId, appIcon, summary, body, actions, hints, timeout);
}

QDBusPendingReply<> OrgFreedesktopNotificationsInterface::closeNotification(quint32 id)
{
	return asyncCall("CloseNotification", id);
}

QDBusPendingReply<QString, QString, QString> OrgFreedesktopNotificationsInterface::getServerInformation()
{
	return asyncCall("GetServerInformation");
}

