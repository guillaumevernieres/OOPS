/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_INTERFACE_LINEARMODEL_H_
#define OOPS_INTERFACE_LINEARMODEL_H_

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "util/Logger.h"
#include "oops/interface/Geometry.h"
#include "oops/interface/Increment.h"
#include "oops/interface/ModelAuxControl.h"
#include "oops/interface/ModelAuxIncrement.h"
#include "oops/interface/State.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"
#include "util/Timer.h"

namespace eckit {
  class Configuration;
}

namespace oops {

/// Encapsulates the linear forecast model.
/*!
 * Encapsulates the linear forecast model and its adjoint. The linearization
 * trajectory is embedded inside the LinearModel class.
 */

// -----------------------------------------------------------------------------

template <typename MODEL>
class LinearModel : public util::Printable,
                    private boost::noncopyable,
                    private util::ObjectCounter<LinearModel<MODEL> > {
  typedef typename MODEL::LinearModel           LinearModel_;
  typedef Increment<MODEL>           Increment_;
  typedef Geometry<MODEL>            Geometry_;
  typedef ModelAuxControl<MODEL>     ModelAux_;
  typedef ModelAuxIncrement<MODEL>   ModelAuxIncr_;
  typedef State<MODEL>               State_;

 public:
  static const std::string classname() {return "oops::LinearModel";}

  LinearModel(const Geometry_ &, const eckit::Configuration &);
  ~LinearModel();

// Set the linearization trajectory
  void setTrajectory(const State_ &, State_ &, const ModelAux_ &);

/// Run the tangent linear forecast
  void forecastTL(Increment_ &, const ModelAuxIncr_ &, const util::Duration &,
                  PostProcessor<Increment_> post = PostProcessor<Increment_>(),
                  PostProcessorTL<Increment_> cost = PostProcessorTL<Increment_>()) const;

/// Run the adjoint forecast
  void forecastAD(Increment_ &, ModelAuxIncr_ &, const util::Duration &,
                  PostProcessor<Increment_> post = PostProcessor<Increment_>(),
                  PostProcessorAD<Increment_> cost = PostProcessorAD<Increment_>()) const;

// Information and diagnostics
  const util::Duration & timeResolution() const {return tlm_->timeResolution();}

 private:
  void print(std::ostream &) const;
  boost::scoped_ptr<LinearModel_> tlm_;

// Run the TL forecast
  void initializeTL(Increment_ &) const;
  void stepTL(Increment_ &, const ModelAuxIncr_ &) const;
  void finalizeTL(Increment_ &) const;

// Run the AD forecast
  void initializeAD(Increment_ &) const;
  void stepAD(Increment_ &, ModelAuxIncr_ &) const;
  void finalizeAD(Increment_ &) const;
};

// =============================================================================

template<typename MODEL>
LinearModel<MODEL>::LinearModel(const Geometry_ & resol, const eckit::Configuration & conf)
  : tlm_()
{
  Log::trace() << "LinearModel<MODEL>::LinearModel starting" << std::endl;
  util::Timer timer(classname(), "LinearModel");
  Log::debug() << "LinearModel config is:" << conf << std::endl;
  tlm_.reset(new LinearModel_(resol.geometry(), conf));
  Log::trace() << "LinearModel<MODEL>::LinearModel done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
LinearModel<MODEL>::~LinearModel() {
  Log::trace() << "LinearModel<MODEL>::~LinearModel starting" << std::endl;
  util::Timer timer(classname(), "~LinearModel");
  tlm_.reset();
  Log::trace() << "LinearModel<MODEL>::~LinearModel done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::setTrajectory(const State_ & xx, State_ & xlr,
                                       const ModelAux_ & maux) {
  Log::trace() << "LinearModel<MODEL>::setTrajectory starting" << std::endl;
  util::Timer timer(classname(), "setTrajectory");
  tlm_->setTrajectory(xx.state(), xlr.state(), maux.modelauxcontrol());
  Log::trace() << "LinearModel<MODEL>::setTrajectory done" << std::endl;
}

// -----------------------------------------------------------------------------
/// Run forecast TL and AD
// -----------------------------------------------------------------------------
//  NOTE: The forecastTL and forecastAD methods are not from MODEL::LinearModel
// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::forecastTL(Increment_ & dx, const ModelAuxIncr_ & mctl,
                                    const util::Duration & len,
                                    PostProcessor<Increment_> post,
                                    PostProcessorTL<Increment_> cost) const {
  Log::trace() << "Increment<MODEL>::forecastTL starting" << std::endl;
  util::Timer timer(classname(), "forecastTL");

  const util::DateTime end(dx.validTime() + len);
  Log::info() << "Increment:forecastTL: Starting " << dx << std::endl;
  this->initializeTL(dx);
  cost.initializeTL(dx, end, tlm_->timeResolution());
  post.initialize(dx, end, tlm_->timeResolution());
  while (dx.validTime() < end) {
    this->stepTL(dx, mctl);
    cost.processTL(dx);
    post.process(dx);
  }
  cost.finalizeTL(dx);
  post.finalize(dx);
  this->finalizeTL(dx);
  Log::info() << "Increment:forecastTL: Finished " << dx << std::endl;
  ASSERT(dx.validTime() == end);

  Log::trace() << "Increment<MODEL>::forecastTL done" << std::endl;
}

// -----------------------------------------------------------------------------
//  NOTE: The forecastTL and forecastAD methods are not from MODEL::LinearModel
// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::forecastAD(Increment_ & dx, ModelAuxIncr_ & mctl,
                                    const util::Duration & len,
                                    PostProcessor<Increment_> post,
                                    PostProcessorAD<Increment_> cost) const {
  Log::trace() << "Increment<MODEL>::forecastAD starting" << std::endl;
  util::Timer timer(classname(), "forecastAD");

  const util::DateTime bgn(dx.validTime() - len);
  Log::info() << "Increment:forecastAD: Starting " << dx << std::endl;
  this->initializeAD(dx);
  post.initialize(dx, bgn, tlm_->timeResolution());
  cost.initializeAD(dx, bgn, tlm_->timeResolution());
  while (dx.validTime() > bgn) {
    cost.processAD(dx);
    this->stepAD(dx, mctl);
    post.process(dx);
  }
  cost.finalizeAD(dx);
  post.finalize(dx);
  this->finalizeAD(dx);
  Log::info() << "Increment:forecastAD: Finished " << dx << std::endl;
  ASSERT(dx.validTime() == bgn);

  Log::trace() << "Increment<MODEL>::forecastAD done" << std::endl;
}

