/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_DRPCGMINIMIZER_H_
#define OOPS_ASSIMILATION_DRPCGMINIMIZER_H_

#include <cmath>
#include <string>
#include <vector>

#include "eckit/config/Configuration.h"
#include "util/Logger.h"
#include "oops/assimilation/BMatrix.h"
#include "oops/assimilation/ControlIncrement.h"
#include "oops/assimilation/CostFunction.h"
#include "oops/assimilation/DRMinimizer.h"
#include "oops/assimilation/HtRinvHMatrix.h"
#include "oops/assimilation/QNewtonLMP.h"
#include "util/dot_product.h"
#include "util/formats.h"

namespace oops {

/// DRPCG Minimizer
/*!
 * \brief Derber-Rosati Preconditioned Conjugate Gradients solver.
 *
 * This solver is based on the standard Preconditioned Conjugate
 * Gradients solver for Ax=b (G. H. Golub and C. F. Van Loan, Matrix
 * Computations), and on the Derber and Rosati double
 * PCG algorithm (J. Derber and A. Rosati, 1989, J. Phys. Oceanog. 1333-1347).
 * For details see S. Gurol, PhD Manuscript, 2013.
 * It solves \f$ Ax=b\f$ for the particular case \f$ A=B^{-1}+C\f$,
 * without requiring the application of \f$ B^{-1}\f$. This algorithm
 * is similar to DRIPCG except it includes standard PCG instead IPCG
 * and stopping criteria is based on the preconditioner norm.
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
 * -    x       =  starting point, \f$ X_0 \f$.
 * -    xh      = \f$ B^{-1} x_0\f$.
 * -    b       = right hand side.
 * -    B       = \f$ B \f$.
 * -    C       = \f$ C \f$.
 * -    precond = preconditioner \f$ F_k \approx (AB)^{-1} \f$.
 *
 * On exit, x will contain the solution \f$ x \f$ and xh will contain
 * \f$ B^{-1} x\f$.
 *  The return value is the achieved reduction in preconditioned residual norm.
 *
 *  Iteration will stop if the maximum iteration limit "maxiter" is reached
 *  or if the residual norm reduces by a factor of "tolerance".
 *
 *  Each matrix must implement a method:
 *  - void multiply(const VECTOR&, VECTOR&) const
 *
 *  which applies the matrix to the first argument, and returns the
 *  matrix-vector product in the second. (Note: the const is optional, but
 *  recommended.)
 */

// -----------------------------------------------------------------------------

template<typename MODEL> class DRPCGMinimizer : public DRMinimizer<MODEL> {
  typedef BMatrix<MODEL>             Bmat_;
  typedef CostFunction<MODEL>        CostFct_;
  typedef ControlIncrement<MODEL>    CtrlInc_;
  typedef HtRinvHMatrix<MODEL>       HtRinvH_;

 public:
  const std::string classname() const {return "DRPCGMinimizer";}
  DRPCGMinimizer(const eckit::Configuration &, const CostFct_ &);
  ~DRPCGMinimizer() {}

 private:
  double solve(CtrlInc_ &, CtrlInc_ &, CtrlInc_ &, const Bmat_ &, const HtRinvH_ &,
               const int, const double);

  QNewtonLMP<CtrlInc_, Bmat_> lmp_;
};

// =============================================================================

template<typename MODEL>
DRPCGMinimizer<MODEL>::DRPCGMinimizer(const eckit::Configuration & conf, const CostFct_ & J)
  : DRMinimizer<MODEL>(J), lmp_(conf)
{}

// -----------------------------------------------------------------------------

template<typename MODEL>
double DRPCGMinimizer<MODEL>::solve(CtrlInc_ & xx, CtrlInc_ & xh, CtrlInc_ & rr,
                                   const Bmat_ & B, const HtRinvH_ & HtRinvH,
                                   const int maxiter, const double tolerance) {
  CtrlInc_ ap(xh);
  CtrlInc_ pp(xh);
  CtrlInc_ ph(xh);
  CtrlInc_ ss(xh);
  CtrlInc_ sh(xh);
  CtrlInc_ ww(xh);

  std::vector<CtrlInc_> vvecs;  // for re-orthogonalization
  std::vector<CtrlInc_> zvecs;  // for re-orthogonalization
  std::vector<double> scals;  // for re-orthogonalization

  Log::info() << "normr0 " << dot_product(rr, rr) << std::endl;

  // s = B precond r
  lmp_.multiply(rr, sh);
  B.multiply(sh, ss);

  double dotRr0  = dot_product(rr, ss);
  double normReduction = 1.0;
  double rdots = dotRr0;
  double rdots_old = 0.0;

  vvecs.push_back(rr);
  zvecs.push_back(ss);
  scals.push_back(1.0/dotRr0);

  Log::info() << std::endl;
  for (int jiter = 0; jiter < maxiter; ++jiter) {
    Log::info() << " DRPCG Starting Iteration " << jiter+1 << std::endl;

    if (jiter == 0) {
      pp = ss;
      ph = sh;
    } else {
      double beta = rdots/rdots_old;
      Log::debug() << "DRPCG beta = " << beta << std::endl;

      pp *= beta;
      pp += ss;    // p = s + beta*p

      ph *= beta;
      ph += sh;    // ph = sh + beta*ph
    }

    // (Binv + HtRinvH) p
    HtRinvH.multiply(pp, ap);
    ap += ph;

    double rho = dot_product(pp, ap);
    double alpha = rdots/rho;

    xx.axpy(alpha, pp);   // xx = xx + alpha*pp
    xh.axpy(alpha, ph);   // xh = xh + alpha*ph
    rr.axpy(-alpha, ap);  // rr = rr - alpha*ap

    // Re-orthogonalization
    for (int jj = 0; jj < jiter; ++jj) {
      double proj = scals[jj] * dot_product(rr, zvecs[jj]);
      rr.axpy(-proj, vvecs[jj]);
    }

    lmp_.multiply(rr, sh);
    B.multiply(sh, ss);

    rdots_old = rdots;
    rdots = dot_product(rr, ss);

    Log::info() << "rdots " << rdots << "      iteration    " << jiter << std::endl;

    normReduction = sqrt(rdots/dotRr0);

    Log::info() << "DRPCG end of iteration " << jiter+1 << ". Norm reduction= "
                << util::full_precision(normReduction) << std::endl << std::endl;

    // Save the pairs for preconditioning
    lmp_.push(pp, ph, ap, rho);

    if (normReduction < tolerance) {
      Log::info() << "DRPCG: Achieved required reduction in residual norm." << std::endl;
      break;
    }

    vvecs.push_back(rr);
    zvecs.push_back(ss);
    scals.push_back(1.0/rdots);
  }

// Generate the (second-level) Limited Memory Preconditioner
  lmp_.update(B);

  return normReduction;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_ASSIMILATION_DRPCGMINIMIZER_H_
