#include <boost/test/unit_test.hpp>

void init_mm();
std::string scramble(const std::string_view& argument, int modifier);

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
} // namespace rote