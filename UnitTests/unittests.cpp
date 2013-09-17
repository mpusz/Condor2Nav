#include "tools.h"
#include "istream.h"
#include "CppUnitTest.h"
#include <boost/filesystem.hpp>

using namespace condor2nav;

template<> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<TPathType>(const TPathType& val)
{
  return val == TPathType::LOCAL ? L"LOCAL" : L"ACTIVE_SYNC";
}

namespace unitTests
{
  using namespace Microsoft::VisualStudio::CppUnitTestFramework;

  TEST_CLASS(Utilities) {
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
}
