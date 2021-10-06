#include <boost/test/unit_test.hpp>

void init_mm();
std::string scramble(const std::string_view& argument, int modifier);
std::string drunkify(const std::string_view& arg, short drunk);

namespace rote
{

BOOST_AUTO_TEST_CASE(scramble_test)
{
    ::init_mm();

    BOOST_TEST(::scramble("beans", 0) == "yhxqk");
    BOOST_TEST(::scramble("beans beans", 1) == "fucjj vqqaa");
    BOOST_TEST(::scramble("beans cabbage beans", 2) == "eebnc zcifflb rynqw");
    BOOST_TEST(::scramble("a", 3) == "u");
    BOOST_TEST(::scramble("beans       beans", 4) == "wayje       hjaag");
    BOOST_TEST(::scramble("beans\tbeans", 5) == "roeig\tunqwb");
    BOOST_TEST(::scramble("", 6) == "");
}

BOOST_AUTO_TEST_CASE(drunkify_test)
{
    ::init_mm();

    BOOST_TEST(::drunkify("beans", 50) == "beAnnsh");
    BOOST_TEST(::drunkify("beans beans", 50) == "bBeaansh bEansH");
    BOOST_TEST(::drunkify("beans cabbage beans", 50) == "BeAnsH ccAbbaage BeeanSh");
    BOOST_TEST(::drunkify("a", 50) == "a");
    BOOST_TEST(::drunkify("beans       beans", 50) == "BbEanNsh       BE-E-EEanSh");
    BOOST_TEST(::drunkify("beans\tbeans", 50) == "BEANsh\tbbeaNSh");
    BOOST_TEST(::drunkify("beans    ", 50) == "Beansh     ");
    BOOST_TEST(::drunkify("", 50) == "");
}
} // namespace rote