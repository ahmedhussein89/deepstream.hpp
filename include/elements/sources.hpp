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
    detail::set_property(mElement.get(), "location", path);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

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
    detail::set_property(mElement.get(), "location", url);
    return *this;
  }

  RTSPSource& latency(std::uint32_t ms) {
    detail::set_property(mElement.get(), "latency", static_cast<guint>(ms));
    return *this;
  }

  RTSPSource& protocols(std::uint32_t flags) {
    detail::set_property(mElement.get(), "protocols", static_cast<guint>(flags));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

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
    detail::set_property(mElement.get(), "device", path);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

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

  UriSource& uri(std::string_view uri_str) {
    detail::set_property(mElement.get(), "uri", uri_str);
    return *this;
  }
  UriSource& gpu_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "gpu-id", static_cast<guint>(id));
    return *this;
  }
  UriSource& num_extra_surfaces(std::uint32_t count) {
    detail::set_property(mElement.get(), "num-extra-surfaces", static_cast<guint>(count));
    return *this;
  }
  UriSource& drop_frame_interval(std::uint32_t interval) {
    detail::set_property(mElement.get(), "drop-frame-interval", static_cast<guint>(interval));
    return *this;
  }
  UriSource& rtsp_reconnect_interval_sec(std::uint32_t sec) {
    detail::set_property(mElement.get(), "rtsp-reconnect-interval", static_cast<guint>(sec));
    return *this;
  }
  UriSource& source_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "source-id", static_cast<guint>(id));
    return *this;
  }
  UriSource& file_loop(bool loop) {
    detail::set_property(mElement.get(), "file-loop", static_cast<gboolean>(loop));
    return *this;
  }
  UriSource& smart_record(std::uint32_t mode) {
    detail::set_property(mElement.get(), "smart-record", static_cast<guint>(mode));
    return *this;
  }
  UriSource& smart_rec_dir_path(std::string_view path) {
    detail::set_property(mElement.get(), "smart-rec-dir-path", path);
    return *this;
  }
  UriSource& smart_rec_file_prefix(std::string_view prefix) {
    detail::set_property(mElement.get(), "smart-rec-file-prefix", prefix);
    return *this;
  }
  UriSource& smart_rec_def_duration(std::uint32_t seconds) {
    detail::set_property(mElement.get(), "smart-rec-def-duration", static_cast<guint>(seconds));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

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
    detail::set_property(mElement.get(), "uri-list", uris);
    return *this;
  }
  MultiUriSource& gpu_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "gpu-id", static_cast<guint>(id));
    return *this;
  }
  MultiUriSource& batch_size(std::uint32_t size) {
    detail::set_property(mElement.get(), "batch-size", static_cast<guint>(size));
    return *this;
  }
  MultiUriSource& width(std::uint32_t val) {
    detail::set_property(mElement.get(), "width", static_cast<guint>(val));
    return *this;
  }
  MultiUriSource& height(std::uint32_t val) {
    detail::set_property(mElement.get(), "height", static_cast<guint>(val));
    return *this;
  }
  MultiUriSource& num_extra_surfaces(std::uint32_t count) {
    detail::set_property(mElement.get(), "num-extra-surfaces", static_cast<guint>(count));
    return *this;
  }
  MultiUriSource& drop_frame_interval(std::uint32_t interval) {
    detail::set_property(mElement.get(), "drop-frame-interval", static_cast<guint>(interval));
    return *this;
  }
  MultiUriSource& live_source(bool live) {
    detail::set_property(mElement.get(), "live-source", static_cast<gboolean>(live));
    return *this;
  }
  MultiUriSource& batched_push_timeout(std::int32_t timeout_us) {
    detail::set_property(mElement.get(), "batched-push-timeout", static_cast<gint>(timeout_us));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

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
    detail::set_property(mElement.get(), "gpu-id", static_cast<guint>(id));
    return *this;
  }
  V4L2Decoder& num_extra_surfaces(std::uint32_t count) {
    detail::set_property(mElement.get(), "num-extra-surfaces", static_cast<guint>(count));
    return *this;
  }
  V4L2Decoder& drop_frame_interval(std::uint32_t interval) {
    detail::set_property(mElement.get(), "drop-frame-interval", static_cast<guint>(interval));
    return *this;
  }
  V4L2Decoder& enable_full_frame(bool enable) {
    detail::set_property(mElement.get(), "enable-full-frame", static_cast<gboolean>(enable));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }  // NOLINT(google-explicit-constructor)

  V4L2Decoder(V4L2Decoder&&) = default;
  V4L2Decoder& operator=(V4L2Decoder&&) = default;
  V4L2Decoder(const V4L2Decoder&) = delete;
  V4L2Decoder& operator=(const V4L2Decoder&) = delete;

private:
  explicit V4L2Decoder(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
