#ifdef ACT_USE_QT

#include "Activia/ActQtDebugStream.hh"

ActQtDebugStream::ActQtDebugStream(std::ostream &stream, QTextEdit* text_edit) : m_stream(stream)
{
  log_window = text_edit;
  m_old_buf = stream.rdbuf();
  stream.rdbuf(this);
}

ActQtDebugStream::~ActQtDebugStream()
{
  // output anything that is left
  if (!m_string.empty()) {
    log_window->append(m_string.c_str());
  }

  m_stream.rdbuf(m_old_buf);

}

std::streambuf::int_type ActQtDebugStream::overflow(std::streambuf::int_type v) {

  if (v == '\n') {
    log_window->append(m_string.c_str());
    m_string.erase(m_string.begin(), m_string.end());
  } else {
    m_string += v;
  }

  return v;
}

std::streamsize ActQtDebugStream::xsputn(const char *p, std::streamsize n) {

  m_string.append(p, p + n);

  std::string::size_type pos = 0;
  while (pos != std::string::npos) {

    pos = m_string.find('\n');

    if (pos != std::string::npos) {
      std::string tmp(m_string.begin(), m_string.begin() + pos);
      log_window->append(tmp.c_str());
      m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
    }
  }
  
  return n;

}

#endif
