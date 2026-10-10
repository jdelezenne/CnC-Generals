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

// FILE: Win32Mouse.h /////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//                                                                          
//                       Westwood Studios Pacific.                          
//                                                                          
//                       Confidential Information					         
//                Copyright (C) 2001 - All Rights Reserved                  
//                                                                          
//-----------------------------------------------------------------------------
//
// Project:    RTS3
//
// File name:  Win32Mouse.h
//
// Created:    Colin Day, July 2001
//
// Desc:       Interface for the mouse using only the Win32 messages
//
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WIN32MOUSE_H_
#define __WIN32MOUSE_H_

// SYSTEM INCLUDES ////////////////////////////////////////////////////////////

// USER INCLUDES //////////////////////////////////////////////////////////////
#include "GameClient/Mouse.h"

// FORWARD REFERENCES /////////////////////////////////////////////////////////

// TYPE DEFINES ///////////////////////////////////////////////////////////////
enum { NO_TIME_FROM_WINDOWS = 0 };
// Win32Mouse -----------------------------------------------------------------
/** Mouse interface for when using only the Win32 messages */
//-----------------------------------------------------------------------------
class Win32Mouse : public Mouse
{

public:

	Win32Mouse( void );
	virtual ~Win32Mouse( void );

	virtual void init( void );		///< init mouse, extend this functionality, do not replace
	virtual void reset( void );		///< reset the system
	virtual void update( void );	///< update
	virtual void initCursorResources(void);	///< load windows resources needed for 2d cursors.

	virtual void setCursor( MouseCursor cursor );		///< set mouse cursor
	virtual void capture( void );										///< capture the mouse
	virtual void releaseCapture( void );						///< release mouse capture

	virtual void setVisibility(Bool visible);
	void lostFocus (Bool state) { m_lostFocus = state;}

protected:

	/// get the next event available in the buffer
	virtual UnsignedByte getMouseEvent( MouseIO *result, Bool flush );
	MouseCursor m_currentWin32Cursor;	///< keep track of last cursor image sent to D3D.
	Int m_directionFrame;	///< current frame of directional cursor (frome 0 points up).
	Bool m_lostFocus;		///< flag if window has lost focues and mouse should stop being updated.	
};  // end Win32Mouse 

// INLINING ///////////////////////////////////////////////////////////////////

// EXTERNALS //////////////////////////////////////////////////////////////////

#endif // __WIN32MOUSE_H_

