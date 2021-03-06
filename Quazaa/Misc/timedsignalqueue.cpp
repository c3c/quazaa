/*
** $Id: timedsignalqueue.cpp 868 2012-01-16 14:39:44Z brov $
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

#include <QDebug>

#include "timedsignalqueue.h"
#include "types.h"
#include "debug_new.h"

CTimedSignalQueue signalQueue;

/* ---------------------------------------------------------------------------------------------- */
/* ---------------------------------------- CTimerObject ---------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */

CTimerObject::CTimerObject(QObject* obj, const char* member, quint64 tInterval, bool bMultiShot,
                           QGenericArgument val0, QGenericArgument val1,
                           QGenericArgument val2, QGenericArgument val3,
                           QGenericArgument val4, QGenericArgument val5,
                           QGenericArgument val6, QGenericArgument val7,
                           QGenericArgument val8, QGenericArgument val9) :
    m_tInterval( tInterval ),
    m_bMultiShot( bMultiShot )
{
	resetTime();

	m_sSignal.obj = obj;

	// Copy the c-string we have been given.
	const size_t nLength = strlen( member );
	char* sTmp = new char[nLength + 1];	// We need 1 extra char for the terminal \0.

	// This is supposed to be safe as eventual changes to pTimerObject->
	// m_sSignal.sName should originate from the same thread.
#ifdef _MSC_VER
	strcpy_s( sTmp, nLength + 1, member );
#else
	strcpy( sTmp, member );
#endif

	m_sSignal.sName = sTmp;

	m_sSignal.val0 = val0;
	m_sSignal.val1 = val1;
	m_sSignal.val2 = val2;
	m_sSignal.val3 = val3;
	m_sSignal.val4 = val4;
	m_sSignal.val5 = val5;
	m_sSignal.val6 = val6;
	m_sSignal.val7 = val7;
	m_sSignal.val8 = val8;
	m_sSignal.val9 = val9;

	m_oUUID = QUuid::createUuid();
}

CTimerObject::CTimerObject(QObject* obj, const char* member, quint32 tSchedule,
                           QGenericArgument val0, QGenericArgument val1,
                           QGenericArgument val2, QGenericArgument val3,
                           QGenericArgument val4, QGenericArgument val5,
                           QGenericArgument val6, QGenericArgument val7,
                           QGenericArgument val8, QGenericArgument val9) :
    m_tInterval( 0 ),
    m_bMultiShot( false )
{
	// Transform 32bit UTC time in seconds to 64bit relative time in ms.
	m_tTime = (quint64)( tSchedule ) * 1000 - signalQueue.m_tTimerStartUTCInMSec;

	m_sSignal.obj = obj;

	// Copy the c-string we have been given.
	const size_t nLength = strlen( member );
	char* sTmp = new char[nLength + 1];	// We need 1 extra char for the terminal \0.

	// This is supposed to be safe as eventual changes to pTimerObject->
	// m_sSignal.sName should originate from the same thread.
#ifdef _MSC_VER
	strcpy_s( sTmp, nLength + 1, member );
#else
	strcpy( sTmp, member );
#endif

	m_sSignal.sName = sTmp;

	m_sSignal.val0 = val0;
	m_sSignal.val1 = val1;
	m_sSignal.val2 = val2;
	m_sSignal.val3 = val3;
	m_sSignal.val4 = val4;
	m_sSignal.val5 = val5;
	m_sSignal.val6 = val6;
	m_sSignal.val7 = val7;
	m_sSignal.val8 = val8;
	m_sSignal.val9 = val9;

	m_oUUID = QUuid::createUuid();
}

