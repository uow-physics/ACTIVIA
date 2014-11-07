#ifdef ACT_USE_QT

#ifndef ACT_QT_DEBUG_STREAM_HH
#define ACT_QT_DEBUG_STREAM_HH

#include <ostream>
#include <fstream>
#include <streambuf>
#include <string>

#include "QtGui/qtextedit.h"

/// \brief Redirect output such as cout to a QTextEdit object in Qt

class ActQtDebugStream : public std::basic_streambuf<char> {

public:

  /// Constructor, requiring the output stream and QTextEdit pointer
  ActQtDebugStream(std::ostream &stream, QTextEdit* text_edit);

  /// Destructor
  virtual ~ActQtDebugStream();

protected:

  virtual std::streambuf::int_type overflow(std::streambuf::int_type v);

  /// Write out n characters from the array p to the output sequence
  /// controlled by the stream buffer
  virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:

 std::ostream &m_stream;
 std::streambuf *m_old_buf;
 std::string m_string;

 QTextEdit* log_window;

};

#endif

#endif
