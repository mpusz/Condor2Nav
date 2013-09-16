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
 * @file waitQueue.h
 *
 * @brief Thread safe waiting queue.
 */

#ifndef __WAITQUEUE_H__
#define __WAITQUEUE_H__

#include "nonCopyable.h"
#include <queue>
#include <thread>
#include <condition_variable>

namespace condor2nav {

  template<typename T>
  class CWaitQueue : CNonCopyable {
    std::queue<T> _queue;
    std::condition_variable _newItemReady;
    std::mutex _mutex;
  public:
    CWaitQueue() {}
    void Push(T msg)
    {
      bool wasEmpty;
      {
        std::lock_guard<std::mutex> lock{_mutex};
        wasEmpty = _queue.empty();
        _queue.push(std::move(msg));
      }
      if(wasEmpty)
        _newItemReady.notify_one();
    }
    T PopWait()
    {
      std::unique_lock<std::mutex> lock{_mutex};
      _newItemReady.wait(lock, [&]{ return _queue.size(); });
      T msg = std::move(_queue.front());
      _queue.pop();
      return msg;
    }
  };

}

#endif /* __WAITQUEUE_H__ */
