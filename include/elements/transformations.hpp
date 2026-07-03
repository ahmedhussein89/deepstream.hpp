#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <elements/detail.hpp>
#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

struct StreamMuxConfig {
  std::uint32_t batch_size{1};
  std::uint32_t width{1920};
  std::uint32_t height{1080};
  std::int32_t  batched_push_timeout{-1};
  bool          live_source{false};
  bool          sync_inputs{false};
};

class StreamMux {
public:
  [[nodiscard]] static nonstd::expected<StreamMux, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvstreammux", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvstreammux' element"});
    }
    return StreamMux{gst::raii::Element{raw}};
  }

  StreamMux& batch_size(std::uint32_t size) {
    detail::set_property(mElement.get(), "batch-size", static_cast<guint>(size));
    return *this;
  }

  StreamMux& width(std::uint32_t val) {
    detail::set_property(mElement.get(), "width", static_cast<guint>(val));
    return *this;
  }

  StreamMux& height(std::uint32_t val) {
    detail::set_property(mElement.get(), "height", static_cast<guint>(val));
    return *this;
  }

  StreamMux& batched_push_timeout(std::int32_t timeout_us) {
    detail::set_property(mElement.get(), "batched-push-timeout", static_cast<gint>(timeout_us));
    return *this;
  }

  StreamMux& live_source(bool live) {
    detail::set_property(mElement.get(), "live-source", static_cast<gboolean>(live));
    return *this;
  }

  StreamMux& sync_inputs(bool sync) {
    detail::set_property(mElement.get(), "sync-inputs", static_cast<gboolean>(sync));
    return *this;
  }

  [[nodiscard]] static nonstd::expected<StreamMux, ElementError> create(const StreamMuxConfig& cfg, std::string_view name = {}) {
    auto result = create(name);
    if(!result) {
      return result;
    }
    result->batch_size(cfg.batch_size)
        .width(cfg.width)
        .height(cfg.height)
        .batched_push_timeout(cfg.batched_push_timeout)
        .live_source(cfg.live_source)
        .sync_inputs(cfg.sync_inputs);
    return result;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

  StreamMux(StreamMux&&) = default;
  StreamMux& operator=(StreamMux&&) = default;
  StreamMux(const StreamMux&) = delete;
  StreamMux& operator=(const StreamMux&) = delete;

private:
  explicit StreamMux(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class VideoConverter {
public:
  [[nodiscard]] static nonstd::expected<VideoConverter, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvvideoconvert", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvvideoconvert' element"});
    }
    return VideoConverter{gst::raii::Element{raw}};
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

  VideoConverter(VideoConverter&&) = default;
  VideoConverter& operator=(VideoConverter&&) = default;
  VideoConverter(const VideoConverter&) = delete;
  VideoConverter& operator=(const VideoConverter&) = delete;

private:
  explicit VideoConverter(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class OSD {
public:
  [[nodiscard]] static nonstd::expected<OSD, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvdsosd", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvdsosd' element"});
    }
    return OSD{gst::raii::Element{raw}};
  }

  OSD& process_mode(std::int32_t mode) {
    detail::set_property(mElement.get(), "process-mode", static_cast<gint>(mode));
    return *this;
  }

  OSD& display_text(bool enable) {
    detail::set_property(mElement.get(), "display-text", static_cast<gboolean>(enable));
    return *this;
  }

  OSD& display_bbox(bool enable) {
    detail::set_property(mElement.get(), "display-bbox", static_cast<gboolean>(enable));
    return *this;
  }

  OSD& display_mask(bool enable) {
    detail::set_property(mElement.get(), "display-mask", static_cast<gboolean>(enable));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

  OSD(OSD&&) = default;
  OSD& operator=(OSD&&) = default;
  OSD(const OSD&) = delete;
  OSD& operator=(const OSD&) = delete;

private:
  explicit OSD(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};


class StreamDemux {
public:
  [[nodiscard]] static nonstd::expected<StreamDemux, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvstreamdemux", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvstreamdemux' element"});
    }
    return StreamDemux{gst::raii::Element{raw}};
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

  StreamDemux(StreamDemux&&) = default;
  StreamDemux& operator=(StreamDemux&&) = default;
  StreamDemux(const StreamDemux&) = delete;
  StreamDemux& operator=(const StreamDemux&) = delete;

private:
  explicit StreamDemux(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class Tiler {
public:
  [[nodiscard]] static nonstd::expected<Tiler, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvmultistreamtiler", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvmultistreamtiler' element"});
    }
    return Tiler{gst::raii::Element{raw}};
  }

  Tiler& rows(std::uint32_t num_rows) {
    detail::set_property(mElement.get(), "rows", static_cast<guint>(num_rows));
    return *this;
  }
  Tiler& columns(std::uint32_t num_cols) {
    detail::set_property(mElement.get(), "columns", static_cast<guint>(num_cols));
    return *this;
  }
  Tiler& width(std::uint32_t val) {
    detail::set_property(mElement.get(), "width", static_cast<guint>(val));
    return *this;
  }
  Tiler& height(std::uint32_t val) {
    detail::set_property(mElement.get(), "height", static_cast<guint>(val));
    return *this;
  }
  Tiler& batch_size(std::uint32_t size) {
    detail::set_property(mElement.get(), "batch-size", static_cast<guint>(size));
    return *this;
  }
  Tiler& gpu_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "gpu-id", static_cast<guint>(id));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

  Tiler(Tiler&&) = default;
  Tiler& operator=(Tiler&&) = default;
  Tiler(const Tiler&) = delete;
  Tiler& operator=(const Tiler&) = delete;

private:
  explicit Tiler(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
