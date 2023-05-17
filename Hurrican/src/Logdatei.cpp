/**
 * This implements logging to a file and cout (or the Android log).
 *
 * (c) 2002 Jörg M. Winterstein
 * (c) 2017 Stefan Schmidt
 **/

#include "Logdatei.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#if defined(ANDROID)
#include <android/log.h>
#endif
namespace fs = std::filesystem;

/**
 * Construct the Logger object and open the logfile
 **/
Logdatei::Logdatei(const std::string &filename)
    : filename_(filename), file(std::ofstream(filename)), delLogFile(false) {
    // Reduce unnecessary syncs
    std::cout.sync_with_stdio(false);

    if (!file) {
        std::cerr << "Unable to open logfile (" << filename << ")!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Logdatei::~Logdatei() {
    // make sure no output gets lost
    flush();
    file.close();

    // Kein Fehler im Game? Dann Logfile löschen
    if (delLogFile)
        fs::remove(fs::path(filename_));
}

/**
 * Puts the internal buffer into the output-streams, flushes them and empties the buffer
 **/
void Logdatei::flush() {
#ifdef ANDROID
    // Android has no usable std::cout, so we use the Android-specific logging
    __android_log_print(ANDROID_LOG_INFO, "Hurrican", "%s", this->str().c_str());
#else
    // print to both outputs
    std::cout << this->str();
    this->file << this->str();

    // flush both streams to ensure they are up-to-date
    std::cout << std::flush;
    this->file << std::flush;
#endif

    // empty internal buffer
    this->str("");
}
