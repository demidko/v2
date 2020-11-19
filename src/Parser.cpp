#include "Parser.h"
#include <sstream>

std::istringstream &Parser::parse(const std::string &text) {
  static std::istringstream lineParser;
  lineParser.clear();
  lineParser.str(text);
  return lineParser;
}

std::string Parser::extract(const std::string &fromText, int wordAtIndex) {
  return *std::next(std::istream_iterator<std::string>(parse(fromText)), wordAtIndex);
}
