#ifndef QG_INSTANTIATETLMFACTORY_H_
#define QG_INSTANTIATETLMFACTORY_H_

#include "model/QgTraits.h"
#include "model/QgTLM.h"
#include "model/QgIdTLM.h"
#include "oops/interface/LinearModelBase.h"

typedef qg::QgTraits  QG;

namespace qg {
// -----------------------------------------------------------------------------

void instantiateTlmFactory() {
  static oops::LinearModelMaker<QG, QgTLM> makerQGTLM_("QgTLM");
  static oops::LinearModelMaker<QG, QgIdTLM> makerQGIdTLM_("QgIdTLM");
}

// -----------------------------------------------------------------------------
}  // namespace qg

#endif  // QG_INSTANTIATETLMFACTORY_H_
