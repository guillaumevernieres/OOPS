#ifndef LORENZ95_INSTANTIATETLMFACTORY_H_
#define LORENZ95_INSTANTIATETLMFACTORY_H_

#include "lorenz95/L95Traits.h"
#include "lorenz95/TLML95.h"
#include "oops/interface/LinearModelBase.h"

typedef lorenz95::L95Traits  L95;

namespace lorenz95 {
// -----------------------------------------------------------------------------

void instantiateTlmFactory() {
  static oops::LinearModelMaker<L95, TLML95> makerTLML95_("L95TLM");
}

// -----------------------------------------------------------------------------
}  // namespace lorenz95

#endif  // LORENZ95_INSTANTIATETLMFACTORY_H_
