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

outputPath = ..\..\data\XCSoar
landscapeEditorTitle = Condor Scenery Toolkit: Landscape Editor
landscapeEditorAirportPropertiesTitle = Airport properties



; ************************ D E P E N D E N C Y   C H E C K S *******************************

RegRead condorPath, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Condor: The Competition Soaring Simulator, DisplayIcon
If ErrorLevel
{
	MsgBox Condor not installed!!!
	ExitApp
}
StringGetPos, pos, condorPath, Condor.exe
condorDir := SubStr(condorPath, 1, pos - 1)


RegRead landscapeEditorPath, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Condor Scenery Toolkit, DisplayIcon
if ErrorLevel
{
	MsgBox Condor Scenery Toolkit not installed!!!
	ExitApp
}


RegRead zipUninstPath, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\7-Zip, UninstallString
If ErrorLevel
{
	MsgBox 7-zip not installed!!!
	ExitApp
}
StringTrimLeft zipUninstPath, zipUninstPath, 1
StringTrimRight zipUninstPath, zipUninstPath, 1
zipAppPath := RegExReplace(zipUninstPath, "Uninstall\.exe$", "7z.exe")



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

; obtain scenery version
IniRead landscapeVersion, %condorDir%\Landscapes\%landscapeName%\%landscapeName%.ini, General, Version




; ************************ G E T   L A N D S C A P E   A I R P O R T S *******************************

csvPath = %outputPath%\Waypoints\%landscapeName%_%landscapeVersion%.csv
FileDelete %csvPath%

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

	; transform latitiude
	if(lat >= 0)
		latLetter = N
	else
		latLetter = S
	lat := Abs(lat)
	latDeg := Floor(lat)
	latMin := Round((lat - latDeg) * 60, 3)
	Loop, % 6 - StrLen(latMin) ; prepend with zeros if necessary
		latMin = 0%latMin%
	lat := latDeg . ":" . latMin . latLetter

	; transform longitude
	if(lon >= 0)
		lonLetter = E
	else
		lonLetter = W
	lon := Abs(lon)
	lonDeg := Floor(lon)
	lonMin := Round((lon - lonDeg) * 60, 3)
	Loop, % 6 - StrLen(lonMin) ; prepend with zeros if necessary
		lonMin = 0%lonMin%
	lon := lonDeg . ":" . lonMin . lonLetter

	altitude = %altitude%M

	if(direction > 180)
		direction := (direction - 180) . "/" . direction
	else
		direction := direction . "/" . (direction + 180)

	csvLine := index . "," . lat . "," . lon . "," . altitude . ",AT," . name . ", Dir: " . direction . " Dim: " . length . "/" . width
	FileAppend %csvline%`n, %csvPath%
	
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
		Loop READ, %csvPath%
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
			FileAppend %csvline%`n, %csvPath%
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
minLon := DllCall("NaviCon\XYToLon", Float, "0", Float, "0", Float)
minLat := DllCall("NaviCon\XYToLat", Float, "0", Float, "0", Float)
maxX := DllCall("NaviCon\GetMaxX", Float)
maxY := DllCall("NaviCon\GetMaxY", Float)
maxLon := DllCall("NaviCon\XYToLon", Float, maxX, Float, maxY, Float)
maxLat := DllCall("NaviCon\XYToLat", Float, maxX, Float, maxY, Float)

DllCall("FreeLibrary", "UInt", hModule)  ; To conserve memory, the DLL may be unloaded after using it.


; create browser window
#Include include\COM.ahk

COM_AtlAxWinInit()
Gui, +LastFound +Resize
;pwb := COM_AtlAxGetControl(COM_AtlAxCreateContainer(WinExist(),top,left,width,height, "Shell.Explorer") )  ;left these here just for reference of the parameters
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
loop
	If (busy := COM_Invoke(pwb, "busy") = false)
		break

; fill lanscape coordinates
; transform top
If(maxLat >= 0)
	latLetter = N
else
	latLetter = S
