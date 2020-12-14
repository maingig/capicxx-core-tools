/* Copyright (C) 2013-2019 BMW Group
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "StopWatch.hpp"

#include <cassert>
#include <ctime>

#ifdef _WIN32
    #include <chrono>
#endif

#define USEC_PER_SEC  1000000ULL
#define NSEC_PER_USEC 1000ULL


StopWatch::usec_t StopWatch::getTotalElapsedMicroseconds() const {
    usec_t elapsed = totalElapsed_;

    if (started_)
        elapsed += getElapsed();

    return elapsed;
}

StopWatch::usec_t StopWatch::getTotalElapsedSeconds() const {
    return getTotalElapsedMicroseconds() / USEC_PER_SEC;
}

StopWatch::usec_t StopWatch::now() {
#ifdef _WIN32
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#else
    struct timespec ts;

    const int ret = clock_gettime(CLOCK_MONOTONIC, &ts);
    assert(!ret);
    (void)ret;

    return (usec_t) ts.tv_sec * USEC_PER_SEC + (usec_t) ts.tv_nsec / NSEC_PER_USEC;
#endif
}

