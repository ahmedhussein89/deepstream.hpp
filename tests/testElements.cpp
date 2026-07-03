#include <type_traits>

#include <gst/gst.h>
#include <gtest/gtest.h>

#include <elements.hpp>

namespace {

bool plugin_available(const char* factory) {
  GstElementFactory* f = gst_element_factory_find(factory);
  if(f != nullptr) {
    gst_object_unref(f);
    return true;
  }
  return false;
}

// ============================================================================
// Shared trait assertions
// ============================================================================

template <typename T>
void assert_move_only() {
  static_assert(!std::is_copy_constructible_v<T>);
  static_assert(!std::is_copy_assignable_v<T>);
  static_assert(std::is_move_constructible_v<T>);
  static_assert(std::is_move_assignable_v<T>);
}

// ============================================================================
// FileSource
// ============================================================================

TEST(ElementsTest, FileSourceMoveOnly) { assert_move_only<ds::FileSource>(); }

TEST(ElementsTest, FileSourceCreateSucceeds) {
  auto result = ds::FileSource::create();
  ASSERT_TRUE(result.has_value());
  EXPECT_NE(result->get(), nullptr);
  EXPECT_TRUE(*result);
}

TEST(ElementsTest, FileSourceCreateWithName) {
  auto result = ds::FileSource::create("my-filesrc");
  ASSERT_TRUE(result.has_value());
  EXPECT_NE(result->get(), nullptr);
}

TEST(ElementsTest, FileSourceLocationSetterChains) {
  auto result = ds::FileSource::create();
  ASSERT_TRUE(result.has_value());

  ds::FileSource& ref = result->location("/tmp/test.mp4");
  EXPECT_EQ(&ref, &*result);

  gchar* loc = nullptr;
  g_object_get(G_OBJECT(result->get()), "location", &loc, nullptr);
  ASSERT_NE(loc, nullptr);
  EXPECT_STREQ(loc, "/tmp/test.mp4");
  g_free(loc);
}

// ============================================================================
// RTSPSource
// ============================================================================

TEST(ElementsTest, RTSPSourceMoveOnly) { assert_move_only<ds::RTSPSource>(); }

TEST(ElementsTest, RTSPSourceCreateSucceeds) {
  auto result = ds::RTSPSource::create();
  ASSERT_TRUE(result.has_value());
  EXPECT_NE(result->get(), nullptr);
}

TEST(ElementsTest, RTSPSourceLocationSetter) {
  auto result = ds::RTSPSource::create();
  ASSERT_TRUE(result.has_value());

  result->location("rtsp://192.168.1.1/stream");

  gchar* loc = nullptr;
  g_object_get(G_OBJECT(result->get()), "location", &loc, nullptr);
  ASSERT_NE(loc, nullptr);
  EXPECT_STREQ(loc, "rtsp://192.168.1.1/stream");
  g_free(loc);
}

TEST(ElementsTest, RTSPSourceLatencySetter) {
  auto result = ds::RTSPSource::create();
  ASSERT_TRUE(result.has_value());

  result->latency(200);

  guint val = 0;
  g_object_get(G_OBJECT(result->get()), "latency", &val, nullptr);
  EXPECT_EQ(val, 200u);
}

// ============================================================================
// CameraSource
// ============================================================================

TEST(ElementsTest, CameraSourceMoveOnly) { assert_move_only<ds::CameraSource>(); }

TEST(ElementsTest, CameraSourceCreateSucceeds) {
  auto result = ds::CameraSource::create();
  ASSERT_TRUE(result.has_value());
  EXPECT_NE(result->get(), nullptr);
}

TEST(ElementsTest, CameraSourceDeviceSetter) {
  auto result = ds::CameraSource::create();
  ASSERT_TRUE(result.has_value());

  result->device("/dev/video0");

  gchar* dev = nullptr;
  g_object_get(G_OBJECT(result->get()), "device", &dev, nullptr);
  ASSERT_NE(dev, nullptr);
  EXPECT_STREQ(dev, "/dev/video0");
  g_free(dev);
}

// ============================================================================
// StreamMux (nvstreammux — DeepStream only)
// ============================================================================

TEST(ElementsTest, StreamMuxMoveOnly) { assert_move_only<ds::StreamMux>(); }

TEST(ElementsTest, StreamMuxCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvstreammux")) {
    GTEST_SKIP() << "nvstreammux available; skipping unavailability test";
  }
  auto result = ds::StreamMux::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_FALSE(result.error().empty());
  EXPECT_NE(result.error().find("nvstreammux"), std::string::npos);
}

