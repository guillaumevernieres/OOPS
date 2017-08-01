/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_HTRINVHMATRIX_H_
#define OOPS_ASSIMILATION_HTRINVHMATRIX_H_

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "oops/assimilation/CostFunction.h"
#include "oops/assimilation/ControlIncrement.h"
#include "oops/base/PostProcessorTL.h"
#include "oops/base/PostProcessorAD.h"
#include "oops/interface/Increment.h"

namespace oops {

/// The \f$ H^T R^{-1} H \f$ matrix.
/*!
 *  The solvers represent matrices as objects that implement a "multiply"
 *  method. This class defines objects that apply a generalized
 *  \f$ H^T R^{-1} H \f$ matrix that also includes the equivalent
 *  operators for the other terms of the cost function.
 */

template<typename MODEL> class HtRinvHMatrix : private boost::noncopyable {
  typedef Increment<MODEL>           Increment_;
  typedef ControlIncrement<MODEL>    CtrlInc_;
  typedef CostFunction<MODEL>        CostFct_;

 public:
  explicit HtRinvHMatrix(const CostFct_ & j): j_(j) {}

  void multiply(const CtrlInc_ & dx, CtrlInc_ & dz) const {
//  Setup TL terms of cost function
    PostProcessorTL<Increment_> costtl;
    for (unsigned jj = 0; jj < j_.nterms(); ++jj) {
      costtl.enrollProcessor(j_.jterm(jj).setupTL(dx));
    }

//  Run TLM
    j_.runTLM(dx, costtl);

//  Get TLM outputs, multiply by covariance inverses, and setup ADJ forcing terms
    j_.zeroAD(dz);
    PostProcessorAD<Increment_> costad;
    for (unsigned jj = 0; jj < j_.nterms(); ++jj) {
      boost::scoped_ptr<GeneralizedDepartures> ww(costtl.releaseOutputFromTL(jj));
      boost::shared_ptr<GeneralizedDepartures> zz(j_.jterm(jj).multiplyCoInv(*ww));
      costad.enrollProcessor(j_.jterm(jj).setupAD(zz, dz));
    }

//  Run ADJ
    j_.runADJ(dz, costad);
  }

 private:
  CostFct_ const & j_;
};

}  // namespace oops

#endif  // OOPS_ASSIMILATION_HTRINVHMATRIX_H_
