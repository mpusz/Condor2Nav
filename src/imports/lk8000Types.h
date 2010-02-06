/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000 - 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}

*/

/**
 * @file lk8000Types.h
 *
 * @brief Data types imported from LK8000 project.
 * 
 * Those types are needed to generate LK8000 task files that are in fact
 * application memory dumps.
**/

#ifndef __LK8000TYPES_H__
#define __LK8000TYPES_H__

#include "xcsoarTypes.h"

/**
 * @brief Data imported from LK8000 project.
**/
namespace lk8000 {
 
  using namespace xcsoar;

//#define LK8000_VER_1_20

#ifdef LK8000_VER_1_20

#define CUPSUP 1
  const unsigned MAXTASKPOINTS = 20;
  const unsigned MAXSTARTPOINTS = 20;

#else

  const unsigned MAXTASKPOINTS = 30;
  const unsigned MAXSTARTPOINTS = 30;

#endif

  struct WAYPOINT
  {
    int Number;
    double Latitude;
    double Longitude;
    double Altitude;
    int Flags;
    wchar_t Name[NAME_SIZE + 1];
    wchar_t Comment[COMMENT_SIZE + 1];
    POINT Screen;
    int Zoom;
    BOOL Reachable;
    double AltArivalAGL;
    BOOL Visible;
    bool InTask;
    TCHAR *Details;
    bool FarVisible;
    int FileNum; // which file it is in, or -1 to delete
#ifdef CUPSUP
    short Format;
    TCHAR Freq[CUPSIZE_FREQ];
    int   RunwayLen;
    int   RunwayDir;
    TCHAR Country[5];
    short Style;
#endif
  };

}

#endif /* __LK8000TYPES_H__ */