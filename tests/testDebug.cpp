#include <atomic>
#include <string>
#include <vector>

#include <gst/gst.h>
#include <gtest/gtest.h>

#include <builder.hpp>
#include <elements.hpp>
#include <utils/debug.hpp>
#include <utils/error.hpp>

// ============================================================================
// Fixture — resets DebugLayer state between tests
// ============================================================================

class DebugLayerTest : public ::testing::Test {
protected:
  void SetUp() override {
    ds::DebugLayer::instance().clear_callback();
    ds::DebugLayer::instance().set_min_level(ds::DebugLevel::Debug);
  }

  void TearDown() override {
    ds::DebugLayer::instance().clear_callback();
    ds::DebugLayer::instance().set_min_level(ds::DebugLevel::Debug);
  }
};

// ============================================================================
// DebugLevel helpers
// ============================================================================

TEST_F(DebugLayerTest, LevelStrings) {
  EXPECT_EQ(ds::debug_level_str(ds::DebugLevel::Debug), "DEBUG");
  EXPECT_EQ(ds::debug_level_str(ds::DebugLevel::Info), "INFO");
  EXPECT_EQ(ds::debug_level_str(ds::DebugLevel::Warn), "WARN");
  EXPECT_EQ(ds::debug_level_str(ds::DebugLevel::Error), "ERROR");
  EXPECT_EQ(ds::debug_level_str(ds::DebugLevel::Off), "OFF");
}

// ============================================================================
// DebugMessage helpers
// ============================================================================

TEST_F(DebugLayerTest, MessageLevelStr) {
  ds::DebugMessage msg{ds::DebugLevel::Warn, ds::ErrorKind::IncompatibleCaps, "bad caps", "file.cpp", 42};
  EXPECT_EQ(msg.level_str(), "WARN");
  EXPECT_EQ(msg.kind_str(), "IncompatibleCaps");
}

// ============================================================================
// Callback registration and dispatch
// ============================================================================

TEST_F(DebugLayerTest, CallbackReceivesMessage) {
  std::vector<ds::DebugMessage> received;
  ds::DebugLayer::instance().set_callback([&received](const ds::DebugMessage& m) { received.push_back(m); });

  DS_INFO("hello {}", 42);

  ASSERT_EQ(received.size(), 1u);
  EXPECT_EQ(received[0].level, ds::DebugLevel::Info);
  EXPECT_EQ(received[0].message, "hello 42");
  EXPECT_EQ(received[0].kind, ds::ErrorKind::Unknown);
}

TEST_F(DebugLayerTest, CallbackReceivesAllLevels) {
  int count = 0;
  ds::DebugLayer::instance().set_callback([&count](const ds::DebugMessage&) { ++count; });

  DS_DEBUG("a");
  DS_INFO("b");
  DS_WARN("c");
  DS_ERROR("d");

  EXPECT_EQ(count, 4);
}

TEST_F(DebugLayerTest, ClearCallbackSilencesDispatch) {
  int count = 0;
  ds::DebugLayer::instance().set_callback([&count](const ds::DebugMessage&) { ++count; });
  DS_DEBUG("one");
  EXPECT_EQ(count, 1);

  ds::DebugLayer::instance().clear_callback();
  DS_DEBUG("two");    // no callback registered — must not crash
  EXPECT_EQ(count, 1);
}

TEST_F(DebugLayerTest, NoCallbackDoesNotCrash) {
  // Dispatch with no callback set must be a safe no-op.
  DS_DEBUG("safe");
  DS_WARN("also safe");
  SUCCEED();
}

// ============================================================================
// Min-level filtering
// ============================================================================

TEST_F(DebugLayerTest, MinLevelFiltersLower) {
  int count = 0;
  ds::DebugLayer::instance().set_callback([&count](const ds::DebugMessage&) { ++count; });
  ds::DebugLayer::instance().set_min_level(ds::DebugLevel::Warn);

  DS_DEBUG("should be filtered");
  DS_INFO("should be filtered");
  DS_WARN("should pass");
  DS_ERROR("should pass");

  EXPECT_EQ(count, 2);
}

TEST_F(DebugLayerTest, MinLevelOffSuppressesAll) {
  int count = 0;
  ds::DebugLayer::instance().set_callback([&count](const ds::DebugMessage&) { ++count; });
  ds::DebugLayer::instance().set_min_level(ds::DebugLevel::Off);

  DS_DEBUG("suppressed");
  DS_ERROR("suppressed");

  EXPECT_EQ(count, 0);
}

TEST_F(DebugLayerTest, MinLevelQueryMatchesSetter) {
  ds::DebugLayer::instance().set_min_level(ds::DebugLevel::Warn);
  EXPECT_EQ(ds::DebugLayer::instance().min_level(), ds::DebugLevel::Warn);
}

// ============================================================================
// ErrorKind helpers
// ============================================================================

TEST(ErrorKindTest, KindStrings) {
  using ds::ErrorKind;
  using ds::error_kind_str;
  EXPECT_EQ(error_kind_str(ErrorKind::Unknown), "Unknown");
  EXPECT_EQ(error_kind_str(ErrorKind::ElementCreation), "ElementCreation");
  EXPECT_EQ(error_kind_str(ErrorKind::ElementLink), "ElementLink");
  EXPECT_EQ(error_kind_str(ErrorKind::ElementState), "ElementState");
  EXPECT_EQ(error_kind_str(ErrorKind::NoElements), "NoElements");
  EXPECT_EQ(error_kind_str(ErrorKind::DuplicateName), "DuplicateName");
  EXPECT_EQ(error_kind_str(ErrorKind::IncompatibleCaps), "IncompatibleCaps");
  EXPECT_EQ(error_kind_str(ErrorKind::PipelineCreation), "PipelineCreation");
  EXPECT_EQ(error_kind_str(ErrorKind::BinAdd), "BinAdd");
  EXPECT_EQ(error_kind_str(ErrorKind::ParseLaunch), "ParseLaunch");
}

