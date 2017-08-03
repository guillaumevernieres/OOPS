/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_DRPLANCZOSMINIMIZER_H_
#define OOPS_ASSIMILATION_DRPLANCZOSMINIMIZER_H_

#include <cmath>
#include <string>
#include <vector>

#include <boost/ptr_container/ptr_vector.hpp>

#include "util/Logger.h"
#include "oops/assimilation/BMatrix.h"
#include "oops/assimilation/ControlIncrement.h"
#include "oops/assimilation/CostFunction.h"
#include "oops/assimilation/DRMinimizer.h"
#include "oops/assimilation/HtRinvHMatrix.h"
#include "oops/assimilation/SpectralLMP.h"
#include "oops/assimilation/TriDiagSolve.h"
#include "util/dot_product.h"
#include "util/formats.h"

namespace oops {

/// DRPLanczos Minimizer
/*!
 * \brief Derber-Rosati Preconditioned Lanczos solver.
 *
 * This solver is the Lanczos version of the DRPCG algorithm
 * It solves \f$ Ax=b\f$ for the particular case \f$ A=B^{-1}+C\f$,
 * without requiring the application of \f$ B^{-1}\f$.
 *
 * A must be square, symmetric, positive definite.
 *
 * A preconditioner must be supplied that, given a vector q, returns an
 * approximation to \f$ (AB)^{-1} q\f$. Possible preconditioning
 * is detailed in S. Gurol, PhD Manuscript, 2013.
 * Note that the traditional \f$ B\f$-preconditioning corresponds to
 * precond=\f$I\f$.
 *
 * On entry:
 * -    xh      = starting point, \f$ B^{-1} x_0\f$.
 * -    rr      = residual at starting point.
 * -    B       = \f$ B \f$.
 * -    C       = \f$ C \f$.
 * -    precond = preconditioner \f$ F_k \approx (AB)^{-1} \f$.
 *
 * On exit, xh will contain \f$ B^{-1} x\f$ where x is the solution.
 * The return value is the achieved reduction in residual norm.
 *
 * Iteration will stop if the maximum iteration limit "maxiter" is reached
 * or if the residual norm reduces by a factor of "tolerance".
 *
 * Each matrix must implement a method:
 * - void multiply(const VECTOR&, VECTOR&) const
 *
 * which applies the matrix to the first argument, and returns the
 * matrix-vector product in the second. (Note: the const is optional, but
 * recommended.)
 */

// -----------------------------------------------------------------------------

template<typename MODEL> class DRPLanczosMinimizer : public DRMinimizer<MODEL> {
  typedef BMatrix<MODEL>             Bmat_;
  typedef CostFunction<MODEL>        CostFct_;
  typedef ControlIncrement<MODEL>    CtrlInc_;
  typedef HtRinvHMatrix<MODEL>       HtRinvH_;

 public:
  const std::string classname() const {return "DRPLanczosMinimizer";}
  DRPLanczosMinimizer(const eckit::Configuration &, const CostFct_ &);
  ~DRPLanczosMinimizer() {}

 private:
  double solve(CtrlInc_ &, CtrlInc_ &, CtrlInc_ &, const Bmat_ &, const HtRinvH_ &,
               const int, const double);

  SpectralLMP<CtrlInc_> lmp_;

  boost::ptr_vector<CtrlInc_> hvecs_;
  boost::ptr_vector<CtrlInc_> vvecs_;
  boost::ptr_vector<CtrlInc_> zvecs_;
  std::vector<double> alphas_;
  std::vector<double> betas_;
};

// =============================================================================

template<typename MODEL>
DRPLanczosMinimizer<MODEL>::DRPLanczosMinimizer(const eckit::Configuration & conf,
                                              const CostFct_ & J)
  : DRMinimizer<MODEL>(J), lmp_(conf),
    hvecs_(), vvecs_(), zvecs_(), alphas_(), betas_() {}

// -----------------------------------------------------------------------------

template<typename MODEL>
double DRPLanczosMinimizer<MODEL>::solve(CtrlInc_ & xx, CtrlInc_ & xh, CtrlInc_ & rr,
                                        const Bmat_ & B, const HtRinvH_ & HtRinvH,
                                        const int maxiter, const double tolerance) {
  CtrlInc_ zz(xh);
  CtrlInc_ zh(xh);
  CtrlInc_ vv(rr);

  std::vector<double> yy;
  std::vector<double> dd;

  lmp_.update(vvecs_, hvecs_, zvecs_, alphas_, betas_);

  // z = B precond r
  lmp_.multiply(vv, zh);
  B.multiply(zh, zz);

  double normReduction = 1.0;
  double beta = sqrt(dot_product(zz, vv));
  const double beta0 = beta;

  vv *= 1/beta;
  zh *= 1/beta;
  zz *= 1/beta;

  hvecs_.push_back(new CtrlInc_(zh));  // hvecs[0] = zh_1 --> required for solution
  zvecs_.push_back(new CtrlInc_(zz));  // zvecs[0] = z_1 ---> for re-orthogonalization
  vvecs_.push_back(new CtrlInc_(vv));  // vvecs[0] = v_1 ---> for re-orthogonalization

  Log::info() << std::endl;
  for (int jiter = 0; jiter < maxiter; ++jiter) {
    Log::info() << "DRPLanczos Starting Iteration " << jiter+1 << std::endl;

    // vv = A z - beta * v_{j-1}
    HtRinvH.multiply(zz, vv);
    vv += zh;            // vv = A zz
    if (jiter > 0) vv.axpy(-beta, vvecs_[jiter-1]);  // vv = vv - beta * v_{j-1}

    double alpha = dot_product(zz, vv);
    vv.axpy(-alpha, vvecs_[jiter]);  // vv = vv - alpha * v_j

    // Re-orthogonalization
    for (int jj = 0; jj < jiter; ++jj) {
      double proj = dot_product(vv, zvecs_[jj]);
      vv.axpy(-proj, vvecs_[jj]);
    }

    lmp_.multiply(vv, zh);
    B.multiply(zh, zz);  // zz = B precond vv

    beta = sqrt(dot_product(zz, vv));

    vv *= 1/beta;
    zh *= 1/beta;
    zz *= 1/beta;

    hvecs_.push_back(new CtrlInc_(zh));  // hvecs[jiter+1] =zh_jiter
    zvecs_.push_back(new CtrlInc_(zz));  // zvecs[jiter+1] = z_jiter
    vvecs_.push_back(new CtrlInc_(vv));  // vvecs[jiter+1] = v_jiter

    alphas_.push_back(alpha);

    if (jiter == 0) {
      yy.push_back(beta0/alpha);
      dd.push_back(beta0);
    } else {
      // Solve the tridiagonal system T_jiter y_jiter = beta0 * e_1
      dd.push_back(beta0*dot_product(zvecs_[0], vv));
      TriDiagSolve(alphas_, betas_, dd, yy);
    }

    betas_.push_back(beta);

    // Gradient norm in precond metric --> sqrt(r'z) --> beta * y(jiter)
    double rznorm = beta*std::abs(yy[jiter]);
    normReduction = rznorm/beta0;

    Log::info() << "DRPLanczos end of iteration " << jiter+1 << ". Norm reduction= "
                << util::full_precision(normReduction) << std::endl << std::endl;

    if (normReduction < tolerance) {
      Log::info() << "DRPLanczos: Achieved required reduction in residual norm." << std::endl;
      break;
    }
  }

  // Calculate the solution (xh = Binv xx)
  for (int jj = 0; jj < yy.size(); ++jj) {
    xx.axpy(yy[jj], zvecs_[jj]);
    xh.axpy(yy[jj], hvecs_[jj]);
  }

  return normReduction;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_ASSIMILATION_DRPLANCZOSMINIMIZER_H_
