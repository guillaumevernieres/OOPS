/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_DRMINIMIZER_H_
#define OOPS_ASSIMILATION_DRMINIMIZER_H_

#include <string>

#include <boost/scoped_ptr.hpp>

#include "util/Logger.h"
#include "oops/assimilation/BMatrix.h"
#include "oops/assimilation/ControlIncrement.h"
#include "oops/assimilation/ControlVariable.h"
#include "oops/assimilation/CostFunction.h"
#include "oops/assimilation/HtRinvHMatrix.h"
#include "oops/assimilation/Minimizer.h"
#include "eckit/config/Configuration.h"

namespace oops {

/// DR (Derber and Rosati) Minimizers
/*!
 * DRMinimizer is the base class for all minimizers that use \f$ B\f$ to
 * precondition the variational minimisation problem and use the auxiliary
 * variable \f$ \hat{x}=B^{-1}x\f$ and to update it in parallel to \f$ x\f$
 * based on Derber and Rosati, 1989, J. Phys. Oceanog. 1333-1347.
 * \f$ J_b\f$ is then computed as \f$ x^T\hat{x}\f$ eliminating the need for
 * \f$ B^{-1}\f$ or \f$ B^{1/2}\f$.
 */

// -----------------------------------------------------------------------------

template<typename MODEL> class DRMinimizer : public Minimizer<MODEL> {
  typedef CostFunction<MODEL>        CostFct_;
  typedef ControlIncrement<MODEL>    CtrlInc_;
  typedef ControlVariable<MODEL>     CtrlVar_;
  typedef BMatrix<MODEL>             Bmat_;
  typedef HtRinvHMatrix<MODEL>       HtRinvH_;

 public:
  explicit DRMinimizer(const CostFct_ & J): J_(J), gradJb_(0) {}
  ~DRMinimizer() {}
  CtrlInc_ * minimize(const eckit::Configuration &);
  virtual const std::string classname() const =0;

 private:
  virtual double solve(CtrlInc_ &, CtrlInc_ &, CtrlInc_ &, const Bmat_ &,
                       const HtRinvH_ &, const int, const double) =0;

  const CostFct_ & J_;
  boost::scoped_ptr<CtrlInc_> gradJb_;
};

// =============================================================================

template<typename MODEL>
ControlIncrement<MODEL> * DRMinimizer<MODEL>::minimize(const eckit::Configuration & config) {
  int ninner = config.getInt("ninner");
  double gnreduc = config.getDouble("gradient_norm_reduction");
  if (gradJb_ == 0) gradJb_.reset(new CtrlInc_(J_.jb()));

  Log::info() << std::endl;
  Log::info() << classname() << ": max iter = " << ninner <<
                ", requested norm reduction = " << gnreduc << std::endl;

// Define the matrices
  const Bmat_ B(J_);
  const HtRinvH_ HtRinvH(J_);

// Compute RHS
  CtrlInc_ rhs(J_.jb());
  J_.computeGradientFG(rhs);
  J_.jb().addGradientFG(rhs, *gradJb_);
  rhs *= -1.0;
  Log::info() << classname() << " rhs" << rhs << std::endl;

// Define minimisation starting point
  CtrlInc_ dh(J_.jb());
  CtrlInc_ * dx = new CtrlInc_(J_.jb());

// Solve the linear system
  double reduc = this->solve(*dx, dh, rhs, B, HtRinvH, ninner, gnreduc);

  Log::test() << classname() << ": reduction in residual norm = " << reduc << std::endl;
  Log::info() << classname() << " output increment:" << *dx << std::endl;

// Update gradient Jb
  *gradJb_ += dh;

  return dx;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_ASSIMILATION_DRMINIMIZER_H_
