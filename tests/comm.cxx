#include <boost/test/unit_test.hpp>
#include "mud.hxx"

namespace rote
{

BOOST_AUTO_TEST_CASE(obj_short_test)
{
    OBJ_DATA obj = {};
    char desc[] = "super beans";
    obj.short_descr = desc;
    obj.count = 1;

    BOOST_TEST(obj_short(&obj) == "super beans");
    
    obj.count = 2;
    BOOST_TEST(obj_short(&obj) == "super beans (2)");

    obj.count = 0;
    BOOST_TEST(obj_short(&obj) == "super beans");
}
} // namespace rote