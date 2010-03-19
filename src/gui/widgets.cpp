//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2010 Mateusz Pusz
//
// Condor2Nav is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Condor2Nav is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Condor2Nav. If not, see <http://www.gnu.org/licenses/>.
//
// Visit the project webpage (http://sf.net/projects/condor2nav) for more info.
//

/**
* @file gui\widgets.cpp
*
* @brief Implements the widgets classes. 
*/

#include "stdafx.h"
#include "widgets.h"
#include <richedit.h>
#include <memory>


/**
 * @brief Class constructor.
 *
 * @param hwndParent Handle of the control's parent. 
 * @param id         Windows control identifier. 
 * @param disabled   Specifies if a widget should be initially disabled. 
 */
condor2nav::gui::CWidget::CWidget(HWND hwndParent, int id, bool disabled /*= false*/):
_hWnd(GetDlgItem(hwndParent, id))
{
  if(disabled)
    Disable();
}


/**
 * @brief Class destructor. 
 */
condor2nav::gui::CWidget::~CWidget()
{

}


/**
 * @brief Stes focus on that object.
 */
void condor2nav::gui::CWidget::Focus() const
{
  ::SetFocus(_hWnd);
}


/**
 * @brief Enables this object.
 */
void condor2nav::gui::CWidget::Enable() const
{
  ::EnableWindow(_hWnd , TRUE);
}


/**
 * @brief Disables this object. 
 */
void condor2nav::gui::CWidget::Disable() const
{
  ::EnableWindow(_hWnd , FALSE);
}


/**
 * @brief Class constructor. 
 *
 * @param hwndParent Handle of the control's parent. 
 * @param id         Windows control identifier. 
 * @param disabled   Specifies if a widget should be initially disabled. 
 */
condor2nav::gui::CWidgetButton::CWidgetButton(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}


/**
* @brief Generates button click event. 
*/
void condor2nav::gui::CWidgetButton::Click() const
{
  SendMessage(Hwnd(), BM_CLICK, 0, 0);
}


/**
* @brief Class constructor. 
*
* @param hwndParent Handle of the control's parent. 
* @param id         Windows control identifier. 
* @param disabled   Specifies if a widget should be initially disabled. 
*/
condor2nav::gui::CWidgetRadioButton::CWidgetRadioButton(HWND hwndParent, int id, bool disabled /*= false*/):
CWidgetButton(hwndParent, id, disabled)
{

}


/**
 * @brief Returns selection state of this object. 
 *
 * @return Selection state of this object. 
 */
