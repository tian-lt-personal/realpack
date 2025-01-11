// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>

namespace {

using small = std::vector<uint8_t>;
using middle = std::vector<uint16_t>;

}  // namespace

TEST(create_tests, z_from_unsigned) {
  {
    auto zero = real::create_z(0);
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    auto one = real::create_z(1);
    EXPECT_EQ(real::cmp_z(one, real::identity()), 0);
  }
}

TEST(create_tests, z_from_string) {
  {
    auto zero = real::create_z("0");
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    auto one = real::create_z<small>("1");
    EXPECT_EQ(real::cmp_z(one, real::identity<small>()), 0);
  }
  {
    auto expected = real::create_z<small>(11u);
    auto num = real::create_z<small>("11");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<small>(-123);
    auto num = real::create_z<small>("-123");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<small>(-1234);
    auto num = real::create_z<small>(" -1234");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<small>(-8237239);
    auto num = real::create_z<small>(" -8237239");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<middle>(1234567890ull);
    auto num = real::create_z<middle>(" +  1234567890");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<middle>(8812349812342376ull);
    expected.sign = true;
    auto num = real::create_z<middle>(" -  8812349812342376");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto num1 = real::create_z(
        "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    auto num2 = real::create_z("10000000000");
    EXPECT_EQ(real::cmp_z(num1, real::pow_z(num2, 10)), 0);
  }
  {
    auto num1 = real::create_z(
        "61823746812376498172634981726394871623987461923874619283746192873641928736419827364192873461928374619287364192"
        "83764192839128374019823740198273401982374019283740192837401982348172634987162349871236984761293874619287364918"
        "2736419253192346918723491827346918726349176239476192837467");
    real::z r;
    real::div_z(num1, real::create_z(2), &r);
    EXPECT_EQ(real::cmp_z(r, real::identity()), 0);
  }
  {
    auto num1 = real::create_z(
        "10293841234098102398741092837410928347019283740198237401982374011098237401928374019823740198273409812730498172"
        "03984710298374019283740192837401928374019283740192837401293874012983019283571092387510932487510928375102987410"
        "239847");
    auto num2 = real::create_z(
        "203168971234659123048917230498172340981723048912341092837401982734091827340981237401982374019823419823740123"
        "4");
    auto expected = real::create_z(
        "10293841234098102398741092837410928347019283740198237401982374011098237401928374019823740198273409812730498172"
        "03984712330063731630331423326574233355742693557423326524704802387002846624489365797323306507334668573337185647"
        "641081");
    EXPECT_EQ(real::cmp_z(expected, real::add_z(num1, num2)), 0);
  }
  {
    auto num1 = real::create_z(
        "76293874619283746192837410928374019283740192837401928374019283740198237401928374012938641098273641092837401928"
        "37401928374019283740192837410298374019283741092837410928374012936018726351092835710951234987123847012983740192"
        "85701987501983475019384751093845701983475019843751098347501983475109384571039485710983457103984571089345");
    auto num2 = real::create_z(
        " - "
        "12340182750987510439587109348751093451083467518346501984375109827350198975109485710349875104938571043985710394"
        "85710983475019834751098437512347102837401985609187465918347501894351098347501984375019843750189437501983475013"
        "98475019384750178643598176345018764350184");
    auto expected = real::create_z(
        "- "
        "94148035558288909978732308900286126667348156352872447053023516778577571389420765245669819660825876691716673337"
        "82222588379267096751269015920596089092040464898856023463364004557534384428706767307006508104912597427266675407"
        "01268810889486679677587646424481259798045886502186578197314561442091834582450410377158874951184407312400664474"
        "86906185903573364449646745811309789234113298923650931088841225095507705170618313058677022755860345851578814897"
        "62084728204964100117753460344253201241757027711264169723232200028506012092654801890132627265956173150746224855"
        "6919797333582169537678511931189480");
    EXPECT_EQ(real::cmp_z(expected, real::mul_z(num1, num2)), 0);
  }
}

TEST(create_tests, z_from_string_errors) {
  constexpr auto verify = [](std::string_view text) {
    bool has_error = false;
    try {
      std::ignore = real::create_z(text);
    } catch (const real::z_parse_error&) {
      has_error = true;
    }
    EXPECT_TRUE(has_error);
  };
  verify("");
  verify("-");
  verify("--");
  verify("+");
  verify("++++");
  verify(" - +");
  verify(" - x");
  verify("abc");
}
