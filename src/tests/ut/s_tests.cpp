// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>
#include <_to.hpp>
#include <s.hpp>

namespace {

using small = std::vector<uint8_t>;
using middle = std::vector<uint16_t>;

}  // namespace

TEST(s_tests, frac) {
  {
    auto fx = real::frac(real::create_z<small>(17), real::create_z<small>(4), 1);
    auto text = real::to_decimal_string(fx, 3);
    EXPECT_EQ(text, "4.250");
  }
  {
    auto fx = real::frac(real::create_z<small>(1), real::create_z<small>(3), 2);
    auto text = real::to_decimal_string(fx, 3);
    EXPECT_EQ(text, "0.333");
  }
  {
    auto fx = real::frac(real::create_z<small>(1), real::create_z<small>(7), 5);
    auto text = real::to_decimal_string(fx, 10);
    EXPECT_EQ(text, "0.1428571428");
  }
  {
    auto fx = real::frac(real::create_z<middle>(100), real::create_z<middle>(7), 5);
    auto text = real::to_decimal_string(fx, 20);
    EXPECT_EQ(text, "14.28571428571428571428");
  }
  {
    auto fx = real::frac(real::create_z<middle>(764188718), real::create_z<middle>(-52361232), 7);
    auto text = real::to_decimal_string(fx, 30);
    EXPECT_EQ(text, "-14.594551900535877383480969278950");
  }
  {
    auto fx = real::frac(
        real::create_z("-3412364519273649187236491872364918723649187236491519872346591873465918374659187324659827365982"
                       "73645982734659287364598712364591873645918734561983746519384756198347561938475619348765134"),
        real::create_z("987619238756193487561932487652938746529387456293874652938746592837465293874562934875"), 80);
    auto text = real::to_decimal_string(fx, 200);
    EXPECT_EQ(text,
              "-345514180502516037022912396890317871367538825327426779642732139372362938890138555664731901662969485."
              "79495408404252759386455019321935849460701619075172339972430568026480424408854524299089156865229037420860"
              "077832199571824148113078765119503698272782814646169976524512612811587120228540630001126479474327");
  }
}
