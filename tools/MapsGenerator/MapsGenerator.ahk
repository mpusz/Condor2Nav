;
; This file is part of Condor2Nav file formats translator.
;
; Copyright (C) 2009 Mateusz Pusz
;
; Condor2Nav is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; Condor2Nav is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with Condor2Nav. If not, see <http://www.gnu.org/licenses/>.
;
; Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
;


#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.



; ************************ C O N S T A N T S ***************************

outputPath = ..\..\data
XCSoarOutputPath = %outputPath%\XCSoar
LK8000OutputPath = %outputPath%\LK8000
landscapeEditorTitle = Condor Scenery Toolkit: Landscape Editor
landscapeEditorAirportPropertiesTitle = Airport properties



; ************************ F U N C T I O N S *******************************


Min(a, b)
{
	If(a<b)
		return a
	Else
		return b
}


Max(a, b)
{
	If(a>b)
		return a
	Else
		return b
}


Coordinates(document, coord, lat, guiPrefix)
{
	If(lat) {
		If(coord >= 0)
			letter = N
		else
			letter = S
	}
	Else {
		if(coord >= 0)
			letter = E
		else
			letter = W
	}
	
	coord := Abs(coord)
	deg := Floor(coord)
	min := Floor((coord - deg) * 60)
	sec := Round(((coord - deg) * 60 - min) * 60)
	If(sec = 60)
	{
		min := min + 1
		sec := 0
	}
	If(min = 60)
	{
		deg := deg + 1
		min := 0
	}

	degStr := guiPrefix . "_degrees"
	minStr := guiPrefix . "_minutes"
	secStr := guiPrefix . "_seconds"
	dirStr := guiPrefix . "_direction"
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", degStr), "Item", 0), "value", deg)
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", minStr), "Item", 0), "value", min)
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", secStr), "Item", 0), "value", sec)
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", dirStr), "Item", 0), "value", letter)
}


XCSoarTerrainDownload()
{
	global landscapeName
	global landscapeVersion
	global maxLat
	global minLat
	global maxLon
	global minLon
	global XCSoarOutputPath
	global fileName

	COM_AtlAxWinInit()
	Gui, +LastFound +Resize

	; create browser window
	pwb := COM_AtlAxGetControl(COM_AtlAxCreateContainer(WinExist(), 0, 0, 800, 600, "Shell.Explorer") )
	Gui, Show, w800 h600, Gui Browser
	
	; navigate to terrain generator
	url := "http://xcsoar.dd.com.au/cgi-bin/terrain.pl"
	COM_Invoke(pwb, "Navigate", url)
	loop
		If (rdy := COM_Invoke(pwb, "readyState") = 4)
			break

	; fill general landscape information
	document := COM_Invoke(pwb, "Document")
	fileName := landscapeName . "_" . landscapeVersion
	StringReplace trnName, fileName, ., _, All
	StringReplace trnName, trnName, %A_SPACE%, _, All
	StringReplace trnName, trnName, -, _, All
	StringTrimRight trnName, trnName, StrLen(trnName) - 20
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "areaname"), "Item", 0), "value", trnName)
	email = user@wp.pl
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "email"), "Item", 0), "value", email)
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "email_confirm"), "Item", 0), "value", email)
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "distance_units"), "Item", 0), "checked", "true")
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "selection_method"), "Item", 2), "checked", "true")
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "generate_xcm"), "Item", 1), "checked", "true")

	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByTagName", "FORM"), "Item", 0), "submit")
	Sleep 1000
	loop
		If (busy := COM_Invoke(pwb, "busy") = false)
			break

	; fill lanscape coordinates
	Coordinates(document, maxLat, true, "top")
	Coordinates(document, minLat, true, "bottom")
	Coordinates(document, maxLon, false, "right")
	Coordinates(document, minLon, false, "left")

	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByTagName", "FORM"), "Item", 0), "submit")
	Sleep 5000
	loop
		If (busy := COM_Invoke(pwb, "busy") = false)
			Break

	; select and submit map resolution
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "resolution"), "Item", 2), "checked", "true")
	COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByTagName", "FORM"), "Item", 0), "submit")
	Sleep 1000
	loop
		If (busy := COM_Invoke(pwb, "busy") = false)
			Break

	; wait for download screen
	loop
	{
		If (busy := COM_Invoke(pwb, "busy") = false)
		{
			body := COM_Invoke(document, "body")
			webText := COM_Invoke(body, "innerHTML")
			IfInString webText, The terrain you generated is ready for download.
			{
				Break
			}
		}
		Sleep 1000
	}

	; download terrain file
	terrainPath = %XCSoarOutputPath%\Maps\%fileName%.xcm
	FileDelete %terrainPath%
	UrlDownloadToFile http://xcsoar.dd.com.au/cgi-bin/terrain.pl?rm=download_file, %terrainPath%

	Gui, Destroy
	COM_AtlAxWinTerm()

	return terrainPath
}


