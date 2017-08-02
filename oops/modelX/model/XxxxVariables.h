/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXVARIABLES_H_
#define XXXX_MODEL_XXXXVARIABLES_H_

#include <ostream>
#include <string>

#include "util/Logger.h"
#include "model/XxxxFortran.h"
#include "eckit/config/Configuration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

namespace xxxx {

// -----------------------------------------------------------------------------
/// XxxxVariables class to handle variables for XXXX model.

class XxxxVariables : public util::Printable,
              private util::ObjectCounter<XxxxVariables> {
 public:
  static const std::string classname() {return "xxxx::XxxxVariables";}

  explicit XxxxVariables(const eckit::Configuration & config) {
    using oops::Log;
    Log::debug() << "XxxxVariables config:" << config << std::endl;
    const eckit::Configuration * conf = &config;
    xxxx_var_create_f90(keyVar_, &conf);
  }
  explicit XxxxVariables(const int keyVar): keyVar_(keyVar) {}

  ~XxxxVariables() {xxxx_var_delete_f90(keyVar_);}

  XxxxVariables(const XxxxVariables & other) {xxxx_var_clone_f90(other.keyVar_, keyVar_);}

  int& toFortran() {return keyVar_;}
  const int& toFortran() const {return keyVar_;}

 private:
  void print(std::ostream &) const;
  int keyVar_;
};

// -----------------------------------------------------------------------------

}  // namespace xxxx

#endif  // XXXX_MODEL_XXXXVARIABLES_H_
