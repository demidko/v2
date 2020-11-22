#include "VlqIstream.h"

VlqIstream::VlqIstream(std::istream &i) : istream(i) {}

VlqIstream &VlqIstream::operator>>(uint16_t &n) {

  return *this;
}



