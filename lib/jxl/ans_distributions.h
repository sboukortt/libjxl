#ifndef LIB_JXL_ANS_DISTRIBUTIONS_H_
#define LIB_JXL_ANS_DISTRIBUTIONS_H_

#include <vector>

#include "lib/jxl/base/random.h"

namespace jxl {

void FillDistributions(int alphabet_size, int num_histograms, int precision,
                       Rng& rng, std::vector<int>* distributions);

}

#endif  // LIB_JXL_ANS_DISTRIBUTIONS_H_
