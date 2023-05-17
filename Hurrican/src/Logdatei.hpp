/**
 * This implements logging to a file and cout (or the Android log).
 *
 * (c) 2002 Jörg M. Winterstein
 * (c) 2017 Stefan Schmidt
 **/

#ifndef _LOGDATEI_HPP_
#define _LOGDATEI_HPP_

#include <fstream>
#include <sstream>
#include <string>

class Logdatei : public std::ostringstream {
  private:
    const std::string filename_;  ///< Name of the logfile
    std::ofstream file;           ///< Filestream of the logfile
    // FIXME: This doesn't work currently. It's not a nice solution, anyway.
    bool delLogFile;  // Logfile am Ende löschen, wenn kein Fehler auftrat

  public:
    Logdatei(const std::string &filename);
    virtual ~Logdatei();

    void flush();

    // inherit all the output-operators for compatibility
    template <typename T>
    inline Logdatei &operator<<(const T &t) {
        (*reinterpret_cast<std::ostringstream *>(this)) << t;
        return *this;
    }

    // compatibility for manipulators
    typedef Logdatei &(*Logdatei_manip)(Logdatei &);
    inline Logdatei &operator<<(Logdatei_manip manip) { return manip(*this); }
};

// overload std::endl with a Logdatei-compatible version
namespace std {
inline Logdatei &endl(Logdatei &out) {
    out.put('\n');
    out.flush();
    return out;
}
}  // namespace std

extern Logdatei Protokoll;

#endif
