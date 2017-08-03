/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef _LORENZ95_TESTCONFIG_H_
#define _LORENZ95_TESTCONFIG_H_

#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include "eckit/runtime/Main.h"
#include "eckit/config/JSONConfiguration.h"
#include "eckit/config/LocalConfiguration.h"
#include "eckit/exception/Exceptions.h"

#include "util/Logger.h"

using oops::Log;

namespace test {

// -----------------------------------------------------------------------------

class TestConfig {

public:

  static const eckit::Configuration & config() {
    static TestConfig test;
    return *test.config_;
  }

  TestConfig() {

    eckit::Main::initialise(boost::unit_test::framework::master_test_suite().argc,
                            boost::unit_test::framework::master_test_suite().argv);

    int narg = boost::unit_test::framework::master_test_suite().argc;
    ASSERT(narg >= 2);
    std::string fname = boost::unit_test::framework::master_test_suite().argv[narg - 1];
    config_.reset(new eckit::JSONConfiguration(fname));
  }

  ~TestConfig() {}

private:
  boost::scoped_ptr<const eckit::JSONConfiguration> config_;
};

// -----------------------------------------------------------------------------

}  // namespace test

#endif  // _LORENZ95_TESTCONFIG_H_
