/*
 * ============================================================================
 *
 *       Filename:  qc-util.hh
 *    Description:  Utilities for qcpp
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

/* Copyright (c) 2015 Kevin Murray
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
