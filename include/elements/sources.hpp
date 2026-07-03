#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class FileSource {
public:
  [[nodiscard]] static nonstd::expected<FileSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("filesrc", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'filesrc' element"});
    }
    return FileSource{gst::raii::Element{raw}};
  }

  FileSource& location(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "location", std::string(path).c_str(), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  FileSource(FileSource&&) = default;
  FileSource& operator=(FileSource&&) = default;
  FileSource(const FileSource&) = delete;
  FileSource& operator=(const FileSource&) = delete;

private:
  explicit FileSource(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class RTSPSource {
public:
  [[nodiscard]] static nonstd::expected<RTSPSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("rtspsrc", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'rtspsrc' element"});
    }
    return RTSPSource{gst::raii::Element{raw}};
  }

  RTSPSource& location(std::string_view url) {
    g_object_set(G_OBJECT(mElement.get()), "location", std::string(url).c_str(), nullptr);
    return *this;
  }

  RTSPSource& latency(std::uint32_t ms) {
    g_object_set(G_OBJECT(mElement.get()), "latency", static_cast<guint>(ms), nullptr);
    return *this;
  }

  RTSPSource& protocols(std::uint32_t flags) {
    g_object_set(G_OBJECT(mElement.get()), "protocols", static_cast<guint>(flags), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  RTSPSource(RTSPSource&&) = default;
  RTSPSource& operator=(RTSPSource&&) = default;
  RTSPSource(const RTSPSource&) = delete;
  RTSPSource& operator=(const RTSPSource&) = delete;

private:
  explicit RTSPSource(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class CameraSource {
public:
  [[nodiscard]] static nonstd::expected<CameraSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("v4l2src", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'v4l2src' element"});
    }
    return CameraSource{gst::raii::Element{raw}};
  }

  CameraSource& device(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "device", std::string(path).c_str(), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  CameraSource(CameraSource&&) = default;
  CameraSource& operator=(CameraSource&&) = default;
  CameraSource(const CameraSource&) = delete;
  CameraSource& operator=(const CameraSource&) = delete;

private:
  explicit CameraSource(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};


class UriSource {
public:
  [[nodiscard]] static nonstd::expected<UriSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvurisrcbin", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvurisrcbin' element"});
    }
    return UriSource{gst::raii::Element{raw}};
  }

  UriSource& uri(std::string_view u) {
    g_object_set(G_OBJECT(mElement.get()), "uri", std::string(u).c_str(), nullptr);
    return *this;
  }
  UriSource& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  UriSource& num_extra_surfaces(std::uint32_t n) {
    g_object_set(G_OBJECT(mElement.get()), "num-extra-surfaces", static_cast<guint>(n), nullptr);
    return *this;
  }
  UriSource& drop_frame_interval(std::uint32_t interval) {
    g_object_set(G_OBJECT(mElement.get()), "drop-frame-interval", static_cast<guint>(interval), nullptr);
    return *this;
  }
  UriSource& rtsp_reconnect_interval_sec(std::uint32_t sec) {
    g_object_set(G_OBJECT(mElement.get()), "rtsp-reconnect-interval", static_cast<guint>(sec), nullptr);
    return *this;
  }
  UriSource& source_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "source-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  UriSource& file_loop(bool loop) {
    g_object_set(G_OBJECT(mElement.get()), "file-loop", static_cast<gboolean>(loop), nullptr);
    return *this;
  }
  UriSource& smart_record(std::uint32_t mode) {
    g_object_set(G_OBJECT(mElement.get()), "smart-record", static_cast<guint>(mode), nullptr);
    return *this;
  }
  UriSource& smart_rec_dir_path(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "smart-rec-dir-path", std::string(path).c_str(), nullptr);
    return *this;
  }
  UriSource& smart_rec_file_prefix(std::string_view prefix) {
    g_object_set(G_OBJECT(mElement.get()), "smart-rec-file-prefix", std::string(prefix).c_str(), nullptr);
    return *this;
  }
  UriSource& smart_rec_def_duration(std::uint32_t seconds) {
    g_object_set(G_OBJECT(mElement.get()), "smart-rec-def-duration", static_cast<guint>(seconds), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  UriSource(UriSource&&) = default;
  UriSource& operator=(UriSource&&) = default;
  UriSource(const UriSource&) = delete;
  UriSource& operator=(const UriSource&) = delete;

private:
  explicit UriSource(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class MultiUriSource {
public:
  [[nodiscard]] static nonstd::expected<MultiUriSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvmultiurisrcbin", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvmultiurisrcbin' element"});
    }
    return MultiUriSource{gst::raii::Element{raw}};
  }

  MultiUriSource& uri_list(std::string_view uris) {
    g_object_set(G_OBJECT(mElement.get()), "uri-list", std::string(uris).c_str(), nullptr);
    return *this;
  }
  MultiUriSource& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  MultiUriSource& batch_size(std::uint32_t size) {
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }
  MultiUriSource& width(std::uint32_t w) {
    g_object_set(G_OBJECT(mElement.get()), "width", static_cast<guint>(w), nullptr);
    return *this;
  }
  MultiUriSource& height(std::uint32_t h) {
    g_object_set(G_OBJECT(mElement.get()), "height", static_cast<guint>(h), nullptr);
    return *this;
  }
  MultiUriSource& num_extra_surfaces(std::uint32_t n) {
    g_object_set(G_OBJECT(mElement.get()), "num-extra-surfaces", static_cast<guint>(n), nullptr);
    return *this;
  }
  MultiUriSource& drop_frame_interval(std::uint32_t interval) {
    g_object_set(G_OBJECT(mElement.get()), "drop-frame-interval", static_cast<guint>(interval), nullptr);
    return *this;
  }
  MultiUriSource& live_source(bool live) {
    g_object_set(G_OBJECT(mElement.get()), "live-source", static_cast<gboolean>(live), nullptr);
    return *this;
  }
  MultiUriSource& batched_push_timeout(std::int32_t timeout_us) {
    g_object_set(G_OBJECT(mElement.get()), "batched-push-timeout", static_cast<gint>(timeout_us), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  MultiUriSource(MultiUriSource&&) = default;
  MultiUriSource& operator=(MultiUriSource&&) = default;
  MultiUriSource(const MultiUriSource&) = delete;
  MultiUriSource& operator=(const MultiUriSource&) = delete;

private:
  explicit MultiUriSource(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class V4L2Decoder {
public:
  [[nodiscard]] static nonstd::expected<V4L2Decoder, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvv4l2decoder", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvv4l2decoder' element"});
    }
    return V4L2Decoder{gst::raii::Element{raw}};
  }

  V4L2Decoder& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  V4L2Decoder& num_extra_surfaces(std::uint32_t n) {
    g_object_set(G_OBJECT(mElement.get()), "num-extra-surfaces", static_cast<guint>(n), nullptr);
    return *this;
  }
  V4L2Decoder& drop_frame_interval(std::uint32_t interval) {
    g_object_set(G_OBJECT(mElement.get()), "drop-frame-interval", static_cast<guint>(interval), nullptr);
    return *this;
  }
  V4L2Decoder& enable_full_frame(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "enable-full-frame", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  V4L2Decoder(V4L2Decoder&&) = default;
  V4L2Decoder& operator=(V4L2Decoder&&) = default;
  V4L2Decoder(const V4L2Decoder&) = delete;
  V4L2Decoder& operator=(const V4L2Decoder&) = delete;

private:
  explicit V4L2Decoder(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
