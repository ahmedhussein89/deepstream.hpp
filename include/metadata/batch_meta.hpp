#pragma once
#include <optional>

#include <gst/gst.h>
#include <gstnvdsmeta.h>

#include <metadata/frame_meta.hpp>
#include <metadata/meta_list_view.hpp>
#include <nvdsmeta.h>

namespace ds {

class BatchMetaView {
public:
  explicit BatchMetaView(NvDsBatchMeta* meta) : meta_(meta) {}

  // Extracts the NvDsBatchMeta attached to a GstBuffer by nvstreammux.
  // Returns nullopt when no batch meta is present (e.g. non-DeepStream pipelines).
  [[nodiscard]] static std::optional<BatchMetaView> from_buffer(GstBuffer* buffer) {
    NvDsBatchMeta* meta = gst_buffer_get_nvds_batch_meta(buffer);
    if(meta == nullptr) {
      return std::nullopt;
    }
    return BatchMetaView{meta};
  }

  [[nodiscard]] MetaListView<FrameMetaView, NvDsFrameMeta> frames() const {
    return MetaListView<FrameMetaView, NvDsFrameMeta>{meta_->frame_meta_list};
  }

  [[nodiscard]] NvDsBatchMeta* get() const {
    return meta_;
  }

private:
  NvDsBatchMeta* meta_;
};

}    // namespace ds