CTimerObject::CTimerObject(const CTimerObject* const pTimerObject)
{
	m_tTime			= pTimerObject->m_tTime;
	m_tInterval		= pTimerObject->m_tInterval;
	m_bMultiShot	= pTimerObject->m_bMultiShot;

	m_oUUID			= pTimerObject->m_oUUID;

	m_sSignal.obj	= pTimerObject->m_sSignal.obj;

	// Copy the c-string from pTimerObject
	const size_t nLength = strlen( pTimerObject->m_sSignal.sName );
	char* sTmp = new char[nLength + 1];				// We need 1 extra char for the terminal 0.

	// This is supposed to be safe as eventual changes to pTimerObject->
	// m_sSignal.sName should originate from the same thread.
#ifdef _MSC_VER
	strcpy_s( sTmp, nLength + 1, pTimerObject->m_sSignal.sName );
#else
	strcpy( sTmp, pTimerObject->m_sSignal.sName );
#endif

	m_sSignal.sName = sTmp;

	m_sSignal.val0	= pTimerObject->m_sSignal.val0;
	m_sSignal.val1	= pTimerObject->m_sSignal.val1;
	m_sSignal.val2	= pTimerObject->m_sSignal.val2;
	m_sSignal.val3	= pTimerObject->m_sSignal.val3;
	m_sSignal.val4	= pTimerObject->m_sSignal.val4;
	m_sSignal.val5	= pTimerObject->m_sSignal.val5;
	m_sSignal.val6	= pTimerObject->m_sSignal.val6;
	m_sSignal.val7	= pTimerObject->m_sSignal.val7;
	m_sSignal.val8	= pTimerObject->m_sSignal.val8;
	m_sSignal.val9	= pTimerObject->m_sSignal.val9;
}

CTimerObject::~CTimerObject()
{
	delete[] m_sSignal.sName;
}

void CTimerObject::resetTime()
{
	m_tTime = CTimedSignalQueue::getRelativeTimeInMs() + m_tInterval;
}

bool CTimerObject::emitSignal() const
{
	return QMetaObject::invokeMethod( m_sSignal.obj, m_sSignal.sName, Qt::QueuedConnection,
	                                  m_sSignal.val0, m_sSignal.val1, m_sSignal.val2,
	                                  m_sSignal.val3, m_sSignal.val4, m_sSignal.val5,
	                                  m_sSignal.val6, m_sSignal.val7, m_sSignal.val8,
	                                  m_sSignal.val9 );
}


/* ---------------------------------------------------------------------------------------------- */
/* ------------------------------------- CTimedSignalQueue -------------------------------------- */
/* ---------------------------------------------------------------------------------------------- */

QElapsedTimer CTimedSignalQueue::m_oTime;
quint64       CTimedSignalQueue::m_tTimerStartUTCInMSec = 0;

CTimedSignalQueue::CTimedSignalQueue(QObject *parent) :
    QObject( parent ),
    m_nPrecision( 1000 )
{
}

CTimedSignalQueue::~CTimedSignalQueue()
{
	// If the deletion needs to be delayed for some reason, hold m_pShutdownLock.
	m_pShutdownLock.lock();
	stop(); // Prevent timer events from occurring during cleanup process.
	clear();
	m_pShutdownLock.unlock();
}

void CTimedSignalQueue::shutdownLock()
{
	m_pShutdownLock.lock();
}

void CTimedSignalQueue::shutdownUnlock()
{
	m_pShutdownLock.unlock();
}

void CTimedSignalQueue::setup()
{
	QMutexLocker l( &m_pSection );

	m_oTimer.start( m_nPrecision, this );
}

void CTimedSignalQueue::stop()
{
	QMutexLocker l( &m_pSection );

	m_oTimer.stop();
}

void CTimedSignalQueue::clear()
{
	QMutexLocker l( &m_pSection );

	for ( TSignalQueueIterator i = m_QueuedSignals.begin(); i != m_QueuedSignals.end(); )
	{
		delete i.value();
		i = m_QueuedSignals.erase(i);
	}
}

void CTimedSignalQueue::setPrecision( quint64 tInterval )
{
	if ( tInterval )
	{
		QMutexLocker l( &m_pSection );

		m_nPrecision = tInterval;
		m_oTimer.start( m_nPrecision, this );
	}
}

void CTimedSignalQueue::timerEvent(QTimerEvent* event)
{
	if ( event->timerId() == m_oTimer.timerId() )
	{
		checkSchedule();
	}
	else
	{
		QObject::timerEvent( event );
	}
}

