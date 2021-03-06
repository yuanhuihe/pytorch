#pragma once
#include "torch/csrc/jit/fuser/config.h"
#if USE_CUDA_FUSER

#include "ATen/ATen.h"
#include "torch/csrc/WindowsTorchApiMacro.h"
#include "torch/csrc/jit/fuser/fused_kernel.h"

#include "nvrtc.h"
#include "cuda.h"
#include "cuda_runtime.h"

#include <cstdint>
#include <vector>
#include <string>

namespace torch { namespace jit { namespace fuser { namespace cuda {

// A class holding metadata for an actual CUDA function.
// Note: CUDA functions are per device.
struct TORCH_API FusedKernelCUDA : public ::torch::jit::fuser::FusedKernel {
  FusedKernelCUDA(
    const int16_t _device
  , const std::string& _name
  , const std::string& _code
  , const std::vector<TensorDesc> _input_desc
  , const std::vector<TensorDesc> _output_desc
  , const std::vector<PartitionDesc> _chunk_desc
  , const std::vector<PartitionDesc> _concat_desc
  , const bool _has_random);

  virtual ~FusedKernelCUDA() override {
    cuModuleUnload(module_);
  }

  virtual void launch_raw(
    const uint32_t numel
  , std::vector<void*>& arguments) const override;

  virtual at::Backend backend() const override {
    return at::Backend::CUDA;
  }

private:
  static constexpr auto kBlockSize = 128;

  // Note: per device to store device properties and compute launch heuristics
  //  Acquiring these values at launch time would be too slow
  int16_t device_;
  int maxBlocks_;
  cudaDeviceProp* prop_;
  std::vector<char> ptx_;
  CUmodule module_;
  CUfunction function_;
};

} // namespace cuda
} // namespace fuser
} // namespace jit
} // namespace torch

#endif // USE_CUDA_FUSER