TEST(ElementsTest, StreamMuxCreateSucceedsWithDeepStream) {
  if(!plugin_available("nvstreammux")) {
    GTEST_SKIP() << "nvstreammux not available";
  }
  auto result = ds::StreamMux::create();
  ASSERT_TRUE(result.has_value());

  result->batch_size(4).width(1920).height(1080).live_source(false).sync_inputs(false);
  EXPECT_NE(result->get(), nullptr);
}

// ============================================================================
// VideoConverter (nvvideoconvert — DeepStream only)
// ============================================================================

TEST(ElementsTest, VideoConverterMoveOnly) { assert_move_only<ds::VideoConverter>(); }

TEST(ElementsTest, VideoConverterCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvvideoconvert")) {
    GTEST_SKIP() << "nvvideoconvert available; skipping unavailability test";
  }
  auto result = ds::VideoConverter::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvvideoconvert"), std::string::npos);
}

// ============================================================================
// OSD (nvdsosd — DeepStream only)
// ============================================================================

TEST(ElementsTest, OSDMoveOnly) { assert_move_only<ds::OSD>(); }

TEST(ElementsTest, OSDCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvdsosd")) {
    GTEST_SKIP() << "nvdsosd available; skipping unavailability test";
  }
  auto result = ds::OSD::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvdsosd"), std::string::npos);
}

// ============================================================================
// PrimaryInfer (nvinfer — DeepStream only)
// ============================================================================

TEST(ElementsTest, PrimaryInferMoveOnly) { assert_move_only<ds::PrimaryInfer>(); }

TEST(ElementsTest, PrimaryInferCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvinfer")) {
    GTEST_SKIP() << "nvinfer available; skipping unavailability test";
  }
  auto result = ds::PrimaryInfer::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvinfer"), std::string::npos);
}

// ============================================================================
// SecondaryInfer (nvinfer — DeepStream only)
// ============================================================================

TEST(ElementsTest, SecondaryInferMoveOnly) { assert_move_only<ds::SecondaryInfer>(); }

TEST(ElementsTest, SecondaryInferCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvinfer")) {
    GTEST_SKIP() << "nvinfer available; skipping unavailability test";
  }
  auto result = ds::SecondaryInfer::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvinfer"), std::string::npos);
}

// ============================================================================
// Tracker (nvtracker — DeepStream only)
// ============================================================================

TEST(ElementsTest, TrackerMoveOnly) { assert_move_only<ds::Tracker>(); }

TEST(ElementsTest, TrackerCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvtracker")) {
    GTEST_SKIP() << "nvtracker available; skipping unavailability test";
  }
  auto result = ds::Tracker::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvtracker"), std::string::npos);
}

// ============================================================================
// WindowSink (nveglglessink — DeepStream only)
// ============================================================================

TEST(ElementsTest, WindowSinkMoveOnly) { assert_move_only<ds::WindowSink>(); }

TEST(ElementsTest, WindowSinkCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nveglglessink")) {
    GTEST_SKIP() << "nveglglessink available; skipping unavailability test";
  }
  auto result = ds::WindowSink::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nveglglessink"), std::string::npos);
}

// ============================================================================
// FileSink
// ============================================================================

TEST(ElementsTest, FileSinkMoveOnly) { assert_move_only<ds::FileSink>(); }

