/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef QC_UTIL_HH
#define QC_UTIL_HH

#include <string>
#include <sstream>
#include <mutex>
#include "qc-config.hh"

namespace qcpp
{

std::string global_report_yaml_header();
typedef std::lock_guard<std::mutex> std_mutex_lock;

} // end namespace qcpp

#endif /* QC_UTIL_HH */
