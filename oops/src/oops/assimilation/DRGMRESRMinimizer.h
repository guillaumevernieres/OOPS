/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_DRGMRESRMINIMIZER_H_
#define OOPS_ASSIMILATION_DRGMRESRMINIMIZER_H_

#include <cmath>
#include <string>
#include <vector>

#include "util/Logger.h"
#include "oops/assimilation/BMatrix.h"
#include "oops/assimilation/ControlIncrement.h"
#include "oops/assimilation/CostFunction.h"
#include "oops/assimilation/HtRinvHMatrix.h"
#include "oops/assimilation/DRMinimizer.h"
#include "oops/base/IdentityMatrix.h"
#include "util/dot_product.h"
#include "util/formats.h"

namespace oops {

/// Derber-Rosati GMRESR Minimizer
/*!
 * \brief Derber-Rosati GMRESR solver for Ax=b.
 *
 * DRGMRESR solver for Ax=b.  This is a "double" version of GMRESR
 * (H.A. Van der Vorst  and C. Vuik, 1994, Numerical Linear Algebra with
 * Applications, 1(4), 369-386) following Derber and Rosati (J. Derber and
 * A. Rosati, 1989, J. Phys. Oceanog. 1333-1347).
 * It solves Ax=b for the particular case \f$ A=B^{-1}+C\f$, without
 * requiring the application of \f$ B^{-1}\f$. A must be square, but need
 * not be symmetric. (For a symmetric matrix, and constant preconditioner,
 * DRGMRESR is simply Derber-Rosati PCG with full orthogonalisation.)
 * A preconditioner must be supplied that, given a vector q, returns an
 * approximate solution of \f$ (AB)^{-1} q\f$. The preconditioner can be
 * variable. Note that the traditional \f$ B\f$-preconditioning corresponds
 * to precond=\f$I\f$.
 *
 *  On entry:
 * -    x       =  starting point, \f$ X_0 \f$.
 * -    xh      = \f$ B^{-1} x_0\f$.
 * -    B       = \f$ B \f$.
 * -    C       = \f$ C \f$.
 * -    precond = preconditioner matrix \f$ F_k \approx (AB)^{-1} \f$.
 *
 * On exit, x will contain the solution \f$ x \f$ and xh will contain
 * \f$ B^{-1} x\f$.
 *
 *  Matrices must implement a method:
 *  - void multiply(const VECTOR&, VECTOR&) const
 *
 *  which applies the matrix to the first argument, and returns the
 *  matrix-vector product in the second. (Note: the const is optional, but
 *  recommended.)
 *
 *  Iteration will stop if the maximum iteration limit "maxiter" is reached
 *  or if the residual norm reduces by a factor of "tolerance".
 *
 *  The return value is the achieved reduction in residual norm.
 */

// -----------------------------------------------------------------------------

template<typename MODEL> class DRGMRESRMinimizer : public DRMinimizer<MODEL> {
  typedef BMatrix<MODEL>             Bmat_;
  typedef CostFunction<MODEL>        CostFct_;
  typedef ControlIncrement<MODEL>    CtrlInc_;
  typedef HtRinvHMatrix<MODEL>       HtRinvH_;

 public:
  const std::string classname() const override {return "DRGMRESRMinimizer";}
  DRGMRESRMinimizer(const eckit::Configuration &, const CostFct_ & J): DRMinimizer<MODEL>(J) {}
  ~DRGMRESRMinimizer() {}
 private:
  double solve(CtrlInc_ &, CtrlInc_ &, CtrlInc_ &, const Bmat_ &, const HtRinvH_ &,
               const double, const double, const int, const double) override;
};

// =============================================================================

template<typename MODEL>
double DRGMRESRMinimizer<MODEL>::solve(CtrlInc_ & xx, CtrlInc_ & xh, CtrlInc_ & rr,
                                      const Bmat_ & B, const HtRinvH_ & HtRinvH,
                                      const double costJ0Jb, const double costJ0JoJc, 
                                      const int maxiter, const double tolerance) {
  IdentityMatrix<CtrlInc_> precond;
  std::vector<CtrlInc_> c;
  std::vector<CtrlInc_> u;
  std::vector<CtrlInc_> uh;
  CtrlInc_ cc(xh);
  CtrlInc_ zz(xh);
  CtrlInc_ zh(xh);

  double dotRr0  = dot_product(rr, rr);
  double normReduction = 1.0;

  Log::info() << std::endl;
  for (int jiter = 0; jiter < maxiter; ++jiter) {
    Log::info() << " DRGMRESR Starting Iteration " << jiter+1 << std::endl;

    precond.multiply(rr, zh);  // returns zh as approxmate solve of (BA)zh = r
    B.multiply(zh, zz);        // x=B zh
    HtRinvH.multiply(zz, cc);
    cc += zh;                  // cc = zh+Cz = Az

    for (int jj = 0; jj < jiter; ++jj) {
      double alpha = -dot_product(c[jj], cc);
      cc.axpy(alpha, c[jj]);   // cc = cc - <c[jj],cc>*c[jj];
      zz.axpy(alpha, u[jj]);   // zz = zz - <c[jj],cc>*u[jj];
      zh.axpy(alpha, uh[jj]);  // zh = zh - <c[jj],cc>*uh[jj];
    }

    double ccnorm = sqrt(dot_product(cc, cc));
    c.push_back(cc);         // c[jiter]=cc
    c[jiter] *= 1.0/ccnorm;
    u.push_back(zz);         // u[jiter]=z
    u[jiter] *= 1.0/ccnorm;
    uh.push_back(zh);        // uh[jiter]=zh
    uh[jiter] *= 1.0/ccnorm;

    double cdotr = dot_product(c[jiter], rr);
    xx.axpy(cdotr,  u[jiter]);
    xh.axpy(cdotr, uh[jiter]);
    rr.axpy(-cdotr, c[jiter]);

    normReduction = sqrt(dot_product(rr, rr)/dotRr0);
    Log::info() << "DRGMRESR end of iteration " << jiter+1 << ". Norm reduction= "
                << util::full_precision(normReduction) << std::endl << std::endl;

    if (normReduction < tolerance) {
      Log::info() << "DRGMRESR: Achieved required reduction in residual norm." << std::endl;
      break;
    }
  }
  return normReduction;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_ASSIMILATION_DRGMRESRMINIMIZER_H_
