#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

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
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }

  StreamMux& width(std::uint32_t w) {
    g_object_set(G_OBJECT(mElement.get()), "width", static_cast<guint>(w), nullptr);
    return *this;
  }

  StreamMux& height(std::uint32_t h) {
    g_object_set(G_OBJECT(mElement.get()), "height", static_cast<guint>(h), nullptr);
    return *this;
  }

  StreamMux& batched_push_timeout(std::int32_t timeout_us) {
    g_object_set(G_OBJECT(mElement.get()), "batched-push-timeout", static_cast<gint>(timeout_us), nullptr);
    return *this;
  }

  StreamMux& live_source(bool live) {
    g_object_set(G_OBJECT(mElement.get()), "live-source", static_cast<gboolean>(live), nullptr);
    return *this;
  }

  StreamMux& sync_inputs(bool sync) {
    g_object_set(G_OBJECT(mElement.get()), "sync-inputs", static_cast<gboolean>(sync), nullptr);
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
  operator bool() const { return static_cast<bool>(mElement); }

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
  operator bool() const { return static_cast<bool>(mElement); }

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
    g_object_set(G_OBJECT(mElement.get()), "process-mode", static_cast<gint>(mode), nullptr);
    return *this;
  }

  OSD& display_text(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "display-text", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  OSD& display_bbox(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "display-bbox", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  OSD& display_mask(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "display-mask", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

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
  operator bool() const { return static_cast<bool>(mElement); }

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

  Tiler& rows(std::uint32_t r) {
    g_object_set(G_OBJECT(mElement.get()), "rows", static_cast<guint>(r), nullptr);
    return *this;
  }
  Tiler& columns(std::uint32_t c) {
    g_object_set(G_OBJECT(mElement.get()), "columns", static_cast<guint>(c), nullptr);
    return *this;
  }
  Tiler& width(std::uint32_t w) {
    g_object_set(G_OBJECT(mElement.get()), "width", static_cast<guint>(w), nullptr);
    return *this;
  }
  Tiler& height(std::uint32_t h) {
    g_object_set(G_OBJECT(mElement.get()), "height", static_cast<guint>(h), nullptr);
    return *this;
  }
  Tiler& batch_size(std::uint32_t size) {
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }
  Tiler& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  Tiler(Tiler&&) = default;
  Tiler& operator=(Tiler&&) = default;
  Tiler(const Tiler&) = delete;
  Tiler& operator=(const Tiler&) = delete;

private:
  explicit Tiler(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