atan2(x, y)
{    ; 4-quadrant atan
   Return dllcall("msvcrt\atan2","Double",x, "Double",y, "CDECL Double")
}


; calculate haversine distance for linear distance
haversine_km(lat1, lon1, lat2, lon2)
{
	r := 6371
	d2r := 0.01745329252  ; pi/180
    dlat := (lat2 - lat1) * d2r
    dlon := (lon2 - lon1) * d2r
    a := (sin(dlat/2.0) ** 2) + cos(lat1*d2r) * cos(lat2*d2r) * (sin(dlon/2.0) ** 2)
    c := 2 * atan2(sqrt(a), sqrt(1-a))
    d := r * c

    return d
}


LK8000MapTemplate()
{
	global landscapeName
	global landscapeVersion
	global maxLat
	global minLat
	global maxLon
	global minLon
	global LK8000OutputPath

	path = %LK8000OutputPath%\Maps\%landscapeName%_%landscapeVersion%.TXT
	FileDelete %path%
	FileAppend NAME=%landscapeName%_%landscapeVersion%`n, %path%
	FileAppend DIR=CONDOR`n, %path%
	FileAppend `n, %path%

	lonMin := Round(Floor(minLon * 10) / 10, 1)
	FileAppend LONMIN=%lonMin%`n, %path%
	lonMax := Round(Ceil(maxLon * 10) / 10, 1)
	FileAppend LONMAX=%lonMax%`n, %path%
	latMin := Round(Floor(minLat * 10) / 10, 1)
	FileAppend LATMIN=%latMin%`n, %path%
	latMax := Round(Ceil(maxLat * 10) / 10, 1)
	FileAppend LATMAX=%latMax%`n, %path%
	FileAppend `n, %path%

	x := haversine_km(latMax, lonMin, latMax, lonMax)
	y := haversine_km(latMin, lonMin, latMax, lonMin)
	area := x * y

	res1000 = NO
	res500 = NO
	res250 = NO
	If(area < 270*270)
		res250 = YES
	Else If(area < 540*540)
		res500 = YES
	Else
		res1000 = YES

	FileAppend RES1000=%res1000%`n, %path%
	FileAppend RES500=%res500%`n, %path%
	FileAppend RES250=%res250%`n, %path%
	FileAppend RES90=NO`n, %path%
	FileAppend `n, %path%

	FileAppend TOPOLOGY=YES`n, %path%
	FileAppend XTOPOLOGY=YES`n, %path%
	
	If(lonMin < -30)
	{
		If(latMin > 10)
			mapzone = NAM
		Else
			mapzone = SAM
	}
	Else If(latMin < 35 && lonMin < 50)
		mazone = AFR
	Else If(latMin < -10)
		mapzone = AUS
	Else If((latMin >= 35 && lonMin < 40) || (latMin > 50))
		mapzone = EUR
	Else
		mapzone = ASO
    FileAppend MAPZONE=%mapzone%`n, %path%
    FileAppend `n, %path%

	FileAppend #REM: Condor landscape: %landscapeName%_%landscapeVersion%`n, %path%
	
	return path
}



; ************************ D E P E N D E N C Y   C H E C K S *******************************

