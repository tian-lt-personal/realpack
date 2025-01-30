#include <cassert>
#include <ios>
#include <istream>
#include <string>

namespace real::math::parse {

std::string get_string(std::istream& stream, std::streampos beg) {
  stream.seekg(-1, std::ios::cur);
  auto curr = stream.tellg();
  assert(curr >= beg);
  size_t len = curr - beg + 1;
  std::string result(len, '\0');
  stream.seekg(beg);
  stream.read(result.data(), static_cast<std::streamsize>(len));
  curr += 1;
  stream.seekg(curr);
  return result;
}

}  // namespace real::math::parse