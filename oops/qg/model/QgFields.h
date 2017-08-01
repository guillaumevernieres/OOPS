/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGFIELDS_H_
#define QG_MODEL_QGFIELDS_H_

#include <ostream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "model/QgGeometry.h"
#include "model/VarQG.h"
#include "util/DateTime.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace qg {
  class LocQG;
  class GomQG;

// -----------------------------------------------------------------------------
/// Class to represent a FieldSet for the QG model
class QgFields : public util::Printable,
                 private util::ObjectCounter<QgFields> {
 public:
  static const std::string classname() {return "qg::QgFields";}

// Constructors and basic operators
  QgFields(const QgGeometry &, const VarQG &, const util::DateTime &);
  QgFields(const QgFields &, const QgGeometry &);
  QgFields(const QgFields &, const VarQG &);
  QgFields(const QgFields &, const bool);
  QgFields(const QgFields &);
  ~QgFields();

  void zero();
  void zero(const util::DateTime &);
  QgFields & operator=(const QgFields &);
  QgFields & operator+=(const QgFields &);
  QgFields & operator-=(const QgFields &);
  QgFields & operator*=(const double &);
  void axpy(const double &, const QgFields &);
  double dot_product_with(const QgFields &) const;
  void schur_product_with(const QgFields &);
  void random();

// Interpolate to given location
  void interpolate(const LocQG &, GomQG &) const;
  void interpolateTL(const LocQG &, GomQG &) const;
  void interpolateAD(const LocQG &, const GomQG &);

// Interpolate full fields
  void changeResolution(const QgFields &);
  void add(const QgFields &);
  void diff(const QgFields &, const QgFields &);

// Utilities
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const;
  boost::shared_ptr<const QgGeometry> geometry() const {return geom_;}

  const util::DateTime & time() const {return time_;}
  util::DateTime & time() {return time_;}

  int & toFortran() {return keyFlds_;}
  const int & toFortran() const {return keyFlds_;}

  bool isForModel(const bool) const;

 private:
  void print(std::ostream &) const;
  int keyFlds_;
  boost::shared_ptr<const QgGeometry> geom_;
  boost::shared_ptr<const VarQG> vars_;
  util::DateTime time_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_QGFIELDS_H_
