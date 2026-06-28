#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer.hpp>
#include <utils/error.hpp>

namespace ds {

class FileSource {
public:
  [[nodiscard]] static nonstd::expected<FileSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("filesrc", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'filesrc' element"});
    }
    return FileSource{gst::Element{raw}};
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
  explicit FileSource(gst::Element element) : mElement(std::move(element)) {}
  gst::Element mElement;
};

class RTSPSource {
public:
  [[nodiscard]] static nonstd::expected<RTSPSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("rtspsrc", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'rtspsrc' element"});
    }
    return RTSPSource{gst::Element{raw}};
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
  explicit RTSPSource(gst::Element element) : mElement(std::move(element)) {}
  gst::Element mElement;
};

class CameraSource {
public:
  [[nodiscard]] static nonstd::expected<CameraSource, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("v4l2src", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'v4l2src' element"});
    }
    return CameraSource{gst::Element{raw}};
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
  explicit CameraSource(gst::Element element) : mElement(std::move(element)) {}
  gst::Element mElement;
};

}    // namespace ds
