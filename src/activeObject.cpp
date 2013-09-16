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
 * @file activeObject.cpp
 *
 * @brief Active object.
 */

#include "activeObject.h"


condor2nav::CActiveObject::CActiveObject() :
  _thread{[this]{ Run(); }}
{
}


condor2nav::CActiveObject::~CActiveObject()
{
  Send([this]{ _done = true; });
  _thread.join();
}


void condor2nav::CActiveObject::Send(CMessage msg)
{
  _msgQueue.Push(std::move(msg));
}


void condor2nav::CActiveObject::Run()
{
  while(!_done) {
    CMessage msg = _msgQueue.PopWait();
    msg();            // execute message
  } // note: last message sets done to true
}
