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

// FILE: Win32Mouse.cpp ///////////////////////////////////////////////////////////////////////////
// Created:    Colin Day, July 2001
// Desc:       Interface for the mouse using only the Win32 messages
///////////////////////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include "Platform/Cursor.h"


#include "Common/Debug.h"
#include "Platform/Input.h"
#include "GameClient/GameClient.h"
#include "Win32Device/GameClient/Win32Mouse.h"


#ifdef _INTERNAL
// for occasional debugging...
//#pragma optimize("", off)
//#pragma MESSAGE("************************************** WARNING, optimization disabled for debugging purposes")
#endif

// EXTERN /////////////////////////////////////////////////////////////////////////////////////////
extern Win32Mouse *TheWin32Mouse;

Platform::Cursor* cursorResources[Mouse::NUM_MOUSE_CURSORS][MAX_2D_CURSOR_DIRECTIONS];
///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
/** Get a mouse event from the buffer if available, we need to translate
	* from the windows message meanings to our own internal mouse 
	* structure */
//-------------------------------------------------------------------------------------------------
UnsignedByte Win32Mouse::getMouseEvent( MouseIO *result, Bool flush )
{
    Platform::MouseEvent event;
    if (!Platform::ReadMouseEvent(event)) return MOUSE_NONE;
    const UnsignedInt frame = TheGameClient ? TheGameClient->getFrame() : 1;
    result->leftState = result->middleState = result->rightState = MBS_Up;
    result->leftFrame = result->middleFrame = result->rightFrame = 0;
    result->pos.x = event.x;
    result->pos.y = event.y;
    result->time = event.time;
    result->wheelPos = event.wheel;
    if (event.type == Platform::MouseEventType::Button) {
        const MouseButtonState state = !event.down ? MBS_Up : event.clicks >= 2 && (event.clicks % 2) == 0 ? MBS_DoubleClick : MBS_Down;
        if (event.button == 1) { result->leftState = state; result->leftFrame = frame; }
        if (event.button == 2) { result->middleState = state; result->middleFrame = frame; }
        if (event.button == 3) { result->rightState = state; result->rightFrame = frame; }
    }
    return MOUSE_OK;
}

//-------------------------------------------------------------------------------------------------
/** Translate a win32 mouse event to our own event info */
//-------------------------------------------------------------------------------------------------
Win32Mouse::Win32Mouse( void )
{
	m_currentWin32Cursor = NONE;
	for (Int i=0; i<NUM_MOUSE_CURSORS; i++)
		for (Int j=0; j<MAX_2D_CURSOR_DIRECTIONS; j++)
			cursorResources[i][j]=NULL;
	m_directionFrame=0; //points up.
	m_lostFocus = FALSE;
}  // end Win32Mouse

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
Win32Mouse::~Win32Mouse( void )
{

	// remove our global reference that was for the WndProc() only
	TheWin32Mouse = NULL;
	for (Int i=0; i<NUM_MOUSE_CURSORS; ++i)
		for (Int j=0; j<MAX_2D_CURSOR_DIRECTIONS; ++j) {
			Platform::DestroyCursor(cursorResources[i][j]);
			cursorResources[i][j] = NULL;
		}

}  // end ~Win32Mouse

//-------------------------------------------------------------------------------------------------
/** Initialize our device */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::init( void )
{

	// extending functionality
	Mouse::init();

	//
	// when we receive messages from a Windows message procedure, the mouse
	// moves report the current cursor position and not deltas, our mouse
	// needs to process those positions as absolute and not relative
	//
	m_inputMovesAbsolute = TRUE;

}  // end int

//-------------------------------------------------------------------------------------------------
/** Reset */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::reset( void )
{
	Platform::ResetMouseInput();

	// extend
	Mouse::reset();

}  // end reset

//-------------------------------------------------------------------------------------------------
/** Update, called once per frame */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::update( void )
{

	// extend 
	Mouse::update();

}  // end update

//-------------------------------------------------------------------------------------------------
/** Add a window message event along with its WPARAM and LPARAM parameters
	* to our input storage buffer */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::setVisibility(Bool visible)
{
	//Extend
	Mouse::setVisibility(visible);
	//Maybe need to set cursor to force hiding of some cursors.
	Win32Mouse::setCursor(getMouseCursor());
}

/**Preload all the cursors we may need during the game.  This must be done before the D3D device
is created to avoid cursor corruption on buggy ATI Radeon cards. */
void Win32Mouse::initCursorResources(void)
{
	for (Int cursor=FIRST_CURSOR; cursor<NUM_MOUSE_CURSORS; cursor++)
	{
		for (Int direction=0; direction<m_cursorInfo[cursor].numDirections; direction++)
		{	if (!cursorResources[cursor][direction] && !m_cursorInfo[cursor].textureName.isEmpty())
			{	//this cursor has never been loaded before.
				char resourcePath[256];
				//Check if this is a directional cursor
				if (m_cursorInfo[cursor].numDirections > 1)
					sprintf(resourcePath,"data\\cursors\\%s%d.ANI",m_cursorInfo[cursor].textureName.str(),direction);
				else
					sprintf(resourcePath,"data\\cursors\\%s.ANI",m_cursorInfo[cursor].textureName.str());

				cursorResources[cursor][direction]=Platform::LoadCursorFile(resourcePath);
				DEBUG_ASSERTCRASH(cursorResources[cursor][direction], ("MissingCursor %s\n",resourcePath));
			}
		}
//		Platform::SetCursor(cursorResources[cursor][m_directionFrame]);
	}
}

//-------------------------------------------------------------------------------------------------
/** Super basic simplistic cursor */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::setCursor( MouseCursor cursor )
{
	// extend
	Mouse::setCursor( cursor );

	if (m_lostFocus)
		return;	//stop messing with mouse cursor if we don't have focus.

	if (cursor == NONE || !m_visible)
		Platform::SetCursor(NULL);
	else
	{
		Platform::SetCursor(cursorResources[cursor][m_directionFrame]);
	}  // end switch

	// save current cursor
	m_currentWin32Cursor=m_currentCursor = cursor;
	
}  // end setCursor

//-------------------------------------------------------------------------------------------------
/** Capture the mouse to our application */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::capture( void )
{

//	SetCapture( Platform::NativeGameWindow() );

}  // end capture

//-------------------------------------------------------------------------------------------------
/** Release the mouse capture for our app window */
//-------------------------------------------------------------------------------------------------
void Win32Mouse::releaseCapture( void )
{

//	ReleaseCapture();

}  // end releaseCapture








