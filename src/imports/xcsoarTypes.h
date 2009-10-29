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
 * @file xcsoarTypes.h
 *
 * @brief Data types imported from XCSoar project.
 * 
 * Those types are needed to generate XCSoar task files that are in fact
 * application memory dumps.
**/

#ifndef __XCSOARTYPES_H__
#define __XCSOARTYPES_H__

/**
 * @brief Data imported from XCSoar project.
**/
namespace xcsoar {

  const unsigned BINFILEMAGICNUMBER = 0x5c378fcf;
  const unsigned MAXTASKPOINTS = 10;
  const unsigned MAXSTARTPOINTS = 10;
  const unsigned MAXISOLINES = 32;

  struct GEOPOINT {
    double Longitude;
    double Latitude;
  };

  enum AATSectorType_t {
    AAT_CIRCLE=0,
    AAT_SECTOR
  };

  struct TASK_POINT {
    int Index;
    double InBound;
    double OutBound;
    double Bisector;
    double LegDistance;
    double LegBearing;
    GEOPOINT SectorStart;
    GEOPOINT SectorEnd;
    AATSectorType_t AATType;
    double AATCircleRadius;
    double AATSectorRadius;
    double AATStartRadial;
    double AATFinishRadial;
    GEOPOINT AATStart;
    GEOPOINT AATFinish;

    // from stats
    double AATTargetOffsetRadius;
    double AATTargetOffsetRadial;
    GEOPOINT AATTargetLocation;
    bool   AATTargetLocked;
    double LengthPercent;
    GEOPOINT IsoLine_Location[MAXISOLINES];
    bool IsoLine_valid[MAXISOLINES];
  };

  enum AutoAdvanceMode_t {
    AUTOADVANCE_MANUAL=0,
    AUTOADVANCE_AUTO,
    AUTOADVANCE_ARM,
    AUTOADVANCE_ARMSTART
  };

  enum ASTSectorType_t {
    AST_CIRCLE=0,
    AST_FAI,
    AST_DAE
  };

  enum StartSectorType_t {
    START_CIRCLE=0,
    START_LINE,
    START_SECTOR
  };

  enum FinishSectorType_t {
    FINISH_CIRCLE=0,
    FINISH_LINE,
    FINISH_SECTOR
  };

  struct SETTINGS_TASK {
    AutoAdvanceMode_t AutoAdvance;
    ASTSectorType_t SectorType;
    unsigned int SectorRadius;
    StartSectorType_t StartType;
    unsigned StartRadius;
    FinishSectorType_t FinishType;
    unsigned FinishRadius;
    double AATTaskLength;
    bool AATEnabled;
    bool EnableMultipleStartPoints;
    bool EnableFAIFinishHeight;
    unsigned FinishMinHeight;
    unsigned StartMaxHeight;
    unsigned StartMaxHeightMargin;
    unsigned StartMaxSpeed;
    unsigned StartMaxSpeedMargin;
    int StartHeightRef;
  };

  struct START_POINT {
    int Index;
    double OutBound;
    GEOPOINT SectorStart;
    GEOPOINT SectorEnd;
  };

}

#endif /* __XCSOARTYPES_H__ */