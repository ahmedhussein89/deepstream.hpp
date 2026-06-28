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

}    // namespace

int main(int argc, char** argv) {
  gst_init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
