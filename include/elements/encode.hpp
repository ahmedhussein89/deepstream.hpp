#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class H264Encoder {
public:
  [[nodiscard]] static nonstd::expected<H264Encoder, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvv4l2h264enc", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvv4l2h264enc' element"});
    }
    return H264Encoder{gst::raii::Element{raw}};
  }

  H264Encoder& bitrate(std::uint32_t bps) {
    g_object_set(G_OBJECT(mElement.get()), "bitrate", static_cast<guint>(bps), nullptr);
    return *this;
  }
  H264Encoder& iframeinterval(std::uint32_t interval) {
    g_object_set(G_OBJECT(mElement.get()), "iframeinterval", static_cast<guint>(interval), nullptr);
    return *this;
  }
  H264Encoder& profile(std::uint32_t p) {
    g_object_set(G_OBJECT(mElement.get()), "profile", static_cast<guint>(p), nullptr);
    return *this;
  }
  H264Encoder& preset_level(std::uint32_t level) {
    g_object_set(G_OBJECT(mElement.get()), "preset-level", static_cast<guint>(level), nullptr);
    return *this;
  }
  H264Encoder& insert_sps_pps(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "insert-sps-pps", static_cast<gboolean>(enable), nullptr);
    return *this;
  }
  H264Encoder& control_rate(std::uint32_t mode) {
    g_object_set(G_OBJECT(mElement.get()), "control-rate", static_cast<guint>(mode), nullptr);
    return *this;
  }
  H264Encoder& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  H264Encoder(H264Encoder&&) = default;
  H264Encoder& operator=(H264Encoder&&) = default;
  H264Encoder(const H264Encoder&) = delete;
  H264Encoder& operator=(const H264Encoder&) = delete;

private:
  explicit H264Encoder(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class H265Encoder {
public:
  [[nodiscard]] static nonstd::expected<H265Encoder, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvv4l2h265enc", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvv4l2h265enc' element"});
    }
    return H265Encoder{gst::raii::Element{raw}};
  }

  H265Encoder& bitrate(std::uint32_t bps) {
    g_object_set(G_OBJECT(mElement.get()), "bitrate", static_cast<guint>(bps), nullptr);
    return *this;
  }
  H265Encoder& iframeinterval(std::uint32_t interval) {
    g_object_set(G_OBJECT(mElement.get()), "iframeinterval", static_cast<guint>(interval), nullptr);
    return *this;
  }
  H265Encoder& profile(std::uint32_t p) {
    g_object_set(G_OBJECT(mElement.get()), "profile", static_cast<guint>(p), nullptr);
    return *this;
  }
  H265Encoder& preset_level(std::uint32_t level) {
    g_object_set(G_OBJECT(mElement.get()), "preset-level", static_cast<guint>(level), nullptr);
    return *this;
  }
  H265Encoder& insert_vps_sps_pps(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "insert-vps-sps-pps", static_cast<gboolean>(enable), nullptr);
    return *this;
  }
  H265Encoder& control_rate(std::uint32_t mode) {
    g_object_set(G_OBJECT(mElement.get()), "control-rate", static_cast<guint>(mode), nullptr);
    return *this;
  }
  H265Encoder& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  H265Encoder(H265Encoder&&) = default;
  H265Encoder& operator=(H265Encoder&&) = default;
  H265Encoder(const H265Encoder&) = delete;
  H265Encoder& operator=(const H265Encoder&) = delete;

private:
  explicit H265Encoder(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class RtspOutSink {
public:
  [[nodiscard]] static nonstd::expected<RtspOutSink, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvrtspoutsink", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvrtspoutsink' element"});
    }
    return RtspOutSink{gst::raii::Element{raw}};
  }

  RtspOutSink& host(std::string_view h) {
    g_object_set(G_OBJECT(mElement.get()), "host", std::string(h).c_str(), nullptr);
    return *this;
  }
  RtspOutSink& port(std::uint32_t p) {
    g_object_set(G_OBJECT(mElement.get()), "port", static_cast<guint>(p), nullptr);
    return *this;
  }
  RtspOutSink& udp_buffer_size(std::uint32_t bytes) {
    g_object_set(G_OBJECT(mElement.get()), "udp-buffer-size", static_cast<guint>(bytes), nullptr);
    return *this;
  }
  RtspOutSink& sync(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "sync", static_cast<gboolean>(enable), nullptr);
    return *this;
  }
  RtspOutSink& async(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "async", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  RtspOutSink(RtspOutSink&&) = default;
  RtspOutSink& operator=(RtspOutSink&&) = default;
  RtspOutSink(const RtspOutSink&) = delete;
  RtspOutSink& operator=(const RtspOutSink&) = delete;

private:
  explicit RtspOutSink(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class FakeSink {
public:
  [[nodiscard]] static nonstd::expected<FakeSink, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("fakesink", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'fakesink' element"});
    }
    return FakeSink{gst::raii::Element{raw}};
  }

  FakeSink& sync(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "sync", static_cast<gboolean>(enable), nullptr);
    return *this;
  }
  FakeSink& async(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "async", static_cast<gboolean>(enable), nullptr);
    return *this;
  }
  FakeSink& signal_handoffs(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "signal-handoffs", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  FakeSink(FakeSink&&) = default;
  FakeSink& operator=(FakeSink&&) = default;
  FakeSink(const FakeSink&) = delete;
  FakeSink& operator=(const FakeSink&) = delete;

private:
  explicit FakeSink(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