RegRead condorDir, HKEY_CURRENT_USER, SOFTWARE\Condor, InstallDir
If ErrorLevel
{
	MsgBox Condor not installed!!!
	ExitApp
}

RegRead landscapeEditorPath, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Condor Scenery Toolkit, DisplayIcon
if ErrorLevel
{
	MsgBox Condor Scenery Toolkit not installed!!!
	ExitApp
}


RegRead zipInstPath, HKEY_CURRENT_USER, SOFTWARE\7-Zip, Path
If ErrorLevel
{
	MsgBox 7-zip not installed!!!
	ExitApp
}
zipAppPath = %zipInstPath%\7z.exe



; ************************ G E T   L A N D S C A P E   I N F O R M A T I O N *******************************

; create a list of all installed landscapes
Loop %condorDir%\Landscapes\*, 2
{
	If lanscapesList =
	{
		lanscapesList = %A_LoopFileName%
	}
	else
	{
		lanscapesList = %lanscapesList%|%A_LoopFileName%
	}
}

; ask which landscape should be translated
Gui, Add, Text,, Please select desired landscape:
Gui, Add, DropDownList, VlandscapeName wp, %lanscapesList%
Gui, Add, Checkbox, VXCSoar Checked, XCSoar
Gui, Add, Checkbox, VLK8000 Checked, LK8000
Gui, Add, Button, default wp, OK
Gui, Show
Return

GuiClose:
ButtonOK:
Gui, Submit            ; Save the input from the user to each control's associated variable.
Gui, Destroy

If landscapeName =
{
    MsgBox You didn't select any landscape. Exiting...
	ExitApp
}

If (XCSoar == 0 && LK8000 == 0)
{
    MsgBox You didn't select any translation. Exiting...
	ExitApp
}

; obtain scenery version
IniRead landscapeVersion, %condorDir%\Landscapes\%landscapeName%\%landscapeName%.ini, General, Version


; ************************ G E T   L A N D S C A P E   A I R P O R T S *******************************

datPath = %XCSoarOutputPath%\Waypoints\%landscapeName%_%landscapeVersion%.dat
FileDelete %datPath%

