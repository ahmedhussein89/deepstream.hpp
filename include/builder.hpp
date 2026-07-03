#pragma once
#include <string>
#include <unordered_set>
#include <vector>

#include <fmt/format.h>
#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <core/concepts.hpp>
#include <gstreamer_raii.hpp>
#include <utils/debug.hpp>
#include <utils/error.hpp>

namespace ds {

namespace detail {

inline std::string element_name(GstElement* elem) {
  gchar* raw = gst_element_get_name(elem);
  std::string name = raw ? raw : "";
  g_free(raw);
  return name;
}

inline std::string factory_name(GstElement* elem) {
  GstElementFactory* factory = gst_element_get_factory(elem);
  if(factory == nullptr) {
    return detail::element_name(elem);
  }
  const gchar* name = GST_OBJECT_NAME(factory);
  return name ? name : "unknown";
}

// Returns false only when both elements have static pads and none can intersect.
// Returns true when uncertain (dynamic pads, ANY caps, missing factory info).
inline bool can_link_statically(GstElement* src, GstElement* sink) {
  GstElementFactory* sf = gst_element_get_factory(src);
  GstElementFactory* kf = gst_element_get_factory(sink);
  if(sf == nullptr || kf == nullptr) {
    return true;
  }

  const GList* src_tmpls = gst_element_factory_get_static_pad_templates(sf);
  const GList* sink_tmpls = gst_element_factory_get_static_pad_templates(kf);

  bool has_src_pad = false;
  bool has_sink_pad = false;

  for(const GList* s = src_tmpls; s != nullptr; s = s->next) {
    const auto* st = static_cast<const GstStaticPadTemplate*>(s->data);
    if(st->direction != GST_PAD_SRC) {
      continue;
    }
    has_src_pad = true;

    GstCaps* s_caps = gst_static_pad_template_get_caps(const_cast<GstStaticPadTemplate*>(st));
    if(gst_caps_is_any(s_caps)) {
      gst_caps_unref(s_caps);
      return true;
    }

    for(const GList* k = sink_tmpls; k != nullptr; k = k->next) {
      const auto* kt = static_cast<const GstStaticPadTemplate*>(k->data);
      if(kt->direction != GST_PAD_SINK) {
        continue;
      }
      has_sink_pad = true;

      GstCaps* k_caps = gst_static_pad_template_get_caps(const_cast<GstStaticPadTemplate*>(kt));
      const bool compat = gst_caps_is_any(k_caps) || (gst_caps_can_intersect(s_caps, k_caps) != FALSE);
      gst_caps_unref(k_caps);
      if(compat) {
        gst_caps_unref(s_caps);
        return true;
      }
    }
    gst_caps_unref(s_caps);
  }

  // If either side has no static pads (e.g. dynamic elements like decodebin),
  // we cannot determine compatibility statically — assume compatible.
  if(!has_src_pad || !has_sink_pad) {
    return true;
  }
  return false;
}

}    // namespace detail

// Fluent pipeline builder for the ds:: element layer.
//
// Usage:
//   auto pipe = ds::Builder{}
//       .add(ds::FileSource::create().value().location("/tmp/v.mp4"))
//       .add(ds::FileSink::create().value().location("/tmp/out.mp4"))
//       .build();
class Builder {
public:
  Builder() = default;

  ~Builder() {
    for(auto* e : elements_) {
      if(e != nullptr) {
        gst_object_unref(e);
      }
    }
  }

  Builder(Builder&&) = default;
  Builder& operator=(Builder&&) = default;
  Builder(const Builder&) = delete;
  Builder& operator=(const Builder&) = delete;

  template <DsElement T>
  Builder& add(T&& element) {
    const std::string name = detail::element_name(element.get());
    if(!name.empty() && !names_.insert(name).second && first_duplicate_.empty()) {
      first_duplicate_ = name;
    }
    factory_names_.push_back(detail::factory_name(element.get()));
    elements_.push_back(element.release());
    return *this;
  }

