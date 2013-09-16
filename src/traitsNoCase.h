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
#include <cwctype>

namespace condor2nav {

  inline int    ToUpper(char ch)    { return std::toupper(ch); }
  inline wint_t ToUpper(wchar_t ch) { return std::towupper(ch); }
  
  /**
   * @brief Case-insensitive traits for strings. 
   *
   * Class implements case-insensitive traits for strings that should be compared
   * with any case.
   */
  template<typename Char>
  struct CTraitsNoCase : std::char_traits<Char> {

    /**
     * @brief Compares 2 characters in case-insensitive manner.
     *
     * @param c1 The first character to compare. 
     * @param c2 The second character to compare. 
     *
     * @return true if both characters are the same.
     */
    static bool eq(const Char &c1, const Char &c2)
    {
      return ToUpper(c1) == ToUpper(c2);
    }

    /**
    * @brief Compares 2 characters in case-insensitive manner.
    *
    * @param c1 The first character to compare. 
    * @param c2 The second character to compare. 
    *
    * @return true if character 1 is smaller than character 2.
    */
    static bool lt(const Char &c1, const Char &c2)
    {
      return ToUpper(c1) < ToUpper(c2);
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
    static int compare(const Char *s1, const Char *s2, size_t N)
    {
      for(size_t i = 0; i < N; ++i) {
        if(lt(s1[i], s2[i]))
          return -1;
        if(lt(s2[i], s1[i]))
          return +1;
      }
      return 0;
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
    static const Char *find(const Char *s, size_t N, const Char &a)
    {
      for(size_t i=0; i<N; ++i)
        if(ToUpper(s[i]) == ToUpper(a))
          return s+i;
      return 0;
    }
  };


  /**
   * @brief Case-insensitive string.
   */
  using CStringNoCase  = std::basic_string<char, CTraitsNoCase<char>>;
  using CWStringNoCase = std::basic_string<wchar_t, CTraitsNoCase<wchar_t>>;


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
      str.assign(begin(s), end(s));
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
      str.assign(begin(s), end(s));
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
  inline CStringNoCase operator+(CStringNoCase str1, const std::string &str2)
  {
    return str1.append(begin(str2), end(str2));
  }


  /**
   * @brief Addition operator that joins case-insensitive string with regular string. 
   *
   * @param str1 The regular string. 
   * @param str2 The case-insensitive string. 
   *
   * @return Resulting case-insensitive string. 
   */
  inline CStringNoCase operator+(const std::string &str1, const CStringNoCase &str2)
  {
    return CStringNoCase{begin(str1), end(str1)} + str2;
  }

}

#endif /* __TRAITSNOCASE_H__ */