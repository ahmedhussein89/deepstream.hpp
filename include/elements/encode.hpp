#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>
#include <gstreamer_raii.hpp>

#include <elements/detail.hpp>
#include <nonstd/expected.hpp>
#include <utils/error.hpp>

namespace ds {

class H264Encoder {
public:
  [[nodiscard]] static nonstd::expected<H264Encoder, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvv4l2h264enc", name.empty() ? nullptr : std::string(name).c_str());
    if(nullptr == raw) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvv4l2h264enc' element"});
    }
    return H264Encoder{gst::raii::Element{raw}};
  }

  H264Encoder& bitrate(std::uint32_t bps) {
    detail::set_property(mElement.get(), "bitrate", static_cast<guint>(bps));
    return *this;
  }
  H264Encoder& iframeinterval(std::uint32_t interval) {
    detail::set_property(mElement.get(), "iframeinterval", static_cast<guint>(interval));
    return *this;
  }
  H264Encoder& profile(std::uint32_t val) {
    detail::set_property(mElement.get(), "profile", static_cast<guint>(val));
    return *this;
  }
  H264Encoder& preset_level(std::uint32_t level) {
    detail::set_property(mElement.get(), "preset-level", static_cast<guint>(level));
    return *this;
  }
  H264Encoder& insert_sps_pps(bool enable) {
    detail::set_property(mElement.get(), "insert-sps-pps", static_cast<gboolean>(enable));
    return *this;
  }
  H264Encoder& control_rate(std::uint32_t mode) {
    detail::set_property(mElement.get(), "control-rate", static_cast<guint>(mode));
    return *this;
  }
  H264Encoder& gpu_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "gpu-id", static_cast<guint>(id));
    return *this;
  }

  [[nodiscard]] GstElement* get() const {
    return mElement.get();
  }
  [[nodiscard]] GstElement* release() {
    return mElement.release();
  }
  operator bool() const {
    return static_cast<bool>(mElement);
  }

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
    detail::set_property(mElement.get(), "bitrate", static_cast<guint>(bps));
    return *this;
  }
  H265Encoder& iframeinterval(std::uint32_t interval) {
    detail::set_property(mElement.get(), "iframeinterval", static_cast<guint>(interval));
    return *this;
  }
  H265Encoder& profile(std::uint32_t val) {
    detail::set_property(mElement.get(), "profile", static_cast<guint>(val));
    return *this;
  }
  H265Encoder& preset_level(std::uint32_t level) {
    detail::set_property(mElement.get(), "preset-level", static_cast<guint>(level));
    return *this;
  }
  H265Encoder& insert_vps_sps_pps(bool enable) {
    detail::set_property(mElement.get(), "insert-vps-sps-pps", static_cast<gboolean>(enable));
    return *this;
  }
  H265Encoder& control_rate(std::uint32_t mode) {
    detail::set_property(mElement.get(), "control-rate", static_cast<guint>(mode));
    return *this;
  }
  H265Encoder& gpu_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "gpu-id", static_cast<guint>(id));
    return *this;
  }

  [[nodiscard]] GstElement* get() const {
    return mElement.get();
  }
  [[nodiscard]] GstElement* release() {
    return mElement.release();
  }
  operator bool() const {
    return static_cast<bool>(mElement);
  }

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

  RtspOutSink& host(std::string_view hostname) {
    detail::set_property(mElement.get(), "host", hostname);
    return *this;
  }
  RtspOutSink& port(std::uint32_t port_num) {
    detail::set_property(mElement.get(), "port", static_cast<guint>(port_num));
    return *this;
  }
  RtspOutSink& udp_buffer_size(std::uint32_t bytes) {
    detail::set_property(mElement.get(), "udp-buffer-size", static_cast<guint>(bytes));
    return *this;
  }
  RtspOutSink& sync(bool enable) {
    detail::set_property(mElement.get(), "sync", static_cast<gboolean>(enable));
    return *this;
  }
  RtspOutSink& async(bool enable) {
    detail::set_property(mElement.get(), "async", static_cast<gboolean>(enable));
    return *this;
  }

  [[nodiscard]] GstElement* get() const {
    return mElement.get();
  }
  [[nodiscard]] GstElement* release() {
    return mElement.release();
  }
  operator bool() const {
    return static_cast<bool>(mElement);
  }

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
    detail::set_property(mElement.get(), "sync", static_cast<gboolean>(enable));
    return *this;
  }
  FakeSink& async(bool enable) {
    detail::set_property(mElement.get(), "async", static_cast<gboolean>(enable));
    return *this;
  }
  FakeSink& signal_handoffs(bool enable) {
    detail::set_property(mElement.get(), "signal-handoffs", static_cast<gboolean>(enable));
    return *this;
  }

  [[nodiscard]] GstElement* get() const {
    return mElement.get();
  }
  [[nodiscard]] GstElement* release() {
    return mElement.release();
  }
  operator bool() const {
    return static_cast<bool>(mElement);
  }

  FakeSink(FakeSink&&) = default;
  FakeSink& operator=(FakeSink&&) = default;
  FakeSink(const FakeSink&) = delete;
  FakeSink& operator=(const FakeSink&) = delete;

private:
  explicit FakeSink(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
