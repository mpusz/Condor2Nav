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
 * @file traitsNoCase.h
 *
 * @brief Declares the No-case Traits. 
 */

#ifndef __TRAITSNOCASE_H__
#define __TRAITSNOCASE_H__

#include <string>
#include <cctype>
#include <iostream>

namespace condor2nav {

  /**
   * @brief Case-insensitive traits for strings. 
   *
   * Class implements case-insensitive traits for strings that should be compared
   * with any case.
   */
  struct CTraitsNoCase : std::char_traits<char> {

    /**
     * @brief Compares 2 characters in case-insensitive manner.
     *
     * @param c1 The first character to compare. 
     * @param c2 The second character to compare. 
     *
     * @return true if both characters are the same.
     */
    static bool eq(const char &c1, const char &c2)
    {
      return std::toupper(c1) == std::toupper(c2);
    }

    /**
    * @brief Compares 2 characters in case-insensitive manner.
    *
    * @param c1 The first character to compare. 
    * @param c2 The second character to compare. 
    *
    * @return true if character 1 is smaller than character 2.
    */
    static bool lt(const char &c1, const char &c2)
    {
      return toupper(c1) < toupper(c2);
    }


    /**
     * @brief Compares 2 strings in case-insensitive manner.
     *
     * @param s1 The first string to compare. 
     * @param s2 The second string to compare. 
     * @param N  Size of the string to be compared. 
     *
     * @return Negative if 's1' is less than 's2', 0 if they are equal, or positive if it is
     *         greater. 
     */
    static int compare(const char *s1, const char *s2, size_t N)
    {
      return _strnicmp(s1, s2, N);
    }


    /**
     * @brief Finds a character inside provided string.
     *
     * @param s The string to search in. 
     * @param N The number of positions, counting from the first, in the range to be searched.
     * @param a The character to find. 
     *
     * @return A pointer to the first occurrence of the specified character in the range
     *         if a match is found; otherwise, a null pointer.
     */
    static const char *find(const char *s, size_t N, const char &a)
    {
      for(size_t i=0; i<N; ++i)
        if(toupper(s[i]) == toupper(a))
          return s+i;
      return 0;
    }

    /**
     * @brief Tests whether two characters represented as int_types are equal or not. 
     *
     * @param c1 The first of the two characters to be tested for equality as int_types.
     * @param c2 The second of the two characters to be tested for equality as int_types.
     *
     * @return true if the first character is equal to the second character; otherwise false. 
     */
    static bool eq_int_type(const int_type &c1, const int_type &c2)
    {
      return toupper(c1) == toupper(c2);
    }
  };


  /**
   * @brief Case-insensitive string.
   */
  typedef std::basic_string<char, CTraitsNoCase> CStringNoCase;


  /**
   * @brief Dumps case-insensitive string to a stream. 
   *
   * @param os   Output stream
   * @param str  Case-insensitive string
   *
   * @return Output stream. 
   */
  inline std::ostream &operator<<(std::ostream &os, const CStringNoCase &str)
  {
    return os << std::string(str.c_str(), str.length());
  }
  

  /**
  * @brief Reads case-insensitive string from a stream. 
  *
  * @param is   Input stream
  * @param str  Case-insensitive string
  *
  * @return Input stream. 
  */
  inline std::istream &operator>>(std::istream &is, CStringNoCase &str)
  {
    std::string s;
    is >> s;
    if(is)
      str.assign(s.begin(), s.end());
    return is;
  }
  

  /**
   * @brief Puts the line from the stream to the case-insensitive string. 
   *
   * @param [in,out] is  The input stream. 
   * @param [in,out] str The case-insensitive string. 
   *
   * @return Input stream. 
   */
  inline std::istream &getline(std::istream &is, CStringNoCase &str)
  {
    std::string s;
    std::getline(is, s);
    if(is)
      str.assign(s.begin(), s.end());
    return is;
  }


  /**
   * @brief Addition operator that joins case-insensitive string with regular string. 
   *
   * @param str1 The case-insensitive string. 
   * @param str2 The regular string. 
   *
   * @return Resulting case-insensitive string.
   */
  inline const CStringNoCase &operator+(const CStringNoCase &str1, const std::string &str2)
  {
    CStringNoCase str(str1);
    return str.append(str2.begin(), str2.end());
  }


  /**
   * @brief Addition operator that joins case-insensitive string with regular string. 
   *
   * @param str1 The regular string. 
   * @param str2 The case-insensitive string. 
   *
   * @return Resulting case-insensitive string. 
   */
  inline const CStringNoCase &operator+(const std::string &str1, const CStringNoCase &str2)
  {
    CStringNoCase str(str1.begin(), str1.end());
    return str += str2;
  }

}

#endif /* __TRAITSNOCASE_H__ */