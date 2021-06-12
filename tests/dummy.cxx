#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <sstream>
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

using namespace boost::serialization;

class SuperFun
{
    int m_x;
    int m_y;
    double m_wheee;

    public:
    SuperFun() : m_x(1), m_y(2), m_wheee(3.0)
    {
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & make_nvp("x", m_x);
        ar & make_nvp("y", m_y);
        ar & make_nvp("wheee", m_wheee);
    }
};

BOOST_AUTO_TEST_CASE(super_fun_test)
{
    SuperFun s;

    std::stringstream ss;

    boost::archive::xml_oarchive out(ss);
    out << make_nvp("SuperFun", s);

    std::cout << ss.str() << std::endl;

    BOOST_TEST(true /* test assertion */);
}