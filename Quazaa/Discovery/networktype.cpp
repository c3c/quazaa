﻿/*
** networktype.cpp
**
** Copyright © Quazaa Development Team, 2012.
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

#include <QObject>

#if QT_VERSION < 0x050000
#include <QMetaType>
#endif

#include "networktype.h"

CNetworkType::CNetworkType() :
	m_nNetworks( 0 )
{
	registerMetaType();
}

CNetworkType::CNetworkType(quint16 type) :
	m_nNetworks( type )
{
	registerMetaType();
}

CNetworkType::CNetworkType(DiscoveryProtocol type) :
	m_nNetworks( (quint16)type )
{
	registerMetaType();
}

void CNetworkType::registerMetaType() const
{
	static int foo = qRegisterMetaType<CNetworkType>( "CNetworkType" );
	++foo;
}

bool CNetworkType::operator==(const CNetworkType& type) const
{
	return ( m_nNetworks == type.m_nNetworks );
}

bool CNetworkType::operator!=(const CNetworkType& type) const
{
	return ( m_nNetworks != type.m_nNetworks );
}

bool CNetworkType::isNull() const
{
	return !m_nNetworks;
}

bool CNetworkType::isGnutella() const
{
	return ( m_nNetworks & dpGnutella );
}

void CNetworkType::setGnutella( bool )
{
	m_nNetworks |= dpGnutella;
}

bool CNetworkType::isG2() const
{
	return ( m_nNetworks & dpG2 );
}

void CNetworkType::setG2( bool )
{
	m_nNetworks |= dpG2;
}

bool CNetworkType::isAres() const
{
	return ( m_nNetworks & dpAres );
}

void CNetworkType::setAres( bool )
{
	m_nNetworks |= dpAres;
}

bool CNetworkType::isEDonkey2000() const
{
	return ( m_nNetworks & dpeDonkey2000 );
}

void CNetworkType::setEDonkey2000( bool )
{
	m_nNetworks |= dpeDonkey2000;
}

bool CNetworkType::isNetwork(const CNetworkType& type) const
{
	return ( m_nNetworks & type.toQuint16() ) == type.toQuint16();
}

void CNetworkType::setNetwork(const CNetworkType& type)
{
	m_nNetworks |= type.m_nNetworks;
}

bool CNetworkType::isMulti() const
{
	if ( !m_nNetworks || ( m_nNetworks && !(m_nNetworks & (m_nNetworks - 1)) ) )
		return false;	// m_nNetworks is 0 or power of 2
	else				// m_nNetworks is not 0 or a power of 2 - meaning there are at
		return true;	// least 2 different bits set to 1 within the 16 bit uint
}

quint16 CNetworkType::toQuint16() const
{
	return m_nNetworks;
}

QString CNetworkType::toString() const
{
	if ( !m_nNetworks )
	{
		return QString();
	}

	QString sNetworks;

	if ( isGnutella() )
	{
		sNetworks.append( QObject::tr( "Gnutella" ) );
	}

	if ( isG2() )
	{
		if ( sNetworks.size() )
		{
			sNetworks.append( ", " );
		}
		sNetworks.append( QObject::tr( "G2" ) );
	}

	if ( isAres() )
	{
		if ( sNetworks.size() )
		{
			sNetworks.append( ", " );
		}
		sNetworks.append( QObject::tr( "Ares" ) );
	}

	if ( isEDonkey2000() )
	{
		if ( sNetworks.size() )
		{
			sNetworks.append( ", " );
		}
		sNetworks.append( QObject::tr( "EDonkey2000" ) );
	}

	return sNetworks;
}
