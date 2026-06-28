#pragma once
#include <cstdint>

#include <gst/gst.h>
#include <nvdsmeta.h>

#include <metadata/meta_list_view.hpp>
#include <metadata/object_meta.hpp>

namespace ds {

class FrameMetaView {
public:
  explicit FrameMetaView(NvDsFrameMeta* meta) : meta_(meta) {}

  [[nodiscard]] std::uint32_t batch_id() const { return meta_->batch_id; }
  [[nodiscard]] std::uint32_t pad_index() const { return meta_->pad_index; }
  [[nodiscard]] std::int32_t frame_num() const { return meta_->frame_num; }
  [[nodiscard]] GstClockTime buf_pts() const { return meta_->buf_pts; }
  [[nodiscard]] std::uint32_t num_objects() const { return meta_->num_obj_meta; }
  [[nodiscard]] std::uint32_t source_id() const { return meta_->source_id; }

  [[nodiscard]] MetaListView<ObjectMetaView, NvDsObjectMeta> objects() const {
    return MetaListView<ObjectMetaView, NvDsObjectMeta>{meta_->obj_meta_list};
  }

  [[nodiscard]] NvDsFrameMeta* get() const { return meta_; }

private:
  NvDsFrameMeta* meta_;
};

}    // namespace ds
