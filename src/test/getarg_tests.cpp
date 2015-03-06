#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BELLY");
    BOOST_CHECK(GetBoolArg("-BELLY"));
    BOOST_CHECK(GetBoolArg("-BELLY", false));
    BOOST_CHECK(GetBoolArg("-BELLY", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BELLYo"));
    BOOST_CHECK(!GetBoolArg("-BELLYo", false));
    BOOST_CHECK(GetBoolArg("-BELLYo", true));

    ResetArgs("-BELLY=0");
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));

    ResetArgs("-BELLY=1");
    BOOST_CHECK(GetBoolArg("-BELLY"));
    BOOST_CHECK(GetBoolArg("-BELLY", false));
    BOOST_CHECK(GetBoolArg("-BELLY", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBELLY");
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));

    ResetArgs("-noBELLY=1");
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));

    ResetArgs("-BELLY -noBELLY");  // -BELLY should win
    BOOST_CHECK(GetBoolArg("-BELLY"));
    BOOST_CHECK(GetBoolArg("-BELLY", false));
    BOOST_CHECK(GetBoolArg("-BELLY", true));

    ResetArgs("-BELLY=1 -noBELLY=1");  // -BELLY should win
    BOOST_CHECK(GetBoolArg("-BELLY"));
    BOOST_CHECK(GetBoolArg("-BELLY", false));
    BOOST_CHECK(GetBoolArg("-BELLY", true));

    ResetArgs("-BELLY=0 -noBELLY=0");  // -BELLY should win
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BELLY=1");
    BOOST_CHECK(GetBoolArg("-BELLY"));
    BOOST_CHECK(GetBoolArg("-BELLY", false));
    BOOST_CHECK(GetBoolArg("-BELLY", true));

    ResetArgs("--noBELLY=1");
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", "eleven"), "eleven");

    ResetArgs("-BELLY -bar");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", "eleven"), "");

    ResetArgs("-BELLY=");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", "eleven"), "");

    ResetArgs("-BELLY=11");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", "eleven"), "11");

    ResetArgs("-BELLY=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BELLY", 0), 0);

    ResetArgs("-BELLY -bar");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BELLY=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BELLY=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BELLY");
    BOOST_CHECK_EQUAL(GetBoolArg("-BELLY"), true);

    ResetArgs("--BELLY=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BELLY", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBELLY");
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));

    ResetArgs("-noBELLY=1");
    BOOST_CHECK(!GetBoolArg("-BELLY"));
    BOOST_CHECK(!GetBoolArg("-BELLY", true));
    BOOST_CHECK(!GetBoolArg("-BELLY", false));

    ResetArgs("-noBELLY=0");
    BOOST_CHECK(GetBoolArg("-BELLY"));
    BOOST_CHECK(GetBoolArg("-BELLY", true));
    BOOST_CHECK(GetBoolArg("-BELLY", false));

    ResetArgs("-BELLY --noBELLY");
    BOOST_CHECK(GetBoolArg("-BELLY"));

    ResetArgs("-noBELLY -BELLY"); // BELLY always wins:
    BOOST_CHECK(GetBoolArg("-BELLY"));
}

BOOST_AUTO_TEST_SUITE_END()
