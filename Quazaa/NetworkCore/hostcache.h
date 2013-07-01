/*
** hostcache.h
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef HOSTCACHE_H
#define HOSTCACHE_H

#include "types.h"
#include <QList>
#include <QMutex>

// Increment this if there have been made changes to the way of storing security rules.
#define HOST_CACHE_CODE_VERSION	5
// History:
// 4 - Initial implementation

class CHostCacheHost
{
public:
	CEndPoint       m_oAddress;     // Adres huba
	QDateTime       m_tTimestamp;   // Kiedy ostatnio widziany

	quint32         m_nQueryKey;    // QK
	CEndPoint       m_nKeyHost;     // host dla ktorego jest QK
	QDateTime       m_nKeyTime;     // kiedy odebrano OK?

	QDateTime       m_tAck;         // czas ostatniej operacji wymagajacej potwierdzenia

	QDateTime       m_tLastQuery;   // kiedy poslano ostatnie zapytanie?
	QDateTime       m_tRetryAfter;  // kiedy mozna ponowic?
	QDateTime       m_tLastConnect; // kiedy ostatnio sie polaczylismy?
	qint32			m_nFailures;

	QString			m_sCountry;

public:
	CHostCacheHost()
	{
		m_nQueryKey = 0;
		m_sCountry  = "ZZ";
		m_nFailures = 0;
	}

	bool canQuery(QDateTime tNow = QDateTime::currentDateTimeUtc());
	void setKey(quint32 nKey, CEndPoint* pHost = 0);
};

typedef QList<CHostCacheHost*>::iterator CHostCacheIterator;

class CHostCache
{

public:
	QList<CHostCacheHost*>  m_lHosts;
	QMutex					m_pSection;
	QDateTime				m_tLastSave;

	quint32					m_nMaxCacheHosts;

public:
	CHostCache();
	~CHostCache();

	CHostCacheHost* add(CEndPoint host, QDateTime ts = QDateTime::currentDateTimeUtc());

	CHostCacheIterator find(CEndPoint oHost);
	CHostCacheIterator find(CHostCacheHost* pHost);
	inline CHostCacheHost* take(CEndPoint oHost);
	inline CHostCacheHost* take(CHostCacheHost *pHost);

	void update(CEndPoint oHost);
	void update(CHostCacheHost* pHost);
	void update(CHostCacheIterator itHost);

	void remove(CHostCacheHost* pRemove);
	void remove(CEndPoint oHost);

	void addXTry(QString& sHeader);
	QString getXTry();

	void onFailure(CEndPoint addr);
	CHostCacheHost* get();
	CHostCacheHost* getConnectable(QDateTime tNow = QDateTime::currentDateTimeUtc(), QList<CHostCacheHost*> oExcept = QList<CHostCacheHost*>(), QString sCountry = QString("ZZ"));

	void save();
	void load();

	void pruneOldHosts();
	void pruneByQueryAck();

	inline quint32 size();
	inline bool isEmpty();
};

CHostCacheHost* CHostCache::take(CEndPoint oHost)
{
	CHostCacheIterator it = find( oHost );
	return it == m_lHosts.end() ? NULL : *it;
}
CHostCacheHost* CHostCache::take(CHostCacheHost *pHost)
{
	CHostCacheIterator it = find( pHost );
	return it == m_lHosts.end() ? NULL : *it;
}

quint32 CHostCache::size()
{
	return m_lHosts.size();
}

bool CHostCache::isEmpty()
{
	return !size();
}

extern CHostCache hostCache;

#endif // HOSTCACHE_H
