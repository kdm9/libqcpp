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

#include <yaml-cpp/yaml.h>

#include "qc-util.hh"

namespace qcpp
{

std::string
global_report_yaml_header()
{
    std::ostringstream ss;
    YAML::Emitter yml;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key   << "QCPP"
        << YAML::Value
        << YAML::BeginMap
        << YAML::Key   << "version"
        << YAML::Value << QCPP_VERSION;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}


} // end namespace qcpp
