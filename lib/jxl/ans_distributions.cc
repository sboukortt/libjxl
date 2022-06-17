#include "lib/jxl/ans_distributions.h"

namespace jxl {

void FillDistributions(const int alphabet_size, const int num_histograms,
                       const int precision, Rng& rng,
                       std::vector<int>* distributions) {
  for (int j = 0; j < num_histograms; j++) {
    distributions[j].resize(precision);
    int symbol = 0;
    int remaining = 1;
    for (int k = 0; k < precision; k++) {
      if (remaining == 0) {
        if (symbol < alphabet_size - 1) symbol++;
        // There is no meaning behind this distribution: it's anything that
        // will create a nonuniform distribution and won't have too few
        // symbols usually. Also we want different distributions we get to be
        // sufficiently dissimilar.
        remaining = rng.UniformU(0, precision - k + 1);
      }
      distributions[j][k] = symbol;
      remaining--;
    }
  }
}

}  // namespace jxl
