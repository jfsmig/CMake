/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmStringAlgorithms_h
#define cmStringAlgorithms_h

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmRange.h"
#include "cm_string_view.hxx"
#include <cctype>
#include <initializer_list>
#include <sstream>
#include <string.h>
#include <string>
#include <utility>
#include <vector>

/** String range type.  */
typedef cmRange<std::vector<std::string>::const_iterator> cmStringRange;

/** Callable string comparison struct.  */
struct cmStrCmp
{
  cmStrCmp(std::string str)
    : Test_(std::move(str))
  {
  }

  bool operator()(cm::string_view sv) const { return Test_ == sv; }

private:
  std::string const Test_;
};

/** Returns true if the character @a ch is a whitespace character.  **/
inline bool cmIsSpace(char ch)
{
  return ((ch & 0x80) == 0) && std::isspace(ch);
}

/** Returns a string that has whitespace removed from the start and the end. */
std::string cmTrimWhitespace(cm::string_view str);

/** Returns a string that has quotes removed from the start and the end. */
std::string cmRemoveQuotes(cm::string_view str);

/** Escape quotes in a string.  */
std::string cmEscapeQuotes(cm::string_view str);

/** Joins elements of a range with separator into a single string.  */
template <typename Range>
std::string cmJoin(Range const& rng, cm::string_view separator)
{
  if (rng.empty()) {
    return std::string();
  }

  std::ostringstream os;
  auto it = rng.begin();
  auto const end = rng.end();
  os << *it;
  while (++it != end) {
    os << separator << *it;
  }
  return os.str();
}

/** Extract tokens that are separated by any of the characters in @a sep.  */
std::vector<std::string> cmTokenize(cm::string_view str, cm::string_view sep);

/** Concatenate string pieces into a single string.  */
std::string cmCatViews(std::initializer_list<cm::string_view> views);

/** Utility class for cmStrCat.  */
class cmAlphaNum
{
public:
  cmAlphaNum(cm::string_view view)
    : View_(view)
  {
  }
  cmAlphaNum(std::string const& str)
    : View_(str)
  {
  }
  cmAlphaNum(const char* str)
    : View_(str)
  {
  }
  cmAlphaNum(char ch)
    : View_(Digits_, 1)
  {
    Digits_[0] = ch;
  }
  cmAlphaNum(int val);
  cmAlphaNum(unsigned int val);
  cmAlphaNum(long int val);
  cmAlphaNum(unsigned long int val);
  cmAlphaNum(long long int val);
  cmAlphaNum(unsigned long long int val);
  cmAlphaNum(float val);
  cmAlphaNum(double val);

  cm::string_view View() const { return View_; }

private:
  cm::string_view View_;
  char Digits_[32];
};

/** Concatenate string pieces and numbers into a single string.  */
template <typename... AV>
inline std::string cmStrCat(cmAlphaNum const& a, cmAlphaNum const& b,
                            AV const&... args)
{
  return cmCatViews(
    { a.View(), b.View(), static_cast<cmAlphaNum const&>(args).View()... });
}

/** Joins wrapped elements of a range with separator into a single string.  */
template <typename Range>
std::string cmWrap(cm::string_view prefix, Range const& rng,
                   cm::string_view suffix, cm::string_view sep)
{
  if (rng.empty()) {
    return std::string();
  }
  return cmCatViews(
    { prefix, cmJoin(rng, cmCatViews({ suffix, sep, prefix })), suffix });
}

/** Joins wrapped elements of a range with separator into a single string.  */
template <typename Range>
std::string cmWrap(char prefix, Range const& rng, char suffix,
                   cm::string_view sep)
{
  return cmWrap(cm::string_view(&prefix, 1), rng, cm::string_view(&suffix, 1),
                sep);
}

/** Returns true if string @a str starts with the character @a prefix.  */
inline bool cmHasPrefix(cm::string_view str, char prefix)
{
  return !str.empty() && (str.front() == prefix);
}

/** Returns true if string @a str starts with string @a prefix.  */
inline bool cmHasPrefix(cm::string_view str, cm::string_view prefix)
{
  return str.compare(0, prefix.size(), prefix) == 0;
}

/** Returns true if string @a str starts with string @a prefix.  */
template <size_t N>
inline bool cmHasLiteralPrefix(cm::string_view str, const char (&prefix)[N])
{
  return cmHasPrefix(str, cm::string_view(prefix, N - 1));
}

/** Returns true if string @a str ends with the character @a suffix.  */
inline bool cmHasSuffix(cm::string_view str, char suffix)
{
  return !str.empty() && (str.back() == suffix);
}

/** Returns true if string @a str ends with string @a suffix.  */
inline bool cmHasSuffix(cm::string_view str, cm::string_view suffix)
{
  return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/** Returns true if string @a str ends with string @a suffix.  */
template <size_t N>
inline bool cmHasLiteralSuffix(cm::string_view str, const char (&suffix)[N])
{
  return cmHasSuffix(str, cm::string_view(suffix, N - 1));
}

/** Removes an existing suffix character of from the string @a str.  */
inline void cmStripSuffixIfExists(std::string& str, char suffix)
{
  if (cmHasSuffix(str, suffix)) {
    str.pop_back();
  }
}

/** Removes an existing suffix string of from the string @a str.  */
inline void cmStripSuffixIfExists(std::string& str, cm::string_view suffix)
{
  if (cmHasSuffix(str, suffix)) {
    str.resize(str.size() - suffix.size());
  }
}

/** Converts a string to long. Expects that the whole string is an integer.  */
bool cmStrToLong(const char* str, long* value);
bool cmStrToLong(std::string const& str, long* value);

/** Converts a string to unsigned long. Expects that the whole string is an
 * integer */
bool cmStrToULong(const char* str, unsigned long* value);
bool cmStrToULong(std::string const& str, unsigned long* value);

#endif