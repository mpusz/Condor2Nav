;
; AutoHotkey Version: 1.x
; Language:       English
; Platform:       Win9x/NT
; Author:         A.N.Other <myemail@nowhere.com>
;
; Script Function:
;	Template script (you can customize this template by editing "ShellNew\Template.ahk" in your Windows folder)
;

#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

landscapeEditorTitle = Condor Scenery Toolkit: Landscape Editor
lanlandscapeEditorAirportPropertiesTitle = Airport properties

RegRead condorPath, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Condor: The Competition Soaring Simulator, DisplayIcon
If ErrorLevel
{
	MsgBox Condor not installed!!!
	ExitApp
}
StringGetPos, pos, condorPath, Condor.exe
condorDir := SubStr(condorPath, 1, pos)

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

If landscapeName =
{
    MsgBox You didn't select any landscape. Exiting...
	ExitApp
}


; obtain scenery version
IniRead landscapeVersion, %condorDir%\Landscapes\%landscapeName%\%landscapeName%.ini, General, Version

; prepare new file
filePath = ..\data\Waypoints\%landscapeName% - %landscapeVersion%.csv
FileDelete %filePath%

SetControlDelay 50

IfWinNotExist %landscapeEditorTitle%
{
	; Open landscape editor
	RegRead landscapeEditorPath, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Condor Scenery Toolkit, DisplayIcon
	if ErrorLevel
	{
		MsgBox Condor Scenery Toolkit not installed!!!
		ExitApp
	}

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

index = 1

; iterate through airports
ControlFocus TListBox1, %landscapeEditorTitle%
ControlGet airportsList, List, , TListBox1, %landscapeEditorTitle%
Loop Parse, airportsList, `n
{
	Control Choose, %index%, TListBox1, %landscapeEditorTitle%
	Send {APPSKEY}{UP}{ENTER}

	WinWaitActive %lanlandscapeEditorAirportPropertiesTitle%

	ControlGetText name,      TEdit3, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText lat,       TEdit2, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText lon,       TEdit1, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText altitude,  TSpinEdit6, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText direction, TSpinEdit5, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText length,    TSpinEdit4, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText width,     TSpinEdit3, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGet     asphalt,   Checked, , TCheckBox4, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText freqHigh,  TSpinEdit2, %lanlandscapeEditorAirportPropertiesTitle%
	ControlGetText freqLow,   TSpinEdit1, %lanlandscapeEditorAirportPropertiesTitle%

	; transform latitiude
	if(lat>=0)
		latLetter = N
	else
		latLetter = S
	latDeg := Floor(Abs(lat))
	latMin := Round((lat - latDeg) * 60, 3)
	lat := latDeg . ":" . latMin . latLetter

	; transform longitude
	if(lon>=0)
		lonLetter = E
	else
		lonLetter = W
	lonDeg := Floor(Abs(lon))
	lonMin := Round((lon - lonDeg) * 60, 3)
	lon := lonDeg . ":" . lonMin . lonLetter

	altitude = %altitude%M

	if(direction > 180)
		direction := (direction - 180) . "/" . direction
	else
		direction := direction . "/" . (direction + 180)

	csvLine := index . "," . lat . "," . lon . "," . altitude . ",AT," . name . ", Dir: " . direction . " Dim: " . length . "/" . width . ","
	FileAppend %csvline%`n, %filePath%
	
	ControlSend TButton1, {ENTER}, %lanlandscapeEditorAirportPropertiesTitle%
	WinWaitClose %lanlandscapeEditorAirportPropertiesTitle%
	
	index++
}

WinClose %landscapeEditorTitle%

; process landscape turnpoints
Loop READ, %condorDir%\Landscapes\%landscapeName%\%landscapeName%.cup
{
	If(A_Index != 1)
	{
		StringSplit, data, A_LoopReadLine, `,
		If(data0 < 6) {
			MsgBox Error parsing Condor Landscape waypoints file '%condorDir%\Landscapes\%landscapeName%\%landscapeName%.cup'!!!
			ExitApp
		}

		; transform latitiude
		latLetter := SubStr(data4, 0, 1)
		StringTrimRight lat, data4, 1
		latDeg := Floor(lat / 100)
		StringTrimLeft latMin, lat, 2
		lat := latDeg . ":" . latMin . latLetter

		; transform longitude
		lonLetter := SubStr(data5, 0, 1)
		StringTrimRight lon, data5, 1
		lonDeg := Floor(lon / 100)
		StringTrimLeft lonMin, lon, 3
		lon := lonDeg . ":" . lonMin . lonLetter

		altitude := Round(data6) . "M"

		csvLine := index . "," . lat . "," . lon . "," . altitude . ",T," . data1 . ","
		FileAppend %csvline%`n, %filePath%

		index++
	}
}

MsgBox Landscape translation completed successfully!!!`n'%filePath%' file generated.
ExitApp
