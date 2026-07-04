#pragma once
#include <cstdint>
#include <string_view>

#include <metadata/classifier_meta.hpp>
#include <metadata/meta_list_view.hpp>
#include <nvdsmeta.h>

namespace ds {

struct BoundingBox {
  float left;
  float top;
  float width;
  float height;

  [[nodiscard]] float right() const {
    return left + width;
  }
  [[nodiscard]] float bottom() const {
    return top + height;
  }
  [[nodiscard]] float area() const {
    return width * height;
  }
};

class ObjectMetaView {
public:
  explicit ObjectMetaView(NvDsObjectMeta* meta) : meta_(meta) {}

  [[nodiscard]] std::int32_t class_id() const {
    return meta_->class_id;
  }
  [[nodiscard]] std::uint64_t object_id() const {
    return meta_->object_id;
  }
  [[nodiscard]] float confidence() const {
    return meta_->confidence;
  }
  [[nodiscard]] float tracker_confidence() const {
    return meta_->tracker_confidence;
  }
  [[nodiscard]] std::string_view label() const {
    return meta_->obj_label;
  }
  [[nodiscard]] std::int32_t unique_component_id() const {
    return meta_->unique_component_id;
  }

  [[nodiscard]] BoundingBox rect() const {
    const auto& r = meta_->rect_params;
    return {r.left, r.top, r.width, r.height};
  }

  [[nodiscard]] MetaListView<ClassifierMetaView, NvDsClassifierMeta> classifiers() const {
    return MetaListView<ClassifierMetaView, NvDsClassifierMeta>{meta_->classifier_meta_list};
  }

  [[nodiscard]] NvDsObjectMeta* get() const {
    return meta_;
  }

private:
  NvDsObjectMeta* meta_;
};

}    // namespace ds
