// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>

#include "lib/extras/codec.h"
#include "lib/extras/dec/decode.h"
#include "lib/extras/packed_image_convert.h"
#include "lib/jxl/base/common.h"
#include "lib/jxl/image_bundle.h"
#include "tools/cmdline.h"
#include "tools/file_io.h"
#include "tools/hdr/image_utils.h"
#include "tools/thread_pool_internal.h"

int main(int argc, const char** argv) {
  jpegxl::tools::ThreadPoolInternal pool;

  jpegxl::tools::CommandLineParser parser;
  float intensity_target = jxl::kDefaultIntensityTarget;
  parser.AddOptionValue('t', "intensity_target", "<nits>", "intensity target",
                        &intensity_target, &jpegxl::tools::ParseFloat, 0);
  const char* input_filename = nullptr;
  auto input_filename_option = parser.AddPositionalOption(
      "input", true, "input image", &input_filename, 0);
  const char* output_filename = nullptr;
  auto output_filename_option = parser.AddPositionalOption(
      "output", true, "output image", &output_filename, 0);

  if (!parser.Parse(argc, argv)) {
    fprintf(stderr, "See -h for help.\n");
    return EXIT_FAILURE;
  }

  if (parser.HelpFlagPassed()) {
    parser.PrintHelp();
    return EXIT_SUCCESS;
  }

  if (!parser.GetOption(input_filename_option)->matched()) {
    fprintf(stderr, "Missing input filename.\nSee -h for help.\n");
    return EXIT_FAILURE;
  }
  if (!parser.GetOption(output_filename_option)->matched()) {
    fprintf(stderr, "Missing output filename.\nSee -h for help.\n");
    return EXIT_FAILURE;
  }

  jxl::extras::PackedPixelFile ppf;
  std::vector<uint8_t> input_bytes;
  JXL_CHECK(jpegxl::tools::ReadFile(input_filename, &input_bytes));
  JXL_CHECK(jxl::extras::DecodeBytes(jxl::Span<const uint8_t>(input_bytes),
                                     jxl::extras::ColorHints(), &ppf));

  jxl::CodecInOut image;
  JXL_CHECK(
      jxl::extras::ConvertPackedPixelFileToCodecInOut(ppf, &pool, &image));
  image.metadata.m.bit_depth.exponent_bits_per_sample = 0;
  jxl::ColorEncoding linear_srgb = jxl::ColorEncoding::LinearSRGB();
  JXL_CHECK(jpegxl::tools::TransformCodecInOutTo(image, linear_srgb, &pool));
  jxl::ScaleImage(image.metadata.m.IntensityTarget() / intensity_target,
                  image.Main().color());
  image.metadata.m.SetIntensityTarget(intensity_target);
  image.metadata.m.color_encoding = linear_srgb;
  std::vector<uint8_t> encoded;
  JXL_CHECK(jxl::Encode(image, output_filename, &encoded, &pool));
  JXL_CHECK(jpegxl::tools::WriteFile(output_filename, encoded));
}
