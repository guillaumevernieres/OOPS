/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXFIELDS_H_
#define XXXX_MODEL_XXXXFIELDS_H_

#include <ostream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "model/XxxxGeometry.h"
#include "model/XxxxVariables.h"
#include "util/DateTime.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace xxxx {

// -----------------------------------------------------------------------------
/// Class to represent a FieldSet for the XXXX model
class XxxxFields : public util::Printable,
                 private util::ObjectCounter<XxxxFields> {
 public:
  static const std::string classname() {return "xxxx::XxxxFields";}

// Constructors and basic operators
  XxxxFields(const XxxxGeometry &, const XxxxVariables &, const util::DateTime &);
  XxxxFields(const XxxxFields &, const XxxxGeometry &);
  XxxxFields(const XxxxFields &, const XxxxVariables &);
  XxxxFields(const XxxxFields &, const bool);
  XxxxFields(const XxxxFields &);
  ~XxxxFields();

  void zero();
  void zero(const util::DateTime &);
  XxxxFields & operator=(const XxxxFields &);
  XxxxFields & operator+=(const XxxxFields &);
  XxxxFields & operator-=(const XxxxFields &);
  XxxxFields & operator*=(const double &);
  void axpy(const double &, const XxxxFields &);
  double dot_product_with(const XxxxFields &) const;
  void schur_product_with(const XxxxFields &);
  void random();

// Interpolate to given location
//  void interpolate(const LocQG &, GomQG &) const;
//  void interpolateTL(const LocQG &, GomQG &) const;
//  void interpolateAD(const LocQG &, const GomQG &);

// Interpolate full fields
  void changeResolution(const XxxxFields &);
  void add(const XxxxFields &);
  void diff(const XxxxFields &, const XxxxFields &);

// Utilities
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const;
  boost::shared_ptr<const XxxxGeometry> geometry() const {return geom_;}

  const util::DateTime & time() const {return time_;}
  util::DateTime & time() {return time_;}

  int & toFortran() {return keyFlds_;}
  const int & toFortran() const {return keyFlds_;}

  bool isForModel(const bool) const;

 private:
  void print(std::ostream &) const;
  int keyFlds_;
  boost::shared_ptr<const XxxxGeometry> geom_;
  boost::shared_ptr<const XxxxVariables> vars_;
  util::DateTime time_;
};
// -----------------------------------------------------------------------------

}  // namespace xxxx
#endif  // XXXX_MODEL_XXXXFIELDS_H_
