#pragma once
#include <cstdint>
#include <span>
#include <string_view>

// gstnvdsinfer.h has no include guards — wrap with a manual idempotency guard.
#ifndef DEEPSTREAM_HPP_GSTNVDSINFER_INCLUDED
#  define DEEPSTREAM_HPP_GSTNVDSINFER_INCLUDED
#  include <gstnvdsinfer.h>
#endif

namespace ds {

class TensorLayerView {
public:
  explicit TensorLayerView(const NvDsInferLayerInfo* info) : info_(info) {}

  [[nodiscard]] std::string_view name() const {
    return info_->layerName ? info_->layerName : "";
  }

  [[nodiscard]] NvDsInferDataType data_type() const {
    return info_->dataType;
  }

  [[nodiscard]] std::uint32_t num_dims() const {
    return info_->inferDims.numDims;
  }

  [[nodiscard]] std::uint32_t num_elements() const {
    return info_->inferDims.numElements;
  }

  // Raw host-side output buffer (cast to the appropriate type based on data_type()).
  [[nodiscard]] void* buffer() const {
    return info_->buffer;
  }

  // Shape as a span over the dimension array (up to num_dims() entries are valid).
  [[nodiscard]] std::span<const std::uint32_t> shape() const {
    return {info_->inferDims.d, info_->inferDims.numDims};
  }

  [[nodiscard]] const NvDsInferLayerInfo* get() const {
    return info_;
  }

private:
  const NvDsInferLayerInfo* info_;
};

class TensorMetaView {
public:
  explicit TensorMetaView(NvDsInferTensorMeta* meta) : meta_(meta) {}

  [[nodiscard]] std::uint32_t unique_id() const {
    return meta_->unique_id;
  }
  [[nodiscard]] std::uint32_t num_output_layers() const {
    return meta_->num_output_layers;
  }
  [[nodiscard]] std::int32_t gpu_id() const {
    return meta_->gpu_id;
  }

  [[nodiscard]] TensorLayerView output_layer(std::uint32_t index) const {
    return TensorLayerView{&meta_->output_layers_info[index]};
  }

  // Range over output layers (index-based, not GList-based).
  struct LayerRange {
    const NvDsInferLayerInfo* data;
    std::uint32_t count;

    struct Iterator {
      using difference_type = std::ptrdiff_t;
      using value_type = TensorLayerView;
      using pointer = void;
      using reference = TensorLayerView;
      using iterator_category = std::random_access_iterator_tag;

      const NvDsInferLayerInfo* ptr;

      TensorLayerView operator*() const {
        return TensorLayerView{ptr};
      }
      Iterator& operator++() {
        ++ptr;
        return *this;
      }
      Iterator operator++(int) {
        auto tmp = *this;
        ++ptr;
        return tmp;
      }
      bool operator==(const Iterator& o) const {
        return ptr == o.ptr;
      }
      bool operator!=(const Iterator& o) const {
        return ptr != o.ptr;
      }
    };

    [[nodiscard]] Iterator begin() const {
      return {data};
    }
    [[nodiscard]] Iterator end() const {
      return {data + count};
    }
    [[nodiscard]] bool empty() const {
      return count == 0;
    }
    [[nodiscard]] std::uint32_t size() const {
      return count;
    }
  };

  [[nodiscard]] LayerRange output_layers() const {
    return {meta_->output_layers_info, meta_->num_output_layers};
  }

  [[nodiscard]] NvDsInferTensorMeta* get() const {
    return meta_;
  }

private:
  NvDsInferTensorMeta* meta_;
};

}    // namespace ds