cupPath = %LK8000OutputPath%\Waypoints\%landscapeName%_%landscapeVersion%.cup
FileDelete %cupPath%
csvLine := "name,code,country,lat,lon,elev,style,rwdir,rwlen,freq,desc"
FileAppend %csvline%`n, %cupPath%

index = 1
SetControlDelay 50

IfWinNotExist %landscapeEditorTitle%
{
	; Open landscape editor
	Run %landscapeEditorPath%

	WinWait %landscapeEditorTitle%, , 3
	if ErrorLevel
	{
		MsgBox Condor Scenery Toolkit not installed!!!
		ExitApp
	}
}
WinActivate %landscapeEditorTitle%
WinWaitActive %landscapeEditorTitle%

; set selected landscape
Control ChooseString, %landscapeName%, TComboBox1, %landscapeEditorTitle%
WinWait Loading textures
WinWaitClose

; select Airports
Control ChooseString, Airports, TCheckListBox1, %landscapeEditorTitle%

; iterate through airports
ControlFocus TListBox1, %landscapeEditorTitle%
ControlGet airportsList, List, , TListBox1, %landscapeEditorTitle%
Loop Parse, airportsList, `n
{
	Control Choose, %A_Index%, TListBox1, %landscapeEditorTitle%
	Send {APPSKEY}{UP}{ENTER}

	WinWaitActive %landscapeEditorAirportPropertiesTitle%

	ControlGetText name,      TEdit3, %landscapeEditorAirportPropertiesTitle%
	ControlGetText lat,       TEdit2, %landscapeEditorAirportPropertiesTitle%
	ControlGetText lon,       TEdit1, %landscapeEditorAirportPropertiesTitle%
	ControlGetText altitude,  TSpinEdit6, %landscapeEditorAirportPropertiesTitle%
	ControlGetText direction, TSpinEdit5, %landscapeEditorAirportPropertiesTitle%
	ControlGetText length,    TSpinEdit4, %landscapeEditorAirportPropertiesTitle%
	ControlGetText width,     TSpinEdit3, %landscapeEditorAirportPropertiesTitle%
	ControlGet     asphalt,   Checked, , TCheckBox4, %landscapeEditorAirportPropertiesTitle%
	ControlGetText freqHigh,  TSpinEdit2, %landscapeEditorAirportPropertiesTitle%
	ControlGetText freqLow,   TSpinEdit1, %landscapeEditorAirportPropertiesTitle%

	cupName = "%name%"

	; transform latitiude
	if(lat >= 0)
		latLetter = N
	else
		latLetter = S
	lat := Abs(lat)
	latDeg := Floor(lat)
	Loop, % 2 - StrLen(latDeg) ; prepend with zeros if necessary
		latDeg = 0%latDeg%
	latMin := Round((lat - latDeg) * 60, 3)
	Loop, % 6 - StrLen(latMin) ; prepend with zeros if necessary
		latMin = 0%latMin%
	datLat := latDeg . ":" . latMin . latLetter
	cupLat := latDeg . latMin . latLetter

	; transform longitude
	if(lon >= 0)
		lonLetter = E
	else
		lonLetter = W
	lon := Abs(lon)
	lonDeg := Floor(lon)
	Loop, % 3 - StrLen(lonDeg) ; prepend with zeros if necessary
		lonDeg = 0%lonDeg%
	lonMin := Round((lon - lonDeg) * 60, 3)
	Loop, % 6 - StrLen(lonMin) ; prepend with zeros if necessary
		lonMin = 0%lonMin%
	datLon := lonDeg . ":" . lonMin . lonLetter
	cupLon := lonDeg . lonMin . lonLetter

	datAltitude = %altitude%M
	cupAltitude = %altitude%.0m
	cupLength = %length%.0m

	if(direction > 180)
		datDirection := (direction - 180) . "/" . direction
	else
		datDirection := direction . "/" . (direction + 180)

	if(asphalt = 1)
		wpstyle = 5
	else
		wpstyle = 2
		
	csvLine := index . "," . datLat . "," . datLon . "," . datAltitude . ",AT," . name . ", Dir: " . datDirection . " Dim: " . length . "/" . width
	FileAppend %csvline%`n, %datPath%

	csvLine := cupName . ",,," . cupLat . "," . cupLon . "," . cupAltitude . "," . wpstyle . "," . direction . "," . cupLength . "," . freqHigh . "." . freqLow . ","
	FileAppend %csvline%`n, %cupPath%
	
	ControlSend TButton1, {ENTER}, %landscapeEditorAirportPropertiesTitle%
	WinWaitClose %landscapeEditorAirportPropertiesTitle%
	
	index++
}

WinClose %landscapeEditorTitle%

airportsNum := index - 1


; ************************ G E T   L A N D S C A P E   W A Y P O I N T S *******************************

