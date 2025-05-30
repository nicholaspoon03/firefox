/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Utility.h"
#include "mozilla/dom/TypedArray.h"
#include "mozilla/dom/WebGPUBinding.h"
#include "mozilla/webgpu/ffi/wgpu.h"
#include "mozilla/webgpu/WebGPUTypes.h"

namespace mozilla::webgpu {

template <typename E>
void ConvertToExtent3D(const E& aExtent, ffi::WGPUExtent3d* aExtentFFI) {
  *aExtentFFI = {};
  if (aExtent.IsRangeEnforcedUnsignedLongSequence()) {
    const auto& seq = aExtent.GetAsRangeEnforcedUnsignedLongSequence();
    aExtentFFI->width = seq.Length() > 0 ? seq[0] : 0;
    aExtentFFI->height = seq.Length() > 1 ? seq[1] : 1;
    aExtentFFI->depth_or_array_layers = seq.Length() > 2 ? seq[2] : 1;
  } else if (aExtent.IsGPUExtent3DDict()) {
    const auto& dict = aExtent.GetAsGPUExtent3DDict();
    aExtentFFI->width = dict.mWidth;
    aExtentFFI->height = dict.mHeight;
    aExtentFFI->depth_or_array_layers = dict.mDepthOrArrayLayers;
  } else {
    MOZ_CRASH("Unexpected extent type");
  }
}

void ConvertExtent3DToFFI(const dom::GPUExtent3D& aExtent,
                          ffi::WGPUExtent3d* aExtentFFI) {
  ConvertToExtent3D(aExtent, aExtentFFI);
}

void ConvertExtent3DToFFI(const dom::OwningGPUExtent3D& aExtent,
                          ffi::WGPUExtent3d* aExtentFFI) {
  ConvertToExtent3D(aExtent, aExtentFFI);
}

ffi::WGPUExtent3d ConvertExtent(const dom::GPUExtent3D& aExtent) {
  ffi::WGPUExtent3d extent = {};
  ConvertToExtent3D(aExtent, &extent);
  return extent;
}

ffi::WGPUExtent3d ConvertExtent(const dom::OwningGPUExtent3D& aExtent) {
  ffi::WGPUExtent3d extent = {};
  ConvertToExtent3D(aExtent, &extent);
  return extent;
}

ffi::WGPUCompareFunction ConvertCompareFunction(
    const dom::GPUCompareFunction& aCompare) {
  // Value of 0 = Undefined is reserved on the C side for "null" semantics.
  return ffi::WGPUCompareFunction(UnderlyingValue(aCompare) + 1);
}

ffi::WGPUTextureFormat ConvertTextureFormat(
    const dom::GPUTextureFormat& aFormat) {
  ffi::WGPUTextureFormat result = {ffi::WGPUTextureFormat_Sentinel};
  switch (aFormat) {
    case dom::GPUTextureFormat::R8unorm:
      result.tag = ffi::WGPUTextureFormat_R8Unorm;
      break;
    case dom::GPUTextureFormat::R8snorm:
      result.tag = ffi::WGPUTextureFormat_R8Snorm;
      break;
    case dom::GPUTextureFormat::R8uint:
      result.tag = ffi::WGPUTextureFormat_R8Uint;
      break;
    case dom::GPUTextureFormat::R8sint:
      result.tag = ffi::WGPUTextureFormat_R8Sint;
      break;
    case dom::GPUTextureFormat::R16uint:
      result.tag = ffi::WGPUTextureFormat_R16Uint;
      break;
    case dom::GPUTextureFormat::R16sint:
      result.tag = ffi::WGPUTextureFormat_R16Sint;
      break;
    case dom::GPUTextureFormat::R16float:
      result.tag = ffi::WGPUTextureFormat_R16Float;
      break;
    case dom::GPUTextureFormat::Rg8unorm:
      result.tag = ffi::WGPUTextureFormat_Rg8Unorm;
      break;
    case dom::GPUTextureFormat::Rg8snorm:
      result.tag = ffi::WGPUTextureFormat_Rg8Snorm;
      break;
    case dom::GPUTextureFormat::Rg8uint:
      result.tag = ffi::WGPUTextureFormat_Rg8Uint;
      break;
    case dom::GPUTextureFormat::Rg8sint:
      result.tag = ffi::WGPUTextureFormat_Rg8Sint;
      break;
    case dom::GPUTextureFormat::R32uint:
      result.tag = ffi::WGPUTextureFormat_R32Uint;
      break;
    case dom::GPUTextureFormat::R32sint:
      result.tag = ffi::WGPUTextureFormat_R32Sint;
      break;
    case dom::GPUTextureFormat::R32float:
      result.tag = ffi::WGPUTextureFormat_R32Float;
      break;
    case dom::GPUTextureFormat::Rg16uint:
      result.tag = ffi::WGPUTextureFormat_Rg16Uint;
      break;
    case dom::GPUTextureFormat::Rg16sint:
      result.tag = ffi::WGPUTextureFormat_Rg16Sint;
      break;
    case dom::GPUTextureFormat::Rg16float:
      result.tag = ffi::WGPUTextureFormat_Rg16Float;
      break;
    case dom::GPUTextureFormat::Rgba8unorm:
      result.tag = ffi::WGPUTextureFormat_Rgba8Unorm;
      break;
    case dom::GPUTextureFormat::Rgba8unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Rgba8UnormSrgb;
      break;
    case dom::GPUTextureFormat::Rgba8snorm:
      result.tag = ffi::WGPUTextureFormat_Rgba8Snorm;
      break;
    case dom::GPUTextureFormat::Rgba8uint:
      result.tag = ffi::WGPUTextureFormat_Rgba8Uint;
      break;
    case dom::GPUTextureFormat::Rgba8sint:
      result.tag = ffi::WGPUTextureFormat_Rgba8Sint;
      break;
    case dom::GPUTextureFormat::Bgra8unorm:
      result.tag = ffi::WGPUTextureFormat_Bgra8Unorm;
      break;
    case dom::GPUTextureFormat::Bgra8unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Bgra8UnormSrgb;
      break;
    case dom::GPUTextureFormat::Rgb9e5ufloat:
      result.tag = ffi::WGPUTextureFormat_Rgb9e5Ufloat;
      break;
    case dom::GPUTextureFormat::Rgb10a2unorm:
      result.tag = ffi::WGPUTextureFormat_Rgb10a2Unorm;
      break;
    case dom::GPUTextureFormat::Rg11b10ufloat:
      result.tag = ffi::WGPUTextureFormat_Rg11b10Ufloat;
      break;
    case dom::GPUTextureFormat::Rg32uint:
      result.tag = ffi::WGPUTextureFormat_Rg32Uint;
      break;
    case dom::GPUTextureFormat::Rg32sint:
      result.tag = ffi::WGPUTextureFormat_Rg32Sint;
      break;
    case dom::GPUTextureFormat::Rg32float:
      result.tag = ffi::WGPUTextureFormat_Rg32Float;
      break;
    case dom::GPUTextureFormat::Rgba16uint:
      result.tag = ffi::WGPUTextureFormat_Rgba16Uint;
      break;
    case dom::GPUTextureFormat::Rgba16sint:
      result.tag = ffi::WGPUTextureFormat_Rgba16Sint;
      break;
    case dom::GPUTextureFormat::Rgba16float:
      result.tag = ffi::WGPUTextureFormat_Rgba16Float;
      break;
    case dom::GPUTextureFormat::Rgba32uint:
      result.tag = ffi::WGPUTextureFormat_Rgba32Uint;
      break;
    case dom::GPUTextureFormat::Rgba32sint:
      result.tag = ffi::WGPUTextureFormat_Rgba32Sint;
      break;
    case dom::GPUTextureFormat::Rgba32float:
      result.tag = ffi::WGPUTextureFormat_Rgba32Float;
      break;
    case dom::GPUTextureFormat::Depth32float:
      result.tag = ffi::WGPUTextureFormat_Depth32Float;
      break;
    case dom::GPUTextureFormat::Bc1_rgba_unorm:
      result.tag = ffi::WGPUTextureFormat_Bc1RgbaUnorm;
      break;
    case dom::GPUTextureFormat::Bc1_rgba_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Bc1RgbaUnormSrgb;
      break;
    case dom::GPUTextureFormat::Bc4_r_unorm:
      result.tag = ffi::WGPUTextureFormat_Bc4RUnorm;
      break;
    case dom::GPUTextureFormat::Bc4_r_snorm:
      result.tag = ffi::WGPUTextureFormat_Bc4RSnorm;
      break;
    case dom::GPUTextureFormat::Bc2_rgba_unorm:
      result.tag = ffi::WGPUTextureFormat_Bc2RgbaUnorm;
      break;
    case dom::GPUTextureFormat::Bc2_rgba_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Bc2RgbaUnormSrgb;
      break;
    case dom::GPUTextureFormat::Bc3_rgba_unorm:
      result.tag = ffi::WGPUTextureFormat_Bc3RgbaUnorm;
      break;
    case dom::GPUTextureFormat::Bc3_rgba_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Bc3RgbaUnormSrgb;
      break;
    case dom::GPUTextureFormat::Bc5_rg_unorm:
      result.tag = ffi::WGPUTextureFormat_Bc5RgUnorm;
      break;
    case dom::GPUTextureFormat::Bc5_rg_snorm:
      result.tag = ffi::WGPUTextureFormat_Bc5RgSnorm;
      break;
    case dom::GPUTextureFormat::Bc6h_rgb_ufloat:
      result.tag = ffi::WGPUTextureFormat_Bc6hRgbUfloat;
      break;
    case dom::GPUTextureFormat::Bc6h_rgb_float:
      result.tag = ffi::WGPUTextureFormat_Bc6hRgbFloat;
      break;
    case dom::GPUTextureFormat::Bc7_rgba_unorm:
      result.tag = ffi::WGPUTextureFormat_Bc7RgbaUnorm;
      break;
    case dom::GPUTextureFormat::Bc7_rgba_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Bc7RgbaUnormSrgb;
      break;
    case dom::GPUTextureFormat::Stencil8:
      result.tag = ffi::WGPUTextureFormat_Stencil8;
      break;
    case dom::GPUTextureFormat::Depth16unorm:
      result.tag = ffi::WGPUTextureFormat_Depth16Unorm;
      break;
    case dom::GPUTextureFormat::Depth24plus:
      result.tag = ffi::WGPUTextureFormat_Depth24Plus;
      break;
    case dom::GPUTextureFormat::Depth24plus_stencil8:
      result.tag = ffi::WGPUTextureFormat_Depth24PlusStencil8;
      break;
    case dom::GPUTextureFormat::Depth32float_stencil8:
      result.tag = ffi::WGPUTextureFormat_Depth32FloatStencil8;
      break;
    case dom::GPUTextureFormat::Astc_4x4_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B4x4;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_4x4_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B4x4;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_5x4_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B5x4;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_5x4_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B5x4;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_5x5_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B5x5;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_5x5_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B5x5;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_6x5_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B6x5;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_6x5_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B6x5;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_6x6_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B6x6;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_6x6_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B6x6;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_8x5_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B8x5;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_8x5_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B8x5;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_8x6_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B8x6;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_8x6_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B8x6;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_8x8_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B8x8;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_8x8_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B8x8;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_10x5_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x5;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_10x5_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x5;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_10x6_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x6;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_10x6_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x6;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_10x8_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x8;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_10x8_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x8;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_10x10_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x10;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_10x10_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B10x10;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_12x10_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B12x10;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_12x10_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B12x10;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Astc_12x12_unorm:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B12x12;
      result.astc.channel = ffi::WGPUAstcChannel_Unorm;
      break;
    case dom::GPUTextureFormat::Astc_12x12_unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Astc;
      result.astc.block = ffi::WGPUAstcBlock_B12x12;
      result.astc.channel = ffi::WGPUAstcChannel_UnormSrgb;
      break;
    case dom::GPUTextureFormat::Etc2_rgb8unorm:
      result.tag = ffi::WGPUTextureFormat_Etc2Rgb8Unorm;
      break;
    case dom::GPUTextureFormat::Etc2_rgb8unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Etc2Rgb8UnormSrgb;
      break;
    case dom::GPUTextureFormat::Etc2_rgb8a1unorm:
      result.tag = ffi::WGPUTextureFormat_Etc2Rgb8A1Unorm;
      break;
    case dom::GPUTextureFormat::Etc2_rgb8a1unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Etc2Rgb8A1UnormSrgb;
      break;
    case dom::GPUTextureFormat::Etc2_rgba8unorm:
      result.tag = ffi::WGPUTextureFormat_Etc2Rgba8Unorm;
      break;
    case dom::GPUTextureFormat::Etc2_rgba8unorm_srgb:
      result.tag = ffi::WGPUTextureFormat_Etc2Rgba8UnormSrgb;
      break;
    case dom::GPUTextureFormat::Eac_r11unorm:
      result.tag = ffi::WGPUTextureFormat_EacR11Unorm;
      break;
    case dom::GPUTextureFormat::Eac_r11snorm:
      result.tag = ffi::WGPUTextureFormat_EacR11Snorm;
      break;
    case dom::GPUTextureFormat::Eac_rg11unorm:
      result.tag = ffi::WGPUTextureFormat_EacRg11Unorm;
      break;
    case dom::GPUTextureFormat::Eac_rg11snorm:
      result.tag = ffi::WGPUTextureFormat_EacRg11Snorm;
      break;
    case dom::GPUTextureFormat::Rgb10a2uint:
      result.tag = ffi::WGPUTextureFormat_Rgb10a2Uint;
      break;
  }

  // Clang will check for us that the switch above is exhaustive,
  // but not if we add a 'default' case. So, check this here.
  MOZ_RELEASE_ASSERT(result.tag != ffi::WGPUTextureFormat_Sentinel,
                     "unexpected texture format enum");

  return result;
}

ffi::WGPUTextureAspect ConvertTextureAspect(
    const dom::GPUTextureAspect& aAspect) {
  ffi::WGPUTextureAspect result = ffi::WGPUTextureAspect_Sentinel;
  switch (aAspect) {
    case dom::GPUTextureAspect::All:
      result = ffi::WGPUTextureAspect_All;
      break;
    case dom::GPUTextureAspect::Depth_only:
      result = ffi::WGPUTextureAspect_DepthOnly;
      break;
    case dom::GPUTextureAspect::Stencil_only:
      result = ffi::WGPUTextureAspect_StencilOnly;
      break;
  }

  // Clang will check for us that the switch above is exhaustive,
  // but not if we add a 'default' case. So, check this here.
  MOZ_RELEASE_ASSERT(result != ffi::WGPUTextureAspect_Sentinel,
                     "unexpected texture aspect enum");

  return result;
}

ffi::WGPUVertexFormat ConvertVertexFormat(const dom::GPUVertexFormat& aFormat) {
  ffi::WGPUVertexFormat result = ffi::WGPUVertexFormat_Sentinel;
  switch (aFormat) {
    case dom::GPUVertexFormat::Uint8:
      result = ffi::WGPUVertexFormat_Uint8;
      break;
    case dom::GPUVertexFormat::Uint8x2:
      result = ffi::WGPUVertexFormat_Uint8x2;
      break;
    case dom::GPUVertexFormat::Uint8x4:
      result = ffi::WGPUVertexFormat_Uint8x4;
      break;
    case dom::GPUVertexFormat::Sint8:
      result = ffi::WGPUVertexFormat_Sint8;
      break;
    case dom::GPUVertexFormat::Sint8x2:
      result = ffi::WGPUVertexFormat_Sint8x2;
      break;
    case dom::GPUVertexFormat::Sint8x4:
      result = ffi::WGPUVertexFormat_Sint8x4;
      break;
    case dom::GPUVertexFormat::Unorm8:
      result = ffi::WGPUVertexFormat_Unorm8;
      break;
    case dom::GPUVertexFormat::Unorm8x2:
      result = ffi::WGPUVertexFormat_Unorm8x2;
      break;
    case dom::GPUVertexFormat::Unorm8x4:
      result = ffi::WGPUVertexFormat_Unorm8x4;
      break;
    case dom::GPUVertexFormat::Snorm8:
      result = ffi::WGPUVertexFormat_Snorm8;
      break;
    case dom::GPUVertexFormat::Snorm8x2:
      result = ffi::WGPUVertexFormat_Snorm8x2;
      break;
    case dom::GPUVertexFormat::Snorm8x4:
      result = ffi::WGPUVertexFormat_Snorm8x4;
      break;
    case dom::GPUVertexFormat::Uint16:
      result = ffi::WGPUVertexFormat_Uint16;
      break;
    case dom::GPUVertexFormat::Uint16x2:
      result = ffi::WGPUVertexFormat_Uint16x2;
      break;
    case dom::GPUVertexFormat::Uint16x4:
      result = ffi::WGPUVertexFormat_Uint16x4;
      break;
    case dom::GPUVertexFormat::Sint16:
      result = ffi::WGPUVertexFormat_Sint16;
      break;
    case dom::GPUVertexFormat::Sint16x2:
      result = ffi::WGPUVertexFormat_Sint16x2;
      break;
    case dom::GPUVertexFormat::Sint16x4:
      result = ffi::WGPUVertexFormat_Sint16x4;
      break;
    case dom::GPUVertexFormat::Unorm16:
      result = ffi::WGPUVertexFormat_Unorm16;
      break;
    case dom::GPUVertexFormat::Unorm16x2:
      result = ffi::WGPUVertexFormat_Unorm16x2;
      break;
    case dom::GPUVertexFormat::Unorm16x4:
      result = ffi::WGPUVertexFormat_Unorm16x4;
      break;
    case dom::GPUVertexFormat::Snorm16:
      result = ffi::WGPUVertexFormat_Snorm16;
      break;
    case dom::GPUVertexFormat::Snorm16x2:
      result = ffi::WGPUVertexFormat_Snorm16x2;
      break;
    case dom::GPUVertexFormat::Snorm16x4:
      result = ffi::WGPUVertexFormat_Snorm16x4;
      break;
    case dom::GPUVertexFormat::Float16:
      result = ffi::WGPUVertexFormat_Float16;
      break;
    case dom::GPUVertexFormat::Float16x2:
      result = ffi::WGPUVertexFormat_Float16x2;
      break;
    case dom::GPUVertexFormat::Float16x4:
      result = ffi::WGPUVertexFormat_Float16x4;
      break;
    case dom::GPUVertexFormat::Float32:
      result = ffi::WGPUVertexFormat_Float32;
      break;
    case dom::GPUVertexFormat::Float32x2:
      result = ffi::WGPUVertexFormat_Float32x2;
      break;
    case dom::GPUVertexFormat::Float32x3:
      result = ffi::WGPUVertexFormat_Float32x3;
      break;
    case dom::GPUVertexFormat::Float32x4:
      result = ffi::WGPUVertexFormat_Float32x4;
      break;
    case dom::GPUVertexFormat::Uint32:
      result = ffi::WGPUVertexFormat_Uint32;
      break;
    case dom::GPUVertexFormat::Uint32x2:
      result = ffi::WGPUVertexFormat_Uint32x2;
      break;
    case dom::GPUVertexFormat::Uint32x3:
      result = ffi::WGPUVertexFormat_Uint32x3;
      break;
    case dom::GPUVertexFormat::Uint32x4:
      result = ffi::WGPUVertexFormat_Uint32x4;
      break;
    case dom::GPUVertexFormat::Sint32:
      result = ffi::WGPUVertexFormat_Sint32;
      break;
    case dom::GPUVertexFormat::Sint32x2:
      result = ffi::WGPUVertexFormat_Sint32x2;
      break;
    case dom::GPUVertexFormat::Sint32x3:
      result = ffi::WGPUVertexFormat_Sint32x3;
      break;
    case dom::GPUVertexFormat::Sint32x4:
      result = ffi::WGPUVertexFormat_Sint32x4;
      break;
    case dom::GPUVertexFormat::Unorm10_10_10_2:
      result = ffi::WGPUVertexFormat_Unorm10_10_10_2;
      break;
    case dom::GPUVertexFormat::Unorm8x4_bgra:
      result = ffi::WGPUVertexFormat_Unorm8x4Bgra;
      break;
  }

  // Clang will check for us that the switch above is exhaustive,
  // but not if we add a 'default' case. So, check this here.
  MOZ_RELEASE_ASSERT(result != ffi::WGPUVertexFormat_Sentinel,
                     "unexpected texture format enum");

  return result;
}

ffi::WGPUMultisampleState ConvertMultisampleState(
    const dom::GPUMultisampleState& aDesc) {
  ffi::WGPUMultisampleState desc = {};
  desc.count = aDesc.mCount;
  desc.mask = aDesc.mMask;
  desc.alpha_to_coverage_enabled = aDesc.mAlphaToCoverageEnabled;
  return desc;
}

ffi::WGPUBlendComponent ConvertBlendComponent(
    const dom::GPUBlendComponent& aDesc) {
  ffi::WGPUBlendComponent desc = {};
  desc.src_factor = ffi::WGPUBlendFactor(aDesc.mSrcFactor);
  desc.dst_factor = ffi::WGPUBlendFactor(aDesc.mDstFactor);
  desc.operation = ffi::WGPUBlendOperation(aDesc.mOperation);
  return desc;
}

ffi::WGPUStencilFaceState ConvertStencilFaceState(
    const dom::GPUStencilFaceState& aDesc) {
  ffi::WGPUStencilFaceState desc = {};
  desc.compare = ConvertCompareFunction(aDesc.mCompare);
  desc.fail_op = ffi::WGPUStencilOperation(aDesc.mFailOp);
  desc.depth_fail_op = ffi::WGPUStencilOperation(aDesc.mDepthFailOp);
  desc.pass_op = ffi::WGPUStencilOperation(aDesc.mPassOp);
  return desc;
}

ffi::WGPUDepthStencilState ConvertDepthStencilState(
    const dom::GPUDepthStencilState& aDesc) {
  ffi::WGPUDepthStencilState desc = {};
  desc.format = ConvertTextureFormat(aDesc.mFormat);
  desc.depth_write_enabled = aDesc.mDepthWriteEnabled;
  desc.depth_compare = ConvertCompareFunction(aDesc.mDepthCompare);
  desc.stencil.front = ConvertStencilFaceState(aDesc.mStencilFront);
  desc.stencil.back = ConvertStencilFaceState(aDesc.mStencilBack);
  desc.stencil.read_mask = aDesc.mStencilReadMask;
  desc.stencil.write_mask = aDesc.mStencilWriteMask;
  desc.bias.constant = aDesc.mDepthBias;
  desc.bias.slope_scale = aDesc.mDepthBiasSlopeScale;
  desc.bias.clamp = aDesc.mDepthBiasClamp;
  return desc;
}

// Extract a list of dynamic offsets from a larger JS-supplied buffer.
// Used by implementions of the `setBindGroup` method of the spec's
// `GPUBindingCommandsMixin`.
//
// If the given start/length are out of bounds, sets a
// `RangeError` in `aRv` and returns `Nothing`.
mozilla::Maybe<mozilla::Buffer<uint32_t>> GetDynamicOffsetsFromArray(
    const dom::Uint32Array& aDynamicOffsetsData,
    uint64_t aDynamicOffsetsDataStart, uint64_t aDynamicOffsetsDataLength,
    ErrorResult& aRv) {
  auto dynamicOffsets =
      aDynamicOffsetsData.CreateFromData<mozilla::Buffer<uint32_t>>(
          [&](const size_t& aLength)
              -> mozilla::Maybe<std::pair<uint64_t, uint64_t>> {
            auto checkedLength =
                CheckedInt<uint64_t>(aDynamicOffsetsDataStart) +
                aDynamicOffsetsDataLength;
            if (!checkedLength.isValid() || checkedLength.value() > aLength) {
              return mozilla::Nothing();
            } else {
              return mozilla::Some(std::make_pair(aDynamicOffsetsDataStart,
                                                  aDynamicOffsetsDataLength));
            }
          });

  if (dynamicOffsets.isNothing()) {
    aRv.ThrowRangeError<dom::MSG_VALUE_OUT_OF_RANGE>(
        "dynamicOffsetsDataStart/Length");
  }

  return dynamicOffsets;
}

}  // namespace mozilla::webgpu