maxLat := Abs(maxLat)
latDeg := Floor(maxLat)
latMin := Floor((maxLat - latDeg) * 60)
latSec := Round(((maxLat - latDeg) * 60 - latMin) * 60)
If(latSec = 60)
{
	latMin := latMin + 1
	latSec := 0
}

COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "top_degrees"), "Item", 0), "value", latDeg)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "top_minutes"), "Item", 0), "value", latMin)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "top_seconds"), "Item", 0), "value", latSec)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "top_direction"), "Item", 0), "value", latLetter)

; transform bottom
if(minLat >= 0)
	latLetter = N
else
	latLetter = S
minLat := Abs(minLat)
latDeg := Floor(minLat)
latMin := Floor((minLat - latDeg) * 60)
latSec := Round(((minLat - latDeg) * 60 - latMin) * 60)
If(latSec = 60)
{
	latMin := latMin + 1
	latSec := 0
}

COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "bottom_degrees"), "Item", 0), "value", latDeg)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "bottom_minutes"), "Item", 0), "value", latMin)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "bottom_seconds"), "Item", 0), "value", latSec)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "bottom_direction"), "Item", 0), "value", latLetter)

; transform left
if(maxLon >= 0)
	lonLetter = E
else
	lonLetter = W
maxLon := Abs(maxLon)
lonDeg := Floor(maxLon)
lonMin := Floor((maxLon - lonDeg) * 60)
lonSec := Round(((maxLon - lonDeg) * 60 - lonMin) * 60)
If(lonSec = 60)
{
	lonMin := lonMin + 1
	lonSec := 0
}

COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "left_degrees"), "Item", 0), "value", lonDeg)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "left_minutes"), "Item", 0), "value", lonMin)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "left_seconds"), "Item", 0), "value", lonSec)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "left_direction"), "Item", 0), "value", lonLetter)

; transform right
if(minLon >= 0)
	lonLetter = E
else
	lonLetter = W
minLon := Abs(minLon)
lonDeg := Floor(minLon)
lonMin := Floor((minLon - lonDeg) * 60)
lonSec := Floor(((minLon - lonDeg) * 60 - lonMin) * 60)
If(lonSec = 60)
{
	lonMin := lonMin + 1
	lonSec := 0
}

COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "right_degrees"), "Item", 0), "value", lonDeg)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "right_minutes"), "Item", 0), "value", lonMin)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "right_seconds"), "Item", 0), "value", lonSec)
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "right_direction"), "Item", 0), "value", lonLetter)

COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByTagName", "FORM"), "Item", 0), "submit")
Sleep 5000
loop
	If (busy := COM_Invoke(pwb, "busy") = false)
		Break

; select and submit map resolution
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByName", "resolution"), "Item", 1), "checked", "true")
COM_Invoke(COM_Invoke(COM_Invoke(document, "getElementsByTagName", "FORM"), "Item", 0), "submit")
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
xcmPath = %outputPath%\Maps\%fileName%.xcm
FileDelete %xcmPath%
UrlDownloadToFile http://xcsoar.dd.com.au/cgi-bin/terrain.pl?rm=download_file, %xcmPath%

Gui, Destroy
COM_AtlAxWinTerm()



; ************************ R E P A C K   T E R R A I N   F I L E *******************************

zipPath := RegExReplace(xcmPath, "\.xcm$", ".zip")
waypointsPath = %outputPath%\Maps\waypoints.xcw

FileCopy %csvPath%, %waypointsPath%
FileMove %xcmPath%, %zipPath%

; remove airspaces
RunWait %zipAppPath% d "%zipPath%" airspace.txt

; add waypoints
RunWait %zipAppPath% a "%zipPath%" "%waypointsPath%"

FileDelete %waypointsPath%
FileMove %zipPath%, %xcmPath%



; ************************ F I N I S H *******************************

MsgBox Landscape translation completed successfully!!!`nMap file: %xcmPath%`nWaypoints file: %csvPath%

ExitApp
