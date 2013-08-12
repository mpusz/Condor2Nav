//
// This file is part of Condor2Nav file formats translator.
//
// Copyright (C) 2009-2012 Mateusz Pusz
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
* @file widgets.h
*
* @brief Declares the widgets classes. 
*/

#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include "nonCopyable.h"

namespace condor2nav {

  namespace gui {

    /**
     * @brief Condor2Nav GUI widgets base
     *
     * condor2nav::gui::CWidget is a base for all Condor2Nav GUI widgets. Basically
     * their role is to be a wrapper over cumbersome Windows API interface.
     *
     * NOTE: That is not a polymorphic type
     */
    class CWidget : CNonCopyable {
      const HWND _hWnd;	                      ///< @brief Widget handler
    protected:
      HWND Hwnd() const { return _hWnd; }
    public:
      CWidget(HWND hwndParent, int id, bool disabled = false);
      ~CWidget() {}
      void Focus() const;
      void Enable() const;
      void Disable() const;
    };


    /**
     * @brief Button widget.
     *
     * Widget that handles all GUI buttons.
     */
    class CWidgetButton : public CWidget {
    public:
      CWidgetButton(HWND hwndParent, int id, bool disabled = false);
      void Click() const;
    };


    /**
     * @brief Radio button widget.
     */
    class CWidgetRadioButton : public CWidgetButton {
    public:
      CWidgetRadioButton(HWND hwndParent, int id, bool disabled = false);
      bool Selected() const;
      void Select() const;
    };


    /**
     * @brief Edit box widget.
     */
    class CWidgetEdit : public CWidget {
    public:
      CWidgetEdit(HWND hwndParent, int id, bool disabled = false);
      void String(const std::string &str) const;
      std::string String() const;
    };


    /**
     * @brief Combo box widget.
     */
    class CWidgetComboBox : public CWidget {
    public:
      CWidgetComboBox(HWND hwndParent, int id, bool disabled = false);
      void Add(const std::string &str) const;
      std::string Selection() const;
      bool ItemSelected() const;
      void String(const std::string &str) const;
    };


    /**
     * @brief Rich edit widget. 
     */
    class CWidgetRichEdit : public CWidget {
    public:
      /**
      * @brief Values that represent text color.
      */
      enum class TColor {
        AUTO,
        RED,
        GREEN,
        BLUE,
        ORANGE,
        BLACK
      };

      /**
      * @brief Values that represent text effects.
      */
      enum TEffect {
        EFFECT_NONE   = 0x00,
        EFFECT_BOLD   = 0x01,
        EFFECT_ITALIC = 0x02,
      };

    private:
      unsigned _effectMask;	    ///< @brief Current text effect mask
      TColor _color;	        ///< @brief Current text color

    public:
      CWidgetRichEdit(HWND hwndParent, int id, bool disabled = false);
      void Clear();
      void Format(unsigned effectMask, TColor color);
      void Append(const std::string &text);
    };

  } // namespace gui

} // namespace condor2nav


#endif // __WIDGETS_H__
