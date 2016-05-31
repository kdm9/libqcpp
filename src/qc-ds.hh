/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#ifndef QC_DS_HH_ZZPPIRE4
#define QC_DS_HH_ZZPPIRE4

#include "qc-config.hh"

#include <string>

namespace qcpp
{

class Read
{
public:
    std::string         name;
    std::string         sequence;
    std::string         quality;

    Read                        ();
    Read                        (const std::string &name,
                                 const std::string &sequence,
                                 const std::string &quality);

    void
    clear                       ();

    size_t
    size                        () const;

    std::string
    str                         () const;

    void
    erase                       (size_t             pos=0);

    void
    erase                       (size_t             pos,
                                 size_t             count);
};
bool operator==(const Read &r1, const Read &r2);


class ReadPair: public std::pair<Read, Read>
{
public:
    ReadPair                    ();
    ReadPair                    (const std::string &name1,
                                 const std::string &sequence1,
                                 const std::string &quality1,
                                 const std::string &name2,
                                 const std::string &sequence2,
                                 const std::string &quality2);
    std::string
    str                         ();

    Read                first;
    Read                second;
};


} /* end namespace qcpp */
#endif /* end of include guard: QC_DS_HH_ZZPPIRE4 */
