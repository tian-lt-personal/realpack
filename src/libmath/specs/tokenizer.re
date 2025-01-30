#include <ios>
#include "parse.hpp"

namespace real::math::parse {
  extern std::string get_string(std::istream& stream, std::streampos beg);

  std::expected<token, token_error> tokenizer::operator()() {
    std::streampos t1, t2, t3; // temporary states for s-tags
    try {
/*!stags:re2c format = 'std::streampos @@;\n'; */
/*!re2c
  re2c:api = custom;
  re2c:api:style = free-form;
  re2c:indent:string = "  ";
  re2c:define:YYCTYPE = char;
  re2c:define:YYPEEK = "static_cast<char>(in_.peek())";
  re2c:define:YYSKIP = "in_.ignore();";
  re2c:define:YYBACKUP = "rmark_ = in_.tellg();";
  re2c:define:YYRESTORE = "in_.seekg(rmark_);";
  re2c:define:YYSTAGP = "@@ = in_.tellg();";
  re2c:yyfill:enable = 0;
  re2c:tags = 1;

  id = [a-zA-Z_][a-zA-Z0-9_]*;

  * { return std::unexpected{token_error{.code = token_error_code::bad_token}}; }
  @t1 id {
     return token{.type = token_type::id, .str = get_string(in_, t1)};
  }
*/
    }
    catch(const std::ios_base::failure& ex) {
      return std::unexpected{token_error{.code = token_error_code::stream_error, .what = ex.what()}};
    }
  }
  
}  // namespace real::math::parse