  [[nodiscard]] nonstd::expected<gst::raii::Pipeline, PipelineError> build() {
    // Mandatory: at least one element
    if(elements_.empty()) {
      const auto msg = std::string("Pipeline must contain at least one element");
      DebugLayer::instance().log(DebugLevel::Error, ErrorKind::NoElements, msg, __FILE__, __LINE__);
      return nonstd::make_unexpected(PipelineError{ErrorKind::NoElements, msg});
    }

    // Unique element name enforcement
    if(!first_duplicate_.empty()) {
      const auto msg = fmt::format("Duplicate element name: '{}'", first_duplicate_);
      DebugLayer::instance().log(DebugLevel::Error, ErrorKind::DuplicateName, msg, __FILE__, __LINE__);
      return nonstd::make_unexpected(PipelineError{ErrorKind::DuplicateName, msg});
    }

    // Caps compatibility validation (static pad templates)
    for(std::size_t i = 0; i + 1 < elements_.size(); ++i) {
      if(!detail::can_link_statically(elements_[i], elements_[i + 1])) {
        const auto msg = fmt::format("Caps incompatible: '{}' cannot link to '{}'", factory_names_[i], factory_names_[i + 1]);
        DebugLayer::instance().log(DebugLevel::Error, ErrorKind::IncompatibleCaps, msg, __FILE__, __LINE__);
        return nonstd::make_unexpected(PipelineError{ErrorKind::IncompatibleCaps, msg});
      }
    }

    GstElement* raw_pipeline = gst_pipeline_new(nullptr);
    if(raw_pipeline == nullptr) {
      const auto msg = std::string("Failed to create GstPipeline");
      DebugLayer::instance().log(DebugLevel::Error, ErrorKind::PipelineCreation, msg, __FILE__, __LINE__);
      return nonstd::make_unexpected(PipelineError{ErrorKind::PipelineCreation, msg});
    }

    // Add elements; gst_bin_add sinks the floating reference for each element.
    std::vector<GstElement*> added;
    added.reserve(elements_.size());
    for(std::size_t i = 0; i < elements_.size(); ++i) {
      if(gst_bin_add(GST_BIN(raw_pipeline), elements_[i]) == FALSE) {
        for(std::size_t j = i; j < elements_.size(); ++j) {
          gst_object_unref(elements_[j]);
        }
        elements_.clear();
        gst_object_unref(raw_pipeline);
        const auto msg = fmt::format("Failed to add '{}' to pipeline (name collision?)", factory_names_[i]);
        DebugLayer::instance().log(DebugLevel::Error, ErrorKind::BinAdd, msg, __FILE__, __LINE__);
        return nonstd::make_unexpected(PipelineError{ErrorKind::BinAdd, msg});
      }
      added.push_back(elements_[i]);
    }
    elements_.clear();    // pipeline now owns every element

    // Link sequentially
    for(std::size_t i = 0; i + 1 < added.size(); ++i) {
      if(gst_element_link(added[i], added[i + 1]) == FALSE) {
        gst_object_unref(raw_pipeline);
        const auto msg = fmt::format("Failed to link '{}' to '{}'", factory_names_[i], factory_names_[i + 1]);
        DebugLayer::instance().log(DebugLevel::Error, ErrorKind::ElementLink, msg, __FILE__, __LINE__);
        return nonstd::make_unexpected(PipelineError{ErrorKind::ElementLink, msg});
      }
    }

    return gst::raii::Pipeline{raw_pipeline};
  }

private:
  std::vector<GstElement*>       elements_;          // raw floating-ref pointers owned by Builder
  std::vector<std::string>       factory_names_;     // parallel: factory name for each element
  std::unordered_set<std::string> names_;            // for duplicate detection
  std::string                    first_duplicate_;
};

}    // namespace ds
