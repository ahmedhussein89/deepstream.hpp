#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class SegVisual {
public:
  [[nodiscard]] static nonstd::expected<SegVisual, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvsegvisual", name.empty() ? nullptr : std::string(name).c_str());
    if(nullptr == raw) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvsegvisual' element"});
    }
    return SegVisual{gst::raii::Element{raw}};
  }

  SegVisual& batch_size(std::uint32_t size) {
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }
  SegVisual& width(std::uint32_t w) {
    g_object_set(G_OBJECT(mElement.get()), "width", static_cast<guint>(w), nullptr);
    return *this;
  }
  SegVisual& height(std::uint32_t h) {
    g_object_set(G_OBJECT(mElement.get()), "height", static_cast<guint>(h), nullptr);
    return *this;
  }
  SegVisual& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  SegVisual(SegVisual&&) = default;
  SegVisual& operator=(SegVisual&&) = default;
  SegVisual(const SegVisual&) = delete;
  SegVisual& operator=(const SegVisual&) = delete;

private:
  explicit SegVisual(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class OpticalFlow {
public:
  [[nodiscard]] static nonstd::expected<OpticalFlow, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvof", name.empty() ? nullptr : std::string(name).c_str());
    if(nullptr == raw) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvof' element"});
    }
    return OpticalFlow{gst::raii::Element{raw}};
  }

  OpticalFlow& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  OpticalFlow& preset(std::uint32_t p) {
    g_object_set(G_OBJECT(mElement.get()), "preset", static_cast<guint>(p), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  OpticalFlow(OpticalFlow&&) = default;
  OpticalFlow& operator=(OpticalFlow&&) = default;
  OpticalFlow(const OpticalFlow&) = delete;
  OpticalFlow& operator=(const OpticalFlow&) = delete;

private:
  explicit OpticalFlow(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class OpticalFlowVisual {
public:
  [[nodiscard]] static nonstd::expected<OpticalFlowVisual, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvofvisual", name.empty() ? nullptr : std::string(name).c_str());
    if(nullptr == raw) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvofvisual' element"});
    }
    return OpticalFlowVisual{gst::raii::Element{raw}};
  }

  OpticalFlowVisual& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  OpticalFlowVisual(OpticalFlowVisual&&) = default;
  OpticalFlowVisual& operator=(OpticalFlowVisual&&) = default;
  OpticalFlowVisual(const OpticalFlowVisual&) = delete;
  OpticalFlowVisual& operator=(const OpticalFlowVisual&) = delete;

private:
  explicit OpticalFlowVisual(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class Dewarper {
public:
  [[nodiscard]] static nonstd::expected<Dewarper, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvdewarper", name.empty() ? nullptr : std::string(name).c_str());
    if(nullptr == raw) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvdewarper' element"});
    }
    return Dewarper{gst::raii::Element{raw}};
  }

  Dewarper& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config-file", std::string(path).c_str(), nullptr);
    return *this;
  }
  Dewarper& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  Dewarper& num_batch_buffers(std::uint32_t n) {
    g_object_set(G_OBJECT(mElement.get()), "num-batch-buffers", static_cast<guint>(n), nullptr);
    return *this;
  }
  Dewarper& source_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "source-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  Dewarper(Dewarper&&) = default;
  Dewarper& operator=(Dewarper&&) = default;
  Dewarper(const Dewarper&) = delete;
  Dewarper& operator=(const Dewarper&) = delete;

private:
  explicit Dewarper(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