// -----------------------------------------------------------------------------
//  NOTE: The forecastTL and forecastAD methods are not from MODEL::LinearModel
// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::initializeTL(Increment_ & dx) const {
  Log::trace() << "LinearModel<MODEL>::initializeTL starting" << std::endl;
  util::Timer timer(classname(), "initializeTL");
  tlm_->initializeTL(dx.increment());
  Log::trace() << "LinearModel<MODEL>::initializeTL done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::stepTL(Increment_ & dx, const ModelAuxIncr_ & merr) const {
  Log::trace() << "LinearModel<MODEL>::stepTL starting" << std::endl;
  util::Timer timer(classname(), "stepTL");
  tlm_->stepTL(dx.increment(), merr.modelauxincrement());
  Log::trace() << "LinearModel<MODEL>::stepTL done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::finalizeTL(Increment_ & dx) const {
  Log::trace() << "LinearModel<MODEL>::finalizeTL starting" << std::endl;
  util::Timer timer(classname(), "finalizeTL");
  tlm_->finalizeTL(dx.increment());
  Log::trace() << "LinearModel<MODEL>::finalizeTL done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::initializeAD(Increment_ & dx) const {
  Log::trace() << "LinearModel<MODEL>::initializeAD starting" << std::endl;
  util::Timer timer(classname(), "initializeAD");
  tlm_->initializeAD(dx.increment());
  Log::trace() << "LinearModel<MODEL>::initializeAD done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::stepAD(Increment_ & dx, ModelAuxIncr_ & merr) const {
  Log::trace() << "LinearModel<MODEL>::stepAD starting" << std::endl;
  util::Timer timer(classname(), "stepAD");
  tlm_->stepAD(dx.increment(), merr.modelauxincrement());
  Log::trace() << "LinearModel<MODEL>::stepAD done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::finalizeAD(Increment_ & dx) const {
  Log::trace() << "LinearModel<MODEL>::finalizeAD starting" << std::endl;
  util::Timer timer(classname(), "finalizeAD");
  tlm_->finalizeAD(dx.increment());
  Log::trace() << "LinearModel<MODEL>::finalizeAD done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename MODEL>
void LinearModel<MODEL>::print(std::ostream & os) const {
  Log::trace() << "LinearModel<MODEL>::print starting" << std::endl;
  util::Timer timer(classname(), "print");
  os << *tlm_;
  Log::trace() << "LinearModel<MODEL>::print done" << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_INTERFACE_LINEARMODEL_H_