TEST(ElementsTest, FileSinkCreateSucceeds) {
  auto result = ds::FileSink::create();
  ASSERT_TRUE(result.has_value());
  EXPECT_NE(result->get(), nullptr);
}

TEST(ElementsTest, FileSinkLocationSetter) {
  auto result = ds::FileSink::create();
  ASSERT_TRUE(result.has_value());

  result->location("/tmp/output.mp4");

  gchar* loc = nullptr;
  g_object_get(G_OBJECT(result->get()), "location", &loc, nullptr);
  ASSERT_NE(loc, nullptr);
  EXPECT_STREQ(loc, "/tmp/output.mp4");
  g_free(loc);
}

TEST(ElementsTest, FileSinkSyncSetter) {
  auto result = ds::FileSink::create();
  ASSERT_TRUE(result.has_value());

  result->sync(false);

  gboolean val = TRUE;
  g_object_get(G_OBJECT(result->get()), "sync", &val, nullptr);
  EXPECT_FALSE(val);
}

TEST(ElementsTest, FileSinkSetterChaining) {
  auto result = ds::FileSink::create();
  ASSERT_TRUE(result.has_value());

  ds::FileSink& ref = result->location("/tmp/out.mp4").sync(false);
  EXPECT_EQ(&ref, &*result);
}

// ============================================================================
// UriSource (nvurisrcbin — DeepStream only)
// ============================================================================

TEST(ElementsTest, UriSourceMoveOnly) { assert_move_only<ds::UriSource>(); }

TEST(ElementsTest, UriSourceCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvurisrcbin")) {
    GTEST_SKIP() << "nvurisrcbin available; skipping unavailability test";
  }
  auto result = ds::UriSource::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvurisrcbin"), std::string::npos);
}

TEST(ElementsTest, UriSourceSettersChain) {
  if(!plugin_available("nvurisrcbin")) {
    GTEST_SKIP() << "nvurisrcbin not available";
  }
  auto result = ds::UriSource::create();
  ASSERT_TRUE(result.has_value());
  ds::UriSource& ref = result->uri("file:///tmp/test.mp4").gpu_id(0).file_loop(false);
  EXPECT_EQ(&ref, &*result);
}

// ============================================================================
// MultiUriSource (nvmultiurisrcbin — DeepStream only)
// ============================================================================

TEST(ElementsTest, MultiUriSourceMoveOnly) { assert_move_only<ds::MultiUriSource>(); }

TEST(ElementsTest, MultiUriSourceCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvmultiurisrcbin")) {
    GTEST_SKIP() << "nvmultiurisrcbin available; skipping unavailability test";
  }
  auto result = ds::MultiUriSource::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvmultiurisrcbin"), std::string::npos);
}

// ============================================================================
// V4L2Decoder (nvv4l2decoder — DeepStream only)
// ============================================================================

TEST(ElementsTest, V4L2DecoderMoveOnly) { assert_move_only<ds::V4L2Decoder>(); }

TEST(ElementsTest, V4L2DecoderCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvv4l2decoder")) {
    GTEST_SKIP() << "nvv4l2decoder available; skipping unavailability test";
  }
  auto result = ds::V4L2Decoder::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvv4l2decoder"), std::string::npos);
}

// ============================================================================
// StreamMuxConfig create overload
// ============================================================================

TEST(ElementsTest, StreamMuxCreateWithConfigFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvstreammux")) {
    GTEST_SKIP() << "nvstreammux available; skipping unavailability test";
  }
  ds::StreamMuxConfig cfg;
  cfg.batch_size = 4;
  cfg.width      = 1920;
  cfg.height     = 1080;
  auto result = ds::StreamMux::create(cfg);
  EXPECT_FALSE(result.has_value());
}

// ============================================================================
// StreamDemux (nvstreamdemux — DeepStream only)
// ============================================================================

TEST(ElementsTest, StreamDemuxMoveOnly) { assert_move_only<ds::StreamDemux>(); }

