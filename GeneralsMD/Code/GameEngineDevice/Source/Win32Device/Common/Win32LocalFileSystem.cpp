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

///////// Win32LocalFileSystem.cpp /////////////////////////
// Bryan Cleveland, August 2002
////////////////////////////////////////////////////////////

#include "Platform/FileSystem.h"
#include "Platform/Paths.h"
#include "Common/AsciiString.h"
#include "Common/GameMemory.h"
#include "Common/PerfTimer.h"
#include "Win32Device/Common/Win32LocalFileSystem.h"
#include "Win32Device/Common/Win32LocalFile.h"
#include "Platform/Directory.h"

Win32LocalFileSystem::Win32LocalFileSystem() : LocalFileSystem() 
{
}

Win32LocalFileSystem::~Win32LocalFileSystem() {
}

//DECLARE_PERF_TIMER(Win32LocalFileSystem_openFile)
File * Win32LocalFileSystem::openFile(const Char *filename, Int access /* = 0 */) 
{
	//USE_PERF_TIMER(Win32LocalFileSystem_openFile)
	Win32LocalFile *file = newInstance( Win32LocalFile );	

	// sanity check
	if (strlen(filename) <= 0) {
		return NULL;
	}

	if (file->open(filename, access) == FALSE) {
		file->close();
		Platform::DeletePoolObject(file);
		file = NULL;
	} else {
		file->deleteOnClose();
	}

// this will also need to play nice with the STREAMING type that I added, if we ever enable this

// srj sez: this speeds up INI loading, but makes BIG files unusable. 
// don't enable it without further tweaking.
//
// unless you like running really slowly.
//	if (!(access&File::WRITE)) {
//		// Return a ramfile.
//		RAMFile *ramFile = newInstance( RAMFile );
//		if (ramFile->open(file)) {
//			file->close(); // is deleteonclose, so should delete.
//			ramFile->deleteOnClose();
//			return ramFile;
//		}	else {
//			ramFile->close();
//			ramFile->deleteInstance();
//		}
//	}

	return file;
}

void Win32LocalFileSystem::update() 
{
}

void Win32LocalFileSystem::init() 
{
}

void Win32LocalFileSystem::reset() 
{
}

//DECLARE_PERF_TIMER(Win32LocalFileSystem_doesFileExist)
Bool Win32LocalFileSystem::doesFileExist(const Char *filename) const
{
	//USE_PERF_TIMER(Win32LocalFileSystem_doesFileExist)
	if (Platform::PathExists(filename)) {
		return TRUE;
	}
	return FALSE;
}

void Win32LocalFileSystem::getFileListInDirectory(const AsciiString& currentDirectory, const AsciiString& originalDirectory, const AsciiString& searchName, FilenameList& filenameList, Bool searchSubdirectories) const
{
    AsciiString logicalDirectory = originalDirectory;
    logicalDirectory.concat(currentDirectory);
    FilenameList subdirectories;
    for (const auto& directory : Platform::SearchDirectories(logicalDirectory.str())) {
        for (const auto& entry : Platform::ReadDirectory(directory.c_str(), searchName.str())) {
            if (entry.info.directory) continue;
            AsciiString filename = logicalDirectory;
            filename.concat(entry.name.c_str());
            filenameList.insert(filename);
        }
        if (searchSubdirectories) {
            for (const auto& entry : Platform::ReadDirectory(directory.c_str(), "*.")) {
                if (!entry.info.directory || entry.name == "." || entry.name == "..") continue;
                AsciiString subdirectory = currentDirectory;
                subdirectory.concat(entry.name.c_str());
                subdirectory.concat('\\');
                subdirectories.insert(subdirectory);
            }
        }
    }
    for (const auto& subdirectory : subdirectories)
        getFileListInDirectory(subdirectory, originalDirectory, searchName, filenameList, searchSubdirectories);
}
Bool Win32LocalFileSystem::getFileInfo(const AsciiString& filename, FileInfo* fileInfo) const
{
    Platform::FileMetadata info;
    if (!Platform::ReadFileMetadata(filename.str(), info)) return FALSE;
    fileInfo->timestampHigh = static_cast<UnsignedInt>(info.lastWriteTime >> 32);
    fileInfo->timestampLow = static_cast<UnsignedInt>(info.lastWriteTime);
    fileInfo->sizeHigh = static_cast<UnsignedInt>(info.size >> 32);
    fileInfo->sizeLow = static_cast<UnsignedInt>(info.size);
    return TRUE;
}
Bool Win32LocalFileSystem::createDirectory(AsciiString directory)
{
	return Platform::CreateUserDirectory(directory.str());
}
