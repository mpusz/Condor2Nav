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
* @file unittests.cpp
*
* @brief Provides unit tests for Condor2Nav project.
*/

#include "tools.h"
#include "activeObject.h"
#include "condor.h"
#include "istream.h"
#include "fileParserCSV.h"
#include "fileParserINI.h"
#include "CppUnitTest.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace condor2nav;

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<TPathType>(const TPathType& val)
{
  return val == TPathType::LOCAL ? L"LOCAL" : L"ACTIVE_SYNC";
}

namespace unitTests
{
  using namespace Microsoft::VisualStudio::CppUnitTestFramework;

  const bfs::path MAIN_SRC_DIR = "..";


  ////////////////////////   U T I L I T I E S   ////////////////////////

  TEST_CLASS(TestUtilities) {
  public:
    TEST_METHOD(ConversionsToText)
    {
      Assert::AreEqual(std::string{"0"},      Convert(0));
      Assert::AreEqual(std::string{"3"},      Convert(3));
      Assert::AreEqual(std::string{"-123"},   Convert(-123));
      Assert::AreEqual(std::string{"99.123"}, Convert(99.123));
    }

    TEST_METHOD(ConversionsFromText)
    {
      Assert::AreEqual(0,      Convert<int>("0"));
      Assert::AreEqual(3,      Convert<int>("3"));
      Assert::AreEqual(-123,   Convert<int>("-123"));
      Assert::AreEqual(99.123, Convert<double>("99.123"));
    }

    TEST_METHOD(ConversionsCoordinatesToText)
    {
      Assert::AreEqual(std::string{ "00:00.000N"}, Coord2DDMMFF(TLatitude{0}));
      Assert::AreEqual(std::string{"000:00.000E"}, Coord2DDMMFF(TLongitude{0}));
      Assert::AreEqual(std::string{ "54:22.000N"}, Coord2DDMMFF(TLatitude{54.366667}));
      Assert::AreEqual(std::string{"018:38.000E"}, Coord2DDMMFF(TLongitude{18.6333336}));
      Assert::AreEqual(std::string{ "21:34.767N"}, Coord2DDMMFF(TLatitude{21.5794446}));
      Assert::AreEqual(std::string{"158:11.833W"}, Coord2DDMMFF(TLongitude{-158.1972226}));
      Assert::AreEqual(std::string{ "13:09.783S"}, Coord2DDMMFF(TLatitude{-13.163056}));
      Assert::AreEqual(std::string{"072:32.733W"}, Coord2DDMMFF(TLongitude{-72.545556}));

      Assert::AreEqual(std::string{ "00:00:00N"},  Coord2DDMMSS(TLatitude{0}));
      Assert::AreEqual(std::string{"000:00:00E"},  Coord2DDMMSS(TLongitude{0}));
      Assert::AreEqual(std::string{ "54:22:00N"},  Coord2DDMMSS(TLatitude{54.366667}));
      Assert::AreEqual(std::string{"018:38:00E"},  Coord2DDMMSS(TLongitude{18.6333336}));
      Assert::AreEqual(std::string{ "21:34:46N"},  Coord2DDMMSS(TLatitude{21.5794446}));
      Assert::AreEqual(std::string{"158:11:50W"},  Coord2DDMMSS(TLongitude{-158.1972226}));
      Assert::AreEqual(std::string{ "13:09:47S"},  Coord2DDMMSS(TLatitude{-13.163056}));
      Assert::AreEqual(std::string{"072:32:44W"},  Coord2DDMMSS(TLongitude{-72.545556}));
    }

    TEST_METHOD(ConversionsSpeed)
    {
      Assert::AreEqual(0,  KmH2MS(0));
      Assert::AreEqual(20, KmH2MS(72));
      Assert::AreEqual(28, KmH2MS(100));
    }

    TEST_METHOD(ConversionsDeg2Rad)
    {
      Assert::AreEqual(0.0,         Deg2Rad(0));
      Assert::AreEqual(0.785398163, Deg2Rad(45),  0.00001);
      Assert::AreEqual(1.74532925,  Deg2Rad(100), 0.00001);
    }

    TEST_METHOD(ConversionsRad2Deg)
    {
      Assert::AreEqual(0.0,   Rad2Deg(0));
      Assert::AreEqual(45.0,  Rad2Deg(0.785398163), 0.00001);
      Assert::AreEqual(100.0, Rad2Deg(1.74532925),  0.00001);
    }

    TEST_METHOD(InsideAreaTest)
    {
      Assert::IsTrue(InsideArea(TLongitude{-10}, TLongitude{10}, TLatitude{-10}, TLatitude{10},
                                TLongitude{-5}, TLongitude{5}, TLatitude{-5}, TLatitude{5}));
      Assert::IsTrue(InsideArea(TLongitude{-10}, TLongitude{10}, TLatitude{-10}, TLatitude{10},
                                TLongitude{-10}, TLongitude{10}, TLatitude{-10}, TLatitude{10}));
      Assert::IsFalse(InsideArea(TLongitude{-10}, TLongitude{10}, TLatitude{-10}, TLatitude{10},
                                 TLongitude{-20}, TLongitude{20}, TLatitude{-20}, TLatitude{20}));
    }

    TEST_METHOD(PathTypes)
    {
      Assert::AreEqual(TPathType::LOCAL, PathType(""));
      Assert::AreEqual(TPathType::LOCAL, PathType("."));
      Assert::AreEqual(TPathType::LOCAL, PathType(".."));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir"));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir/"));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir/subdir"));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir/subdir/"));
      Assert::AreEqual(TPathType::LOCAL, PathType("../dir/subdir/"));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir\\"));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir\\subdir"));
      Assert::AreEqual(TPathType::LOCAL, PathType("dir\\subdir\\"));
      Assert::AreEqual(TPathType::LOCAL, PathType("..\\dir\\subdir\\"));
      Assert::AreEqual(TPathType::LOCAL, PathType("c:\\"));
      Assert::AreEqual(TPathType::LOCAL, PathType("c:\\dir"));
      Assert::AreEqual(TPathType::LOCAL, PathType("d:"));
      Assert::AreEqual(TPathType::ACTIVE_SYNC, PathType("\\dir"));
    }

    TEST_METHOD(FileExistsTest)
    {
      Assert::IsTrue(FileExists("UnitTests.dll"));
      Assert::IsTrue(FileExists("unitTests.DLL"));
      Assert::IsTrue(FileExists("./UnitTests.dll"));
      Assert::IsTrue(FileExists(".\\UnitTests.dll"));
      Assert::IsFalse(FileExists("nonexisting"));
    }

  };



  ////////////////////////   A C T I V E   O B J E C T   ////////////////////////

  TEST_CLASS(TestActiveObject) {
  public:
    TEST_METHOD(SimpleCommand)
    {
      int data = 123;
      {
        CActiveObject obj;
        obj.Send([&]{ data = 234; });
      }
      Assert::AreEqual(234, data);
    }
  };



  ////////////////////////   I S T R E A M   ////////////////////////

  TEST_CLASS(TestIStream) {
  public:
    TEST_METHOD(InvalidInput)
    {
      Assert::ExpectException<EOperationFailed>([]{ CIStream stream("nonexisting.some_file"); });
    }

    TEST_METHOD(ValidInput)
    {
      CIStream actual(MAIN_SRC_DIR / "README.txt");
      std::stringstream actualStr;
      actualStr << actual;
      bfs::ifstream expected(MAIN_SRC_DIR / "README.txt");
      std::stringstream expectedStr;
      expectedStr << expected.rdbuf();
      Assert::AreEqual(expectedStr.str(), actualStr.str());
    }

    TEST_METHOD(GetLine)
    {
      CIStream stream(MAIN_SRC_DIR / "README.txt");
      Assert::IsTrue(static_cast<bool>(stream));
      std::string txt;
      stream.GetLine(txt);
      Assert::AreEqual(std::string( "================"), txt);
      Assert::IsTrue(static_cast<bool>(stream));
      while(stream)
        stream.GetLine(txt);
      Assert::AreEqual(std::string(""), txt);
      Assert::IsFalse(static_cast<bool>(stream));
    }
  };



  ////////////////////////   F I L E   P A R S E R    I N I   ////////////////////////

  TEST_CLASS(TestFileParserINI) {
  public:
    TEST_METHOD(InvalidINIFile)
    {
      Assert::ExpectException<EOperationFailed>([]{ CFileParserINI parser("nonexisting.some_file"); });
    }

    TEST_METHOD(NotAnINIFile)
    {
      Assert::ExpectException<EOperationFailed>([]{ CFileParserINI parser(MAIN_SRC_DIR / "data/GliderData.csv"); });
    }

    TEST_METHOD(ValidINIFile)
    {
      CFileParserINI parser(MAIN_SRC_DIR / "data/condor2nav.ini");
      Assert::AreEqual((MAIN_SRC_DIR / "data/condor2nav.ini").string(), parser.Path().string());
      Assert::AreEqual(std::string("LK8000"), parser.Value("Condor2Nav", "Target"));
      Assert::AreEqual(std::string("1"), parser.Value("LK8000", "DefaultTaskOverwrite"));
      Assert::AreEqual(std::string("1"), parser.Value("LK8000", "CheckForMapUpdates"));
    }

    TEST_METHOD(InvalidINIEntry)
    {
      CFileParserINI parser(MAIN_SRC_DIR / "data/condor2nav.ini");
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("Condor2Nav", "DefaultTaskOverwrite"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("", "DefaultTaskOverwrite"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("Condor2Nav", "NonExisting"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("NonExisting", "DefaultTaskOverwrite"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("NonExisting", ""); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("", ""); });
    }

    TEST_METHOD(ValidINIValueOverwrite)
    {
      CFileParserINI parser(MAIN_SRC_DIR / "data/condor2nav.ini");
      parser.Value("Condor2Nav", "Target", "UnitTest");
      Assert::AreEqual(std::string("UnitTest"), parser.Value("Condor2Nav", "Target"));
      parser.Value("LK8000", "DefaultTaskOverwrite", "99");
      Assert::AreEqual(std::string("99"), parser.Value("LK8000", "DefaultTaskOverwrite"));
      parser.Value("Condor2Nav", "NonExisting", "Test");
      Assert::AreEqual(std::string("Test"), parser.Value("Condor2Nav", "NonExisting"));
      parser.Value("", "NonExisting", "Test");
      Assert::AreEqual(std::string("Test"), parser.Value("", "NonExisting"));
    }

    TEST_METHOD(InvalidINIValueOverwrite)
    {
      CFileParserINI parser(MAIN_SRC_DIR / "data/condor2nav.ini");
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("NonExisting", "DefaultTaskOverwrite", "Fail"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("NonExisting", "", "Fail"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("", "", "Fail"); });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Value("", {}, "Fail"); });
    }
  };



  ////////////////////////   F I L E   P A R S E R    C S V   ////////////////////////

  TEST_CLASS(TestFileParserCSV) {
  public:
    TEST_METHOD(InvalidCSVFile)
    {
      Assert::ExpectException<EOperationFailed>([]{ CFileParserCSV parser("nonexisting.some_file"); });
    }

    TEST_METHOD(NotACSVFile)
    {
      Assert::ExpectException<EOperationFailed>([]{ CFileParserCSV parser(MAIN_SRC_DIR / "data/condor2nav.ini"); });
    }

    TEST_METHOD(ValidCSVFile)
    {
      CFileParserCSV parser(MAIN_SRC_DIR / "data/GliderData.csv");
      Assert::AreEqual((MAIN_SRC_DIR / "data/GliderData.csv").string(), parser.Path().string());
      Assert::AreEqual(24U, parser.Rows().size());
      Assert::AreEqual(14U, parser.Rows()[0].size());
      Assert::AreEqual(std::string("285"), parser.Row("ASW28")[1]);
      Assert::AreEqual(std::string("285"), parser.Row("asw28", 0, true)[1]);
      Assert::AreEqual(std::string("ASW22"), parser.Row("280", 1)[0]);
    }

    TEST_METHOD(InvalidCSVFileEntry)
    {
      CFileParserCSV parser(MAIN_SRC_DIR / "data/GliderData.csv");
      Assert::ExpectException<EOperationFailed>([&]{ parser.Row("asw28")[1]; });
      Assert::ExpectException<EOperationFailed>([&]{ parser.Row("123", 1)[0]; });
    }
  };



  ////////////////////////   C O N D O R   ////////////////////////

  TEST_CLASS(TestCondor) {
  public:
    TEST_METHOD(InstallPath)
    {
      Assert::AreEqual("C:\\Program Files (x86)\\Condor", condor::InstallPath().string().c_str());
    }

    //TEST_METHOD(FPLPath)
    //{
    //  Assert::AreEqual("C:\\Program Files (x86)\\Condor", CCondor::InstallPath().string().c_str());
    //}

    //TEST_METHOD(TaskParser)
    //{
    //  Assert::AreEqual("C:\\Program Files (x86)\\Condor", CCondor::InstallPath().string().c_str());
    //}

    //TEST_METHOD(CoordConverter)
    //{
    //  Assert::AreEqual("C:\\Program Files (x86)\\Condor", CCondor::InstallPath().string().c_str());
    //}

  };

}