TEST(ElementsTest, StreamDemuxCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvstreamdemux")) {
    GTEST_SKIP() << "nvstreamdemux available; skipping unavailability test";
  }
  auto result = ds::StreamDemux::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvstreamdemux"), std::string::npos);
}

// ============================================================================
// Tiler (nvmultistreamtiler — DeepStream only)
// ============================================================================

TEST(ElementsTest, TilerMoveOnly) { assert_move_only<ds::Tiler>(); }

TEST(ElementsTest, TilerCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvmultistreamtiler")) {
    GTEST_SKIP() << "nvmultistreamtiler available; skipping unavailability test";
  }
  auto result = ds::Tiler::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvmultistreamtiler"), std::string::npos);
}

// ============================================================================
// InferServer (nvinferserver — DeepStream only)
// ============================================================================

TEST(ElementsTest, InferServerMoveOnly) { assert_move_only<ds::InferServer>(); }

TEST(ElementsTest, InferServerCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvinferserver")) {
    GTEST_SKIP() << "nvinferserver available; skipping unavailability test";
  }
  auto result = ds::InferServer::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvinferserver"), std::string::npos);
}

// ============================================================================
// NvInferConfig create overload
// ============================================================================

TEST(ElementsTest, PrimaryInferCreateWithConfigFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvinfer")) {
    GTEST_SKIP() << "nvinfer available; skipping unavailability test";
  }
  ds::NvInferConfig cfg;
  cfg.config_file_path = "/tmp/config.txt";
  auto result = ds::PrimaryInfer::create(cfg);
  EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Preprocess (nvdspreprocess — DeepStream only)
// ============================================================================

TEST(ElementsTest, PreprocessMoveOnly) { assert_move_only<ds::Preprocess>(); }

TEST(ElementsTest, PreprocessCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvdspreprocess")) {
    GTEST_SKIP() << "nvdspreprocess available; skipping unavailability test";
  }
  auto result = ds::Preprocess::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvdspreprocess"), std::string::npos);
}

// ============================================================================
// Analytics (nvdsanalytics — DeepStream only)
// ============================================================================

TEST(ElementsTest, AnalyticsMoveOnly) { assert_move_only<ds::Analytics>(); }

TEST(ElementsTest, AnalyticsCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvdsanalytics")) {
    GTEST_SKIP() << "nvdsanalytics available; skipping unavailability test";
  }
  auto result = ds::Analytics::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvdsanalytics"), std::string::npos);
}

// ============================================================================
// TrackerConfig create overload
// ============================================================================

TEST(ElementsTest, TrackerCreateWithConfigFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvtracker")) {
    GTEST_SKIP() << "nvtracker available; skipping unavailability test";
  }
  ds::TrackerConfig cfg;
  cfg.lib_file = "/opt/nvidia/deepstream/deepstream/lib/libnvds_mot_klt.so";
  auto result = ds::Tracker::create(cfg);
  EXPECT_FALSE(result.has_value());
}

// ============================================================================
// H264Encoder (nvv4l2h264enc — DeepStream only)
// ============================================================================

TEST(ElementsTest, H264EncoderMoveOnly) { assert_move_only<ds::H264Encoder>(); }

TEST(ElementsTest, H264EncoderCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvv4l2h264enc")) {
    GTEST_SKIP() << "nvv4l2h264enc available; skipping unavailability test";
  }
  auto result = ds::H264Encoder::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvv4l2h264enc"), std::string::npos);
}

// ============================================================================
// H265Encoder (nvv4l2h265enc — DeepStream only)
// ============================================================================

TEST(ElementsTest, H265EncoderMoveOnly) { assert_move_only<ds::H265Encoder>(); }

TEST(ElementsTest, H265EncoderCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvv4l2h265enc")) {
    GTEST_SKIP() << "nvv4l2h265enc available; skipping unavailability test";
  }
  auto result = ds::H265Encoder::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvv4l2h265enc"), std::string::npos);
}

// ============================================================================
// RtspOutSink (nvrtspoutsink — DeepStream only)
// ============================================================================

