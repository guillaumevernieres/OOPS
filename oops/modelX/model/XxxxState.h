/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXSTATE_H_
#define XXXX_MODEL_XXXXSTATE_H_

#include <ostream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include "model/XxxxFields.h"
#include "util/DateTime.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

namespace eckit {
  class Configuration;
}

namespace xxxx {
  class XxxxGeometry;
  class XxxxIncrement;
  class XxxxVariables;

/// XXXX model state
/*!
 * A State contains everything that is needed to propagate the state
 * forward in time.
 */

// -----------------------------------------------------------------------------
class XxxxState : public util::Printable,
                private util::ObjectCounter<XxxxState> {
 public:
  static const std::string classname() {return "xxxx::XxxxState";}

/// Constructor, destructor
  XxxxState(const XxxxGeometry &, const XxxxVariables &, const util::DateTime &);  // Is it used?
  XxxxState(const XxxxGeometry &, const eckit::Configuration &);
  XxxxState(const XxxxGeometry &, const XxxxState &);
  XxxxState(const XxxxState &);
  virtual ~XxxxState();
  XxxxState & operator=(const XxxxState &);

/// Interpolate to observation location
//  void interpolate(const LocQG &, GomQG &) const;

/// Interpolate full fields
  void changeResolution(const XxxxState & xx);

/// Interactions with Increment
  XxxxState & operator+=(const XxxxIncrement &);

/// I/O and diagnostics
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const {return fields_->norm();}
  const util::DateTime & validTime() const {return fields_->time();}
  util::DateTime & validTime() {return fields_->time();}

/// Access to fields
  XxxxFields & fields() {return *fields_;}
  const XxxxFields & fields() const {return *fields_;}

  boost::shared_ptr<const XxxxGeometry> geometry() const {
    return fields_->geometry();
  }

/// Other
  void activateModel();
  void deactivateModel();

  void zero();
  void accumul(const double &, const XxxxState &);

 private:
  void print(std::ostream &) const;
  boost::scoped_ptr<XxxxFields> fields_;
  boost::scoped_ptr<XxxxFields> stash_;
};
// -----------------------------------------------------------------------------

}  // namespace xxxx

#endif  // XXXX_MODEL_XXXXSTATE_H_
