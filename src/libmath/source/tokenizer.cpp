/* Generated by re2c 3.0 on Fri Jan 31 01:26:57 2025 */
#include <ios>

#include "parse.hpp"

namespace real::math::parse {
  extern std::string get_string(std::istream& stream, std::streampos beg);

  std::expected<token, token_error> tokenizer::operator()() {
    std::streampos t1, t2, t3; // temporary states for s-tags
    for(;;) {
    try {
std::streampos yyt1;


{
  char yych;
  yych = static_cast<char>(in_.peek());
  switch (yych) {
    case 0x08:
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
    case ' ': goto yy2;
    case '%': goto yy4;
    case '(': goto yy5;
    case ')': goto yy6;
    case '*': goto yy7;
    case '+': goto yy8;
    case ',': goto yy9;
    case '-': goto yy10;
    case '.':
      yyt1 = in_.tellg();
      goto yy11;
    case '/': goto yy13;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      yyt1 = in_.tellg();
      goto yy14;
    case '=': goto yy16;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
      yyt1 = in_.tellg();
      goto yy17;
    case '^': goto yy19;
    default: goto yy1;
  }
yy1:
  in_.ignore();
  { return std::unexpected{token_error{.code = token_error_code::bad_token, .what = std::nullopt}}; }
yy2:
  in_.ignore();
  yych = static_cast<char>(in_.peek());
  switch (yych) {
    case 0x08:
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
    case ' ': goto yy2;
    default: goto yy3;
  }
yy3:
  { continue; }
yy4:
  in_.ignore();
  { return token{.type = token_type::perc, .str = std::nullopt}; }
yy5:
  in_.ignore();
  { return token{.type = token_type::lparen, .str = std::nullopt}; }
yy6:
  in_.ignore();
  { return token{.type = token_type::rparen, .str = std::nullopt}; }
yy7:
  in_.ignore();
  { return token{.type = token_type::mul, .str = std::nullopt}; }
yy8:
  in_.ignore();
  { return token{.type = token_type::plus, .str = std::nullopt}; }
yy9:
  in_.ignore();
  { return token{.type = token_type::comma, .str = std::nullopt}; }
yy10:
  in_.ignore();
  { return token{.type = token_type::minus, .str = std::nullopt}; }
yy11:
  in_.ignore();
  yych = static_cast<char>(in_.peek());
  switch (yych) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': goto yy20;
    default: goto yy12;
  }
yy12:
  { return token{.type = token_type::dot, .str = std::nullopt}; }
yy13:
  in_.ignore();
  { return token{.type = token_type::div, .str = std::nullopt}; }
yy14:
  in_.ignore();
  yych = static_cast<char>(in_.peek());
  switch (yych) {
    case '.': goto yy20;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': goto yy14;
    default: goto yy15;
  }
yy15:
  t1 = yyt1;
  { return token{.type = token_type::value, .str = get_string(in_, t1)}; }
yy16:
  in_.ignore();
  { return token{.type = token_type::eql, .str = std::nullopt}; }
yy17:
  in_.ignore();
  yych = static_cast<char>(in_.peek());
  switch (yych) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z': goto yy17;
    default: goto yy18;
  }
yy18:
  t1 = yyt1;
  { return token{.type = token_type::id, .str = get_string(in_, t1)}; }
yy19:
  in_.ignore();
  { return token{.type = token_type::exp, .str = std::nullopt}; }
yy20:
  in_.ignore();
  yych = static_cast<char>(in_.peek());
  switch (yych) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': goto yy20;
    default: goto yy15;
  }
}

    } // end of try
    catch(const std::ios_base::failure& ex) {
      if (in_.eof()) {
        return std::unexpected{token_error{.code = token_error_code::eof, .what = std::nullopt}};
      } else {
        return std::unexpected{token_error{.code = token_error_code::stream_error, .what = ex.what()}};
      }
    }
  } // end of for
  } // end of operator()
  
}  // namespace real::math::parse