TEST(ElementsTest, RtspOutSinkMoveOnly) { assert_move_only<ds::RtspOutSink>(); }

TEST(ElementsTest, RtspOutSinkCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvrtspoutsink")) {
    GTEST_SKIP() << "nvrtspoutsink available; skipping unavailability test";
  }
  auto result = ds::RtspOutSink::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvrtspoutsink"), std::string::npos);
}

// ============================================================================
// FakeSink (fakesink — always available)
// ============================================================================

TEST(ElementsTest, FakeSinkMoveOnly) { assert_move_only<ds::FakeSink>(); }

TEST(ElementsTest, FakeSinkCreateSucceeds) {
  auto result = ds::FakeSink::create();
  ASSERT_TRUE(result.has_value());
  EXPECT_NE(result->get(), nullptr);
}

TEST(ElementsTest, FakeSinkSetterChaining) {
  auto result = ds::FakeSink::create();
  ASSERT_TRUE(result.has_value());
  ds::FakeSink& ref = result->sync(false).async(false).signal_handoffs(false);
  EXPECT_EQ(&ref, &*result);
}

// ============================================================================
// MsgConv (nvmsgconv — DeepStream only)
// ============================================================================

TEST(ElementsTest, MsgConvMoveOnly) { assert_move_only<ds::MsgConv>(); }

TEST(ElementsTest, MsgConvCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvmsgconv")) {
    GTEST_SKIP() << "nvmsgconv available; skipping unavailability test";
  }
  auto result = ds::MsgConv::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvmsgconv"), std::string::npos);
}

// ============================================================================
// MsgBroker (nvmsgbroker — DeepStream only)
// ============================================================================

TEST(ElementsTest, MsgBrokerMoveOnly) { assert_move_only<ds::MsgBroker>(); }

TEST(ElementsTest, MsgBrokerCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvmsgbroker")) {
    GTEST_SKIP() << "nvmsgbroker available; skipping unavailability test";
  }
  auto result = ds::MsgBroker::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvmsgbroker"), std::string::npos);
}

// ============================================================================
// SegVisual (nvsegvisual — DeepStream only)
// ============================================================================

TEST(ElementsTest, SegVisualMoveOnly) { assert_move_only<ds::SegVisual>(); }

TEST(ElementsTest, SegVisualCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvsegvisual")) {
    GTEST_SKIP() << "nvsegvisual available; skipping unavailability test";
  }
  auto result = ds::SegVisual::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvsegvisual"), std::string::npos);
}

// ============================================================================
// OpticalFlow (nvof — DeepStream only)
// ============================================================================

TEST(ElementsTest, OpticalFlowMoveOnly) { assert_move_only<ds::OpticalFlow>(); }

TEST(ElementsTest, OpticalFlowCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvof")) {
    GTEST_SKIP() << "nvof available; skipping unavailability test";
  }
  auto result = ds::OpticalFlow::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvof"), std::string::npos);
}

// ============================================================================
// OpticalFlowVisual (nvofvisual — DeepStream only)
// ============================================================================

TEST(ElementsTest, OpticalFlowVisualMoveOnly) { assert_move_only<ds::OpticalFlowVisual>(); }

TEST(ElementsTest, OpticalFlowVisualCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvofvisual")) {
    GTEST_SKIP() << "nvofvisual available; skipping unavailability test";
  }
  auto result = ds::OpticalFlowVisual::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvofvisual"), std::string::npos);
}

// ============================================================================
// Dewarper (nvdewarper — DeepStream only)
// ============================================================================

TEST(ElementsTest, DewarperMoveOnly) { assert_move_only<ds::Dewarper>(); }

TEST(ElementsTest, DewarperCreateFailsGracefullyWithoutDeepStream) {
  if(plugin_available("nvdewarper")) {
    GTEST_SKIP() << "nvdewarper available; skipping unavailability test";
  }
  auto result = ds::Dewarper::create();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("nvdewarper"), std::string::npos);
}

}    // namespace

int main(int argc, char** argv) {
  gst_init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
