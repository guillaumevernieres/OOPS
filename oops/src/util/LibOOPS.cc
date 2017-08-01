/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   December 2016

#include <algorithm>
#include <string>

#include "util/LibOOPS.h"

//#include "eckit/eckit_version.h"

namespace oops {

//----------------------------------------------------------------------------------------------------------------------

static LibOOPS liboops;

LibOOPS::LibOOPS() : Library("oops") {}

const LibOOPS& LibOOPS::instance()
{
    return liboops;
}

const void* LibOOPS::addr() const { return this; }

std::string LibOOPS::version() const { return "0.0.0"; }

std::string LibOOPS::gitsha1(unsigned int count) const {
    std::string sha1("");
    if(sha1.empty()) {
        return "not available";
    }

    return sha1.substr(0,std::min(count,40u));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace oops

