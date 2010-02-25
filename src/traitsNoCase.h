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
 * @file traitsNoCase.h
 *
 * @brief Declares the No-case Traits. 
**/

#ifndef __TRAITSNOCASE_H__
#define __TRAITSNOCASE_H__

#include <cctype>
#include <iostream>

namespace condor2nav {

  struct CTraitsNoCase : std::char_traits<char> {
    static bool eq(const char &c1, const char &c2)
    {
      return std::toupper(c1) == std::toupper(c2);
    }

    static bool lt(const char &c1, const char &c2)
    {
      return toupper(c1) < toupper(c2);
    }

    static int compare(const char *s1, const char *s2, size_t N)
    {
      return _strnicmp(s1, s2, N);
    }

    static const char *find(const char *s, size_t N, const char &a)
    {
      for(size_t i=0; i<N; ++i)
        if(toupper(s[i]) == toupper(a))
          return s+i;
      return 0 ;
    }

    static bool eq_int_type(const int_type &c1, const int_type &c2)
    {
      return toupper(c1) == toupper(c2);
    }
  };

  typedef std::basic_string<char, CTraitsNoCase> CStringNoCase;

  inline std::ostream &operator<<(std::ostream &os, const CStringNoCase &str)
  {
    return os << std::string(str.c_str(), str.length());
  }
  
  inline std::istream &operator>>(std::istream &is, CStringNoCase &str)
  {
    std::string s;
    is >> s;
    if(is)
      str.assign(s.begin(), s.end());
    return is;
  }
  
  inline std::istream &getline(std::istream &is, CStringNoCase &str)
  {
    std::string s;
    std::getline(is, s);
    if(is)
      str.assign(s.begin(), s.end());
    return is;
  }

  inline const CStringNoCase &operator+(const CStringNoCase &str1, const std::string &str2)
  {
    CStringNoCase str(str1);
    return str.append(str2.begin(), str2.end());
  }

  inline const CStringNoCase &operator+(const std::string &str1, const CStringNoCase &str2)
  {
    CStringNoCase str(str1.begin(), str1.end());
    return str += str2;
  }

}

#endif /* __TRAITSNOCASE_H__ */