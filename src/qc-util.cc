/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
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