void CTimedSignalQueue::checkSchedule()
{
	// don't block too long waiting for a lock...
	if ( m_pSection.tryLock( m_nPrecision / 2 ) )
	{
		const quint64 tRelativeTimeMs = getRelativeTimeInMs();

		for ( TSignalQueueIterator i = m_QueuedSignals.begin(); i != m_QueuedSignals.end(); )
		{
			if ( i.key() > tRelativeTimeMs )
				break;

			CTimerObject* pObj = i.value();
			i = m_QueuedSignals.erase(i);

			bool bSuccess = pObj->emitSignal();

			if ( bSuccess && pObj->m_bMultiShot )
			{
				pObj->resetTime();
				m_QueuedSignals.insert( pObj->m_tTime, pObj );
			}
			else
			{
				qDebug() << "Error in CTimedSignalQueue::checkSchedule(): Unable invoke method! "
				         << pObj->m_sSignal.sName;
				delete pObj;
			}
		}

		m_pSection.unlock();
	}
	else
	{
		qDebug() << "CTimedSignalQueue::checkSchedule(): Unable to aquire a lock. Skipping check.";
	}
}

QUuid CTimedSignalQueue::push(QObject* parent, const char* signal,
                              quint64 tInterval, bool bMultiShot,
                              QGenericArgument val0, QGenericArgument val1,
                              QGenericArgument val2, QGenericArgument val3,
                              QGenericArgument val4, QGenericArgument val5,
                              QGenericArgument val6, QGenericArgument val7,
                              QGenericArgument val8, QGenericArgument val9)
{
	return push( new CTimerObject( parent, signal, tInterval, bMultiShot,
	                               val0, val1, val2, val3, val4, val5, val6, val7, val8, val9 ) );
}

QUuid CTimedSignalQueue::push(QObject* parent, const char* signal, quint32 tSchedule,
                              QGenericArgument val0, QGenericArgument val1,
                              QGenericArgument val2, QGenericArgument val3,
                              QGenericArgument val4, QGenericArgument val5,
                              QGenericArgument val6, QGenericArgument val7,
                              QGenericArgument val8, QGenericArgument val9)
{
	return push( new CTimerObject( parent, signal, tSchedule,
	                               val0, val1, val2, val3, val4, val5, val6, val7, val8, val9 ) );
}

QUuid CTimedSignalQueue::push(CTimerObject* pTimedSignal)
{
	QMutexLocker l( &m_pSection );

	m_QueuedSignals.insert( pTimedSignal->m_tTime, pTimedSignal );

	return pTimedSignal->m_oUUID;
}

bool CTimedSignalQueue::pop(const QObject* parent, const char* signal)
{
	if ( !parent )
		return false;

	bool bFound = false;
	QString sSignalName = "";

	if ( signal )
		sSignalName = signal;

	QMutexLocker l( &m_pSection );

	for ( TSignalQueueIterator iQueue = m_QueuedSignals.begin(); iQueue != m_QueuedSignals.end(); )
	{
		if ( iQueue.value()->m_sSignal.obj == parent &&
		     ( !signal || sSignalName == iQueue.value()->m_sSignal.sName ) )
		{
			delete iQueue.value();
			iQueue = m_QueuedSignals.erase( iQueue );
			bFound = true;
		}
		else
		{
			++iQueue;
		}
	}

	return bFound;
}

bool CTimedSignalQueue::pop(QUuid oTimer_ID)
{
	QMutexLocker l( &m_pSection );

	CTimerObject* pTimer = popInternal( oTimer_ID );

	if ( pTimer )
	{
		delete pTimer;
		return true;
	}

	return false;
}

bool CTimedSignalQueue::setInterval(QUuid oTimer_ID, quint64 tInterval)
{
	QMutexLocker l( &m_pSection );

	CTimerObject* pTimer = popInternal( oTimer_ID );

	if ( pTimer )
	{
		pTimer->m_tInterval = tInterval;
		pTimer->resetTime();

		l.unlock();

		push( pTimer );
		return true;
	}

	return false;
}

CTimerObject* CTimedSignalQueue::popInternal(QUuid oTimer_ID)
{
	CTimerObject* pTimer;

	for ( TSignalQueueIterator iQueue = m_QueuedSignals.begin(); iQueue != m_QueuedSignals.end(); )
	{
		pTimer = iQueue.value();
		if ( pTimer->m_oUUID == oTimer_ID )
		{
			m_QueuedSignals.erase( iQueue );
			return pTimer;
		}
		else
		{
			++iQueue;
		}
	}

	return NULL;
}

