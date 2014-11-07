// Class for defining strings that can also handle numbers
// Useful for writing lines of output text that has a combination of
// words and numbers

#include "Activia/ActString.hh"

#include <iostream>
#include <ctype.h>
#include <sstream>

using std::cout;
using std::endl;

ActString::ActString() : _theString("")
{
  // Constructor
}

ActString::ActString(const char* s) : _theString(s)
{
  // Constructor
}

ActString::ActString(std::string& s) : _theString(s)
{
  // Constructor
}

ActString::~ActString() 
{
  // Destructor
}

void ActString::addInteger(int integer) {

  std::ostringstream o;
  o << integer;
  _theString += o.str();

}

void ActString::addDouble(double value) {

  std::ostringstream o;
  o << value;
  _theString += o.str();

}

ActString ActString::operator += (int integer) {
  this->addInteger(integer);
  return (*this);
}

ActString ActString::operator += (double value) {
  this->addDouble(value);
  return (*this);
}

ActString ActString::operator += (const char* text) {
  this->addText(text);
  return (*this);
}

ActString ActString::operator += (std::string &text) {
  this->addText(text);
  return (*this);
}

std::vector<std::string> ActString::split(ActString& splitter) {

  std::string splitString = splitter.getString();
  std::vector<std::string> result = this->split(splitString);
  return result;

}

std::vector<std::string> ActString::split(std::string& splitter) {

  // Code from STLplus
  std::vector<std::string> result;

  if (!_theString.empty() && !splitter.empty()) {
    
    for (std::string::size_type offset = 0;;) {

      std::string::size_type found = _theString.find(splitter, offset);

      if (found != std::string::npos) {
	std::string tmpString = _theString.substr(offset, found-offset);
	if (tmpString.size() > 0) {result.push_back(tmpString);}
        offset = found + splitter.size();
      } else {
	std::string tmpString = _theString.substr(offset, _theString.size()-offset);
	if (tmpString.size() > 0) {result.push_back(tmpString);}
        break;
      }
    }
  }

  return result;

}
