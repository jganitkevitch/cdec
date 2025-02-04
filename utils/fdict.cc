#include "fdict.h"
#include "stdlib.h"
//for malloc (need on cygwin); todo <cstdlib> and std::malloc
#include <string>
#include <sstream>

using namespace std;

Dict FD::dict_;
bool FD::frozen_ = false;

std::string FD::Convert(std::vector<WordID> const& v) {
    return Convert(&*v.begin(),&*v.end());
}

std::string FD::Convert(WordID const *b,WordID const* e) {
  ostringstream o;
  for (WordID const* i=b;i<e;++i) {
    if (i>b) o << ' ';
    o << FD::Convert(*i);
  }
  return o.str();
}

static int HexPairValue(const char * code) {
  int value = 0;
  const char * pch = code;
  for (;;) {
    int digit = *pch++;
    if (digit >= '0' && digit <= '9') {
      value += digit - '0';
    }
    else if (digit >= 'A' && digit <= 'F') {
      value += digit - 'A' + 10;
    }
    else if (digit >= 'a' && digit <= 'f') {
      value += digit - 'a' + 10;
    }
    else {
      return -1;
    }
    if (pch == code + 2)
      return value;
    value <<= 4;
  }
}

int UrlDecode(const char *source, char *dest)
{
  char * start = dest;

  while (*source) {
    switch (*source) {
    case '+':
      *(dest++) = ' ';
      break;
    case '%':
      if (source[1] && source[2]) {
        int value = HexPairValue(source + 1);
        if (value >= 0) {
          *(dest++) = value;
          source += 2;
        }
        else {
          *dest++ = '?';
        }
      }
      else {
        *dest++ = '?';
      }
      break;
    default:
      *dest++ = *source;
    }
    source++;
  }

  *dest = 0;
  return dest - start;
}

int UrlEncode(const char *source, char *dest, unsigned max) {
  static const char *digits = "0123456789ABCDEF";
  unsigned char ch;
  unsigned len = 0;
  char *start = dest;

  while (len < max - 4 && *source)
  {
    ch = (unsigned char)*source;
    if (*source == ' ') {
      *dest++ = '+';
    }
    else if (strchr("=:;,_| %", ch)) {
      *dest++ = '%';
      *dest++ = digits[(ch >> 4) & 0x0F];
      *dest++ = digits[       ch & 0x0F];
    }
    else {
      *dest++ = *source;
    }
    source++;
  }
  *dest = 0;
  return start - dest;
}

std::string UrlDecodeString(const std::string & encoded) {
  const char * sz_encoded = encoded.c_str();
  size_t needed_length = encoded.length();
  for (const char * pch = sz_encoded; *pch; pch++) {
    if (*pch == '%')
      needed_length += 2;
  }
  needed_length += 10;
  char stackalloc[64];
  char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ?
    (char *)malloc(needed_length) : stackalloc;
  UrlDecode(encoded.c_str(), buf);
  std::string result(buf);
  if (buf != stackalloc) {
    free(buf);
  }
  return result;
}

std::string UrlEncodeString(const std::string & decoded) {
  size_t needed_length = decoded.length() * 3 + 3;
  char stackalloc[64];
  char * buf = needed_length > sizeof(stackalloc)/sizeof(*stackalloc) ?
    (char *)malloc(needed_length) : stackalloc;
  UrlEncode(decoded.c_str(), buf, needed_length);
  std::string result(buf);
  if (buf != stackalloc) {
    free(buf);
  }
  return result;
}

string FD::Escape(const string& s) {
  return UrlEncodeString(s);
}

