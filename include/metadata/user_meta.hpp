#pragma once
#include <optional>

// gstnvdsinfer.h has no include guards — wrap with a manual idempotency guard.
#ifndef DEEPSTREAM_HPP_GSTNVDSINFER_INCLUDED
#define DEEPSTREAM_HPP_GSTNVDSINFER_INCLUDED
#include <gstnvdsinfer.h>
#endif
#include <nvdsmeta.h>

#include <metadata/tensor_meta.hpp>

namespace ds {

class UserMetaView {
public:
  explicit UserMetaView(NvDsUserMeta* meta) : meta_(meta) {}

  [[nodiscard]] NvDsMetaType meta_type() const { return meta_->base_meta.meta_type; }

  // Returns a TensorMetaView if this user-meta holds nvinfer tensor output.
  // Returns nullopt for any other meta type.
  [[nodiscard]] std::optional<TensorMetaView> as_tensor_meta() const {
    if(meta_->base_meta.meta_type != NVDSINFER_TENSOR_OUTPUT_META) {
      return std::nullopt;
    }
    auto* tensor = static_cast<NvDsInferTensorMeta*>(meta_->user_meta_data);
    if(tensor == nullptr) {
      return std::nullopt;
    }
    return TensorMetaView{tensor};
  }

  [[nodiscard]] void* raw_data() const { return meta_->user_meta_data; }

  [[nodiscard]] NvDsUserMeta* get() const { return meta_; }

private:
  NvDsUserMeta* meta_;
};

}    // namespace ds
