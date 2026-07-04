#pragma once
#include <cstdint>
#include <string_view>

#include <metadata/meta_list_view.hpp>
#include <nvdsmeta.h>

namespace ds {

class LabelInfoView {
public:
  explicit LabelInfoView(NvDsLabelInfo* info) : info_(info) {}

  [[nodiscard]] std::string_view label() const {
    return info_->result_label;
  }
  [[nodiscard]] std::uint32_t class_id() const {
    return info_->result_class_id;
  }
  [[nodiscard]] float probability() const {
    return info_->result_prob;
  }
  [[nodiscard]] std::uint32_t label_id() const {
    return info_->label_id;
  }

  [[nodiscard]] NvDsLabelInfo* get() const {
    return info_;
  }

private:
  NvDsLabelInfo* info_;
};

class ClassifierMetaView {
public:
  explicit ClassifierMetaView(NvDsClassifierMeta* meta) : meta_(meta) {}

  [[nodiscard]] std::int32_t unique_component_id() const {
    return meta_->unique_component_id;
  }
  [[nodiscard]] std::string_view classifier_type() const {
    return meta_->classifier_type ? meta_->classifier_type : "";
  }

  [[nodiscard]] MetaListView<LabelInfoView, NvDsLabelInfo> labels() const {
    return MetaListView<LabelInfoView, NvDsLabelInfo>{meta_->label_info_list};
  }

  [[nodiscard]] NvDsClassifierMeta* get() const {
    return meta_;
  }

private:
  NvDsClassifierMeta* meta_;
};

}    // namespace ds