// ============================================================================
// ds::Error — string-compatible interface
// ============================================================================

TEST(ErrorTest, EmptyDefaultConstructed) {
  const ds::Error e;
  EXPECT_TRUE(e.empty());
  EXPECT_EQ(e.kind, ds::ErrorKind::Unknown);
}

TEST(ErrorTest, MessageAndKind) {
  const ds::Error e{ds::ErrorKind::NoElements, "no elements"};
  EXPECT_FALSE(e.empty());
  EXPECT_EQ(e.kind, ds::ErrorKind::NoElements);
  EXPECT_EQ(e.what(), "no elements");
}

TEST(ErrorTest, FindSubstring) {
  const ds::Error e{ds::ErrorKind::ElementCreation, "Failed to create 'nvinfer' element"};
  EXPECT_NE(e.find("nvinfer"), std::string::npos);
  EXPECT_EQ(e.find("nonexistent"), std::string::npos);
}

TEST(ErrorTest, EqualityWithStringView) {
  const ds::Error e{ds::ErrorKind::Unknown, "hello"};
  EXPECT_TRUE(e == "hello");
  EXPECT_FALSE(e == "world");
  EXPECT_FALSE(e != "hello");
  EXPECT_TRUE(e != "world");
}

TEST(ErrorTest, EqualitySymmetric) {
  const ds::Error e{ds::ErrorKind::Unknown, "msg"};
  EXPECT_TRUE("msg" == e);
  EXPECT_FALSE("other" == e);
}

TEST(ErrorTest, ElementErrorIsError) {
  const ds::ElementError ee{ds::ErrorKind::ElementCreation, "Failed to create 'fakesrc' element"};
  const ds::Error& base = ee;
  EXPECT_EQ(base.kind, ds::ErrorKind::ElementCreation);
  EXPECT_NE(base.find("fakesrc"), std::string::npos);
}

TEST(ErrorTest, PipelineErrorIsError) {
  const ds::PipelineError pe{ds::ErrorKind::NoElements, "Pipeline must contain at least one element"};
  EXPECT_TRUE(pe == "Pipeline must contain at least one element");
  EXPECT_EQ(pe.kind, ds::ErrorKind::NoElements);
}

// ============================================================================
// Builder integration — typed errors dispatched through DebugLayer
// ============================================================================

TEST(DebugLayerBuilderTest, EmptyBuildDispatchesCallback) {
  std::vector<ds::DebugMessage> received;
  ds::DebugLayer::instance().set_callback([&received](const ds::DebugMessage& m) { received.push_back(m); });

  auto result = ds::Builder{}.build();

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error().kind, ds::ErrorKind::NoElements);

  ASSERT_GE(received.size(), 1u);
  EXPECT_EQ(received.back().level, ds::DebugLevel::Error);
  EXPECT_EQ(received.back().kind, ds::ErrorKind::NoElements);

  ds::DebugLayer::instance().clear_callback();
}

TEST(DebugLayerBuilderTest, DuplicateNameDispatchesCallback) {
  std::vector<ds::DebugMessage> received;
  ds::DebugLayer::instance().set_callback([&received](const ds::DebugMessage& m) { received.push_back(m); });

  auto result = ds::Builder{}
                    .add(gst::Element{gst_element_factory_make("fakesrc", "dup")})
                    .add(gst::Element{gst_element_factory_make("fakesink", "dup")})
                    .build();

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error().kind, ds::ErrorKind::DuplicateName);

  ASSERT_GE(received.size(), 1u);
  EXPECT_EQ(received.back().kind, ds::ErrorKind::DuplicateName);

  ds::DebugLayer::instance().clear_callback();
}

TEST(DebugLayerBuilderTest, IncompatibleCapsDispatchesCallback) {
  std::vector<ds::DebugMessage> received;
  ds::DebugLayer::instance().set_callback([&received](const ds::DebugMessage& m) { received.push_back(m); });

  GstElement* audio = gst_element_factory_make("audiotestsrc", nullptr);
  GstElement* video = gst_element_factory_make("videoconvert", nullptr);
  ASSERT_NE(audio, nullptr);
  ASSERT_NE(video, nullptr);

  auto result = ds::Builder{}.add(gst::Element{audio}).add(gst::Element{video}).build();

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error().kind, ds::ErrorKind::IncompatibleCaps);

  ASSERT_GE(received.size(), 1u);
  EXPECT_EQ(received.back().kind, ds::ErrorKind::IncompatibleCaps);

  ds::DebugLayer::instance().clear_callback();
}

// ============================================================================
// ElementError from ds:: factories
// ============================================================================

TEST(ElementErrorTest, FileSinkErrorKind) {
  auto result = ds::FileSink::create();
  ASSERT_TRUE(result.has_value()) << "filesink must be available";
  (void)result;
}

TEST(ElementErrorTest, WindowSinkAbsenceSetsKind) {
  if(gst_element_factory_find("nveglglessink") != nullptr) {
    GTEST_SKIP() << "nveglglessink available; skipping absence test";
  }
  auto result = ds::WindowSink::create();
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error().kind, ds::ErrorKind::ElementCreation);
  EXPECT_NE(result.error().find("nveglglessink"), std::string::npos);
}

int main(int argc, char** argv) {
  gst_init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