; process landscape turnpoints
Loop READ, %condorDir%\Landscapes\%landscapeName%\%landscapeName%.cup
{
	If(A_Index != 1)
	{
		FileAppend %A_LoopReadLine%`n, %cupPath%

		searchStr = "
		pos := InStr(A_LoopReadLine, searchStr, false, 2)
		StringLeft name, A_LoopReadLine, pos
		; crop name
		StringTrimLeft name, name, 1
		StringTrimRight name, name, 1
		StringReplace name, name, `,

		StringTrimLeft rest, A_LoopReadLine, pos + 1
		StringSplit, data, rest, `,
		If(data0 < 5) {
			MsgBox Error parsing Condor Landscape waypoints file '%condorDir%\Landscapes\%landscapeName%\%landscapeName%.cup'!!!
			ExitApp
		}

		; transform latitiude
		latLetter := SubStr(data3, 0, 1)
		StringTrimRight lat, data3, 1
		latDeg := Floor(lat / 100)
		StringTrimLeft latMin, lat, 2
		lat := latDeg . ":" . latMin . latLetter

		; transform longitude
		lonLetter := SubStr(data4, 0, 1)
		StringTrimRight lon, data4, 1
		lonDeg := Floor(lon / 100)
		StringTrimLeft lonMin, lon, 3
		lon := lonDeg . ":" . lonMin . lonLetter

		altitude := Round(data5) . "M"

		; check if airport with the same coordinates is present already
		skip := false
		Loop READ, %datPath%
		{
			StringSplit, data, A_LoopReadLine, `,
			If(data2 = lat && data3 = lon)
			{
				; airport found -> skip the waypoint
				skip := true
				Break
			}

			If(A_Index = airportsNum)
				Break
		}
	
		If(skip = false)
		{
			; write waypoint to file
			csvLine := index . "," . lat . "," . lon . "," . altitude . ",T," . name . ","
			FileAppend %csvline%`n, %datPath%
			index++
		}
	}
}


; ************************ G E N E R A T E   L A N D S C A P E   T E R R A I N *******************************

; resolve scenery extremum coordinates
dllName = %condorDir%\NaviCon.dll
trnPath = %condorDir%\Landscapes\%landscapeName%\%landscapeName%.trn

hModule := DllCall("LoadLibrary", Str, dllName)  ; Avoids the need for DllCall() in the loop to load the library.
If ErrorLevel
{
	MsgBox Error '%ErrorLevel%' returned while loading library '%dllName%'!!!
	ExitApp
}

DllCall("NaviCon\NaviConInit", Str, trnPath)
maxX := DllCall("NaviCon\GetMaxX", Float)
maxY := DllCall("NaviCon\GetMaxY", Float)

minLon1 := DllCall("NaviCon\XYToLon", Float, maxX, Float, maxY, Float)
minLon2 := DllCall("NaviCon\XYToLon", Float, maxX, Float, "0", Float)
minLon := Min(minLon1, minLon2)

minLat1 := DllCall("NaviCon\XYToLat", Float, "0", Float, "0", Float)
minLat2 := DllCall("NaviCon\XYToLat", Float, maxX, Float, "0", Float)
minLat := Min(minLat1, minLat2)

maxLon1 := DllCall("NaviCon\XYToLon", Float, "0", Float, "0", Float)
maxLon2 := DllCall("NaviCon\XYToLon", Float, "0", Float, maxY, Float)
maxLon := Max(maxLon1, maxLon2)

maxLat1 := DllCall("NaviCon\XYToLat", Float, maxX, Float, maxY, Float)
maxLat2 := DllCall("NaviCon\XYToLat", Float, "0", Float, maxY, Float)
maxLat := Max(maxLat1, maxLat2)

DllCall("FreeLibrary", "UInt", hModule)  ; To conserve memory, the DLL may be unloaded after using it.

If %XCSoar%
{
	#Include include\COM.ahk
	XCSoarXcmPath := XCSoarTerrainDownload()

	; ************************ R E P A C K   T E R R A I N   F I L E S *******************************

	XCSoarZipPath := RegExReplace(XCSoarXcmPath, "\.xcm$", ".zip")
	XCSoarWaypointsPath = %XCSoarOutputPath%\Maps\waypoints.xcw

	FileCopy %datPath%, %XCSoarWaypointsPath%
	FileMove %XCSoarXcmPath%, %XCSoarZipPath%

	; remove airspaces
	RunWait %zipAppPath% d "%XCSoarZipPath%" airspace.txt

	; add waypoints
	RunWait %zipAppPath% a "%XCSoarZipPath%" "%XCSoarWaypointsPath%"

	FileDelete %XCSoarWaypointsPath%
	FileMove %XCSoarZipPath%, %XCSoarXcmPath%
}
If %LK8000%
{
	LK8000TemplatePath := LK8000MapTemplate()
}


; ************************ F I N I S H *******************************

MsgBox Landscape translation completed successfully!!!`n`nXCSoar map file: %XCSoarXcmPath%`nXCSoar waypoints file: %datPath%`nLK8000 template file: %LK8000TemplatePath%`nLK8000 waypoints file: %cupPath%

ExitApp

