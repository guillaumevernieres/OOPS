/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_MINIMIZER_H_
#define OOPS_ASSIMILATION_MINIMIZER_H_

#include <boost/noncopyable.hpp>
#include <map>
#include <string>

#include "util/Logger.h"
#include "oops/assimilation/ControlIncrement.h"
#include "oops/assimilation/CostFunction.h"
#include "eckit/config/Configuration.h"
#include "util/abor1_cpp.h"

namespace oops {

// -----------------------------------------------------------------------------

/// A Minimizer knows how to minimize a cost function
template<typename MODEL> class Minimizer : private boost::noncopyable {
 public:
  virtual ~Minimizer() {}
  virtual ControlIncrement<MODEL> * minimize(const eckit::Configuration &) =0;
  virtual const std::string classname() const =0;
};

// -----------------------------------------------------------------------------

/// Minimizer Factory
template <typename MODEL>
class MinFactory {
  typedef CostFunction<MODEL>        CostFct_;
 public:
  static Minimizer<MODEL> * create(const eckit::Configuration &, const CostFct_ &);
  virtual ~MinFactory() { getMakers().clear(); }
 protected:
  explicit MinFactory(const std::string &);
 private:
  virtual Minimizer<MODEL> * make(const eckit::Configuration &, const CostFct_ &) =0;
  static std::map < std::string, MinFactory<MODEL> * > & getMakers() {
    static std::map < std::string, MinFactory<MODEL> * > makers_;
    return makers_;
  }
};

// -----------------------------------------------------------------------------

template<class MODEL, class FCT>
class MinMaker : public MinFactory<MODEL> {
  typedef CostFunction<MODEL>        CostFct_;
  virtual Minimizer<MODEL> * make(const eckit::Configuration & conf, const CostFct_ & J) {
    return new FCT(conf, J);
  }
 public:
  explicit MinMaker(const std::string & name) : MinFactory<MODEL>(name) {}
};

// =============================================================================

template <typename MODEL>
MinFactory<MODEL>::MinFactory(const std::string & name) {
  if (getMakers().find(name) != getMakers().end()) {
    Log::error() << name << " already registered in minimizer factory." << std::endl;
    ABORT("Element already registered in MinFactory.");
  }
  getMakers()[name] = this;
}

// -----------------------------------------------------------------------------

template <typename MODEL>
Minimizer<MODEL>* MinFactory<MODEL>::create(const eckit::Configuration & config,
                                            const CostFct_ & J) {
  std::string id = config.getString("algorithm");
  Log::info() << "Minimizer algorithm=" << id << std::endl;
  typename std::map<std::string, MinFactory<MODEL>*>::iterator j = getMakers().find(id);
  if (j == getMakers().end()) {
    Log::error() << id << " does not exist in minimizer factory." << std::endl;
    ABORT("Element does not exist in MinFactory.");
  }
  return (*j).second->make(config, J);
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_ASSIMILATION_MINIMIZER_H_