bool condor2nav::gui::CWidgetRadioButton::Selected() const
{
  return SendMessage(Hwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}


/**
 * @brief Selects this object. 
 */
void condor2nav::gui::CWidgetRadioButton::Select() const
{
  SendMessage(Hwnd(), BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
}


/**
* @brief Class constructor. 
*
* @param hwndParent Handle of the control's parent. 
* @param id         Windows control identifier. 
* @param disabled   Specifies if a widget should be initially disabled. 
*/
condor2nav::gui::CWidgetEdit::CWidgetEdit(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}


/**
 * @brief Sets provided string inside the widget. 
 *
 * @param str The string to set. 
 */
void condor2nav::gui::CWidgetEdit::String(const std::string &str) const
{
  SendMessage(Hwnd(), WM_SETTEXT, 0, (LPARAM)str.c_str());
}


/**
 * @brief Returns the text of this object. 
 *
 * @return The text of this object
 */
std::string condor2nav::gui::CWidgetEdit::String() const
{
  unsigned len = static_cast<unsigned>(SendMessage (Hwnd(), WM_GETTEXTLENGTH, 0, 0));
  if(len == 0)
    return "";
  
  std::auto_ptr<char> buf(new char[len + 1]);
  SendMessage(Hwnd(), WM_GETTEXT, (WPARAM)(len + 1), (LPARAM)buf.get());
  return buf.release();
}



/**
* @brief Class constructor. 
*
* @param hwndParent Handle of the control's parent. 
* @param id         Windows control identifier. 
* @param disabled   Specifies if a widget should be initially disabled. 
*/
condor2nav::gui::CWidgetComboBox::CWidgetComboBox(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}


/**
 * @brief Adds an entry to ComboBox.
 *
 * @param str The text to add. 
 */
void condor2nav::gui::CWidgetComboBox::Add(const std::string &str) const
{
  SendMessage(Hwnd(), CB_ADDSTRING, 0, (LPARAM)str.c_str());
}


/**
 * @brief Returns the selected text. 
 *
 * @return Selected text. 
 */
std::string condor2nav::gui::CWidgetComboBox::Selection() const
{
  const unsigned bufSize = 255;
  char buf[bufSize];
  SendMessage(Hwnd(), WM_GETTEXT, (WPARAM)bufSize, (LPARAM)buf);
  return buf;
}


/**
 * @brief Checks if any item is selected. 
 *
 * @return true if it succeeds, false if it fails. 
 */
bool condor2nav::gui::CWidgetComboBox::ItemSelected() const
{
  return SendMessage(Hwnd(), CB_GETCURSEL, 0, 0) != CB_ERR;
}


/**
 * @brief Sets provided string inside the widget. 
 *
 * @param str The string to set. 
 */
void condor2nav::gui::CWidgetComboBox::String(const std::string &str) const
{
  SendMessage(Hwnd(), WM_SETTEXT, 0, (LPARAM)str.c_str());
}



/**
* @brief Class constructor. 
*
* @param hwndParent Handle of the control's parent. 
* @param id         Windows control identifier. 
* @param disabled   Specifies if a widget should be initially disabled. 
*/
condor2nav::gui::CWidgetRichEdit::CWidgetRichEdit(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled),
_effectMask(EFFECT_NONE), _color(COLOR_AUTO)
{

}


/**
 * @brief Clears this object to its blank/initial state. 
 */
void condor2nav::gui::CWidgetRichEdit::Clear()
{
  SendMessage(Hwnd(), WM_SETTEXT, 0, (LPARAM)"");
  Format(EFFECT_NONE, COLOR_AUTO);
}


/**
 * @brief Sets text format. 
 *
 * @param effectMask Text effect mask. 
 * @param color      Text color. 
 *
 * @exception EOperationFailed Thrown when operation failed to execute. 
 */
void condor2nav::gui::CWidgetRichEdit::Format(unsigned effectMask, TColor color)
{
  if(effectMask == _effectMask && color == _color)
    return;

  CHARFORMAT2 format;
  ZeroMemory(&format, sizeof(format));
  format.cbSize = sizeof(CHARFORMAT2);
  format.dwMask = CFM_BOLD | CFM_ITALIC | CFM_COLOR;
  
  if(effectMask & EFFECT_BOLD)
    format.dwEffects |= CFE_BOLD;
  if(effectMask & EFFECT_ITALIC)
    format.dwEffects |= CFE_ITALIC;
  switch(color) {
  case COLOR_AUTO:
    format.dwEffects |= CFE_AUTOCOLOR;
    break;
  case COLOR_RED:
    format.crTextColor = RGB(255, 0, 0);
    break;
  case COLOR_GREEN:
    format.crTextColor = RGB(0, 255, 0);
    break;
  case COLOR_BLUE:
    format.crTextColor = RGB(0, 0, 255);
    break;
  case COLOR_BLACK:
    format.crTextColor = RGB(0, 0, 0);
    break;
  default:
    throw EOperationFailed("Unsupported color specified (" + Convert(color) + "!!!");
  }

  SendMessage(Hwnd(), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format);
  _effectMask = effectMask;
  _color = color;
}


/**
 * @brief Appends provided text.
 *
 * @param text The text to append
 */
void condor2nav::gui::CWidgetRichEdit::Append(const std::string &text)
{
  CHARRANGE cr;
  cr.cpMin = -1;
  cr.cpMax = -1;
  SendMessage(Hwnd(), EM_EXSETSEL, 0, (LPARAM)&cr);
  SendMessage(Hwnd(), EM_REPLACESEL, 0, (LPARAM)text.c_str());
  SendMessage(Hwnd(), WM_VSCROLL, SB_BOTTOM, NULL);
}
