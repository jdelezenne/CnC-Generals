/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"
#include "Platform/Sockets.h"	// This must go first in EVERY cpp file int the GameEngine

#include "GameNetwork/IPEnumeration.h"

IPEnumeration::IPEnumeration( void )
{
	m_IPlist = NULL;
	m_isWinsockInitialized = false;
}

IPEnumeration::~IPEnumeration( void )
{
	if (m_isWinsockInitialized)
	{
		Platform::ShutdownSockets();
		m_isWinsockInitialized = false;
	}

	EnumeratedIP *ip = m_IPlist;
	while (ip)
	{
		ip = ip->getNext();
		Platform::DeletePoolObject(m_IPlist);
		m_IPlist = ip;
	}
}

EnumeratedIP * IPEnumeration::getAddresses( void )
{
	if (m_IPlist)
		return m_IPlist;

	if (!m_isWinsockInitialized)
	{
		if (!Platform::InitializeSockets()) return NULL;
		m_isWinsockInitialized = true;
	}

std::vector<std::uint32_t> addresses;
	if (!Platform::LocalIPv4Addresses(addresses)) return NULL;
	for (const auto address : addresses) {
		const auto* entry = reinterpret_cast<const unsigned char*>(&address);
		EnumeratedIP *newIP = newInstance(EnumeratedIP);

		AsciiString str;
		str.format("%d.%d.%d.%d", (unsigned char)entry[0], (unsigned char)entry[1], (unsigned char)entry[2], (unsigned char)entry[3]);

		UnsignedInt testIP = address;
		UnsignedInt ip = ntohl(testIP);

		/*
		ip = *entry++;
		ip <<= 8;
		ip += *entry++;
		ip <<= 8;
		ip += *entry++;
		ip <<= 8;
		ip += *entry++;
		*/

		newIP->setIPstring(str);
		newIP->setIP(ip);

		DEBUG_LOG(("IP: 0x%8.8X / 0x%8.8X (%s)\n", testIP, ip, str.str()));

		// Add the IP to the list in ascending order
		if (!m_IPlist)
		{
			m_IPlist = newIP;
			newIP->setNext(NULL);
		}
		else
		{
			if (newIP->getIP() < m_IPlist->getIP())
			{
				newIP->setNext(m_IPlist);
				m_IPlist = newIP;
			}
			else
			{
				EnumeratedIP *p = m_IPlist;
				while (p->getNext() && p->getNext()->getIP() < newIP->getIP())
				{
					p = p->getNext();
				}
				newIP->setNext(p->getNext());
				p->setNext(newIP);
			}
		}
	}

	return m_IPlist;
}

AsciiString IPEnumeration::getMachineName( void )
{
	if (!m_isWinsockInitialized)
	{
		if (!Platform::InitializeSockets()) return NULL;
		m_isWinsockInitialized = true;
	}

	// get the local machine's host name
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)))
	{
		DEBUG_LOG(("Failed call to gethostname; WSAGetLastError returned %d\n", WSAGetLastError()));
		return NULL;
	}

	return AsciiString(hostname);
}


