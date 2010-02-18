//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009 Mateusz Pusz
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


condor2nav::gui::CWidget::CWidget(HWND hwndParent, int id, bool disabled /*= false*/):
_hWnd(GetDlgItem(hwndParent, id))
{
  if(disabled)
    Disable();
}


void condor2nav::gui::CWidget::Focus() const
{
  ::SetFocus(_hWnd);
}


void condor2nav::gui::CWidget::Enable() const
{
  ::EnableWindow(_hWnd , TRUE);
}


void condor2nav::gui::CWidget::Disable() const
{
  ::EnableWindow(_hWnd , FALSE);
}



condor2nav::gui::CWidgetButton::CWidgetButton(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}



condor2nav::gui::CWidgetRadioButton::CWidgetRadioButton(HWND hwndParent, int id, bool disabled /*= false*/) : CWidgetButton(hwndParent, id, disabled)
{

}


bool condor2nav::gui::CWidgetRadioButton::Selected() const
{
  return SendMessage(Hwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}


void condor2nav::gui::CWidgetRadioButton::Select() const
{
  SendMessage(Hwnd(), BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
}


condor2nav::gui::CWidgetEdit::CWidgetEdit(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}


void condor2nav::gui::CWidgetEdit::String(const std::string &str) const
{
  SendMessage(Hwnd(), WM_SETTEXT, 0, (LPARAM)str.c_str());
}


std::string condor2nav::gui::CWidgetEdit::String() const
{
  unsigned len = static_cast<unsigned>(SendMessage (Hwnd(), WM_GETTEXTLENGTH, 0, 0));
  if(len == 0)
    return "";
  
  std::auto_ptr<char> buf(new char[len + 1]);
  SendMessage(Hwnd(), WM_GETTEXT, (WPARAM)(len + 1), (LPARAM)buf.get());
  return buf.release();
}



condor2nav::gui::CWidgetComboBox::CWidgetComboBox(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}


void condor2nav::gui::CWidgetComboBox::Add(const std::string &str) const
{
  SendMessage(Hwnd(), CB_ADDSTRING, 0, (LPARAM)str.c_str());
}


std::string condor2nav::gui::CWidgetComboBox::Selection() const
{
  const unsigned bufSize = 255;
  char buf[bufSize];
  SendMessage(Hwnd(), WM_GETTEXT, (WPARAM)bufSize, (LPARAM)buf);
  return buf;
}


bool condor2nav::gui::CWidgetComboBox::ItemSelected() const
{
  return SendMessage(Hwnd(), CB_GETCURSEL, 0, 0) != CB_ERR;
}


condor2nav::gui::CWidgetRichEdit::CWidgetRichEdit(HWND hwndParent, int id, bool disabled /*= false*/):
CWidget(hwndParent, id, disabled)
{

}


void condor2nav::gui::CWidgetRichEdit::Clear()
{
  SendMessage(Hwnd(), WM_SETTEXT, 0, (LPARAM)"");
}


void condor2nav::gui::CWidgetRichEdit::Format(unsigned effectMask, TColor color)
{
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
    throw std::out_of_range("Unsupported color specified (" + Convert(color) + "!!!");
  }

  SendMessage(Hwnd(), EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format);
}


void condor2nav::gui::CWidgetRichEdit::Append(const std::string &text)
{
  CHARRANGE cr;
  cr.cpMin = -1;
  cr.cpMax = -1;
  SendMessage(Hwnd(), EM_EXSETSEL, 0, (LPARAM)&cr);
  SendMessage(Hwnd(), EM_REPLACESEL, 0, (LPARAM)text.c_str());
  SendMessage(Hwnd(), WM_VSCROLL, SB_BOTTOM, NULL);
}
