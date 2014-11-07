#ifndef ACT_STRING_HH
#define ACT_STRING_HH

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

/// \brief A class for defining a string that can also handle numbers.
///
/// Useful for writing lines of text that has a combination of
/// words and numbers.

class ActString {

 public:

  /// Define an empty string
  ActString();
  /// Define a string using a set of characters
  ActString(const char* s);
  /// Define a string using a STL string.
  ActString(std::string& s);

  /// Add an integer value to the string
  void addInteger(int integer);
  /// Add a double value to the string
  void addDouble(double value);
  /// Add some text
  void addText(const char* text) {_theString += text;}
  /// Add some text
  void addText(std::string& text) {_theString += text;}

  /// Add an integer to the string
  ActString operator += (int integer);
  /// Add a double to the string
  ActString operator += (double value);
  /// Add some text
  ActString operator += (const char* text);
  /// Add some text
  ActString operator += (std::string& text);

  virtual ~ActString();

  /// Return the full string as an STL string
  std::string getString() {return _theString;}
  /// Return the full string as a set of characters
  const char* getData() {return _theString.c_str();}

  /// Return the total size of the string.
  int size() {return _theString.size();}

  /// Split up a string according to a split string (e.g. ":" or " ")
  std::vector<std::string> split(std::string& splitter);
  /// Split up a string according to a split string (e.g. ":" or " ")
  std::vector<std::string> split(ActString& splitter);

 protected:
  
 private:

  std::string _theString;

};

#endif
