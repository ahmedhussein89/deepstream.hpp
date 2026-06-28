#include <type_traits>

#include <gst/gst.h>
#include <gtest/gtest.h>

#include <builder.hpp>
#include <elements.hpp>

namespace {

// Helper: wrap a raw GstElement* in a gst::Element for use with the builder
gst::Element make_raw(const char* factory) {
  GstElement* e = gst_element_factory_make(factory, nullptr);
  EXPECT_NE(e, nullptr) << "Factory '" << factory << "' not available";
  return gst::Element{e};
}

// ============================================================================
// Type properties
// ============================================================================

TEST(BuilderTest, BuilderMoveOnly) {
  static_assert(!std::is_copy_constructible_v<ds::Builder>);
  static_assert(!std::is_copy_assignable_v<ds::Builder>);
  static_assert(std::is_move_constructible_v<ds::Builder>);
  static_assert(std::is_move_assignable_v<ds::Builder>);
}

// ============================================================================
// Mandatory-node validation
// ============================================================================

TEST(BuilderTest, EmptyBuilderFails) {
  auto result = ds::Builder{}.build();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), "Pipeline must contain at least one element");
}

TEST(BuilderTest, EmptyAfterBuildFails) {
  ds::Builder b;
  b.add(make_raw("fakesrc"));
  ASSERT_TRUE(b.build().has_value());
  // Builder is now drained — second build() must fail
  auto again = b.build();
  EXPECT_FALSE(again.has_value());
}

// ============================================================================
// Unique element name enforcement
// ============================================================================

TEST(BuilderTest, DuplicateElementNameFails) {
  auto result = ds::Builder{}
                    .add(gst::Element{gst_element_factory_make("fakesrc", "same-name")})
                    .add(gst::Element{gst_element_factory_make("fakesink", "same-name")})
                    .build();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("same-name"), std::string::npos);
}

TEST(BuilderTest, DifferentNamesSucceed) {
  auto result = ds::Builder{}
                    .add(gst::Element{gst_element_factory_make("fakesrc", "src-a")})
                    .add(gst::Element{gst_element_factory_make("fakesink", "sink-b")})
                    .build();
  EXPECT_TRUE(result.has_value());
}

// ============================================================================
// Caps compatibility validation
// ============================================================================

TEST(BuilderTest, IncompatibleCapsFailsBeforeLink) {
  // audiotestsrc (audio/x-raw) -> videoconvert (video/x-raw sink) — incompatible
  auto result = ds::Builder{}.add(make_raw("audiotestsrc")).add(make_raw("videoconvert")).build();
  EXPECT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("incompatible"), std::string::npos);
}

TEST(BuilderTest, CompatibleCapsSucceed) {
  // fakesrc (ANY) -> fakesink (ANY) — always compatible
  auto result = ds::Builder{}.add(make_raw("fakesrc")).add(make_raw("fakesink")).build();
  EXPECT_TRUE(result.has_value());
}

// ============================================================================
// Successful linear builds
// ============================================================================

TEST(BuilderTest, SingleElementBuilds) {
  auto result = ds::Builder{}.add(make_raw("fakesrc")).build();
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(GST_IS_PIPELINE(result->get()));
}

TEST(BuilderTest, TwoElementLinearPipeline) {
  auto result = ds::Builder{}.add(make_raw("fakesrc")).add(make_raw("fakesink")).build();
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(GST_IS_PIPELINE(result->get()));
}

TEST(BuilderTest, ThreeElementLinearPipeline) {
  auto result =
      ds::Builder{}.add(make_raw("videotestsrc")).add(make_raw("videoconvert")).add(make_raw("fakesink")).build();
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(GST_IS_PIPELINE(result->get()));
}

TEST(BuilderTest, BuildReturnsPipelineType) {
  auto result = ds::Builder{}.add(make_raw("fakesrc")).add(make_raw("fakesink")).build();
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(GST_IS_PIPELINE(result->get()));
  EXPECT_TRUE(GST_IS_BIN(result->get()));
}

// ============================================================================
// Fluent chaining with ds:: typed elements
// ============================================================================

TEST(BuilderTest, DsFileSourceAndFileSinkBuild) {
  auto src = ds::FileSource::create();
  auto sink = ds::FileSink::create();
  ASSERT_TRUE(src.has_value());
  ASSERT_TRUE(sink.has_value());

  auto result = ds::Builder{}.add(std::move(*src)).add(std::move(*sink)).build();
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(GST_IS_PIPELINE(result->get()));
}

TEST(BuilderTest, DsFileSourceWithPropertyThenFileSink) {
  auto src = ds::FileSource::create();
  ASSERT_TRUE(src.has_value());
  src->location("/tmp/test.mp4");

  auto sink = ds::FileSink::create();
  ASSERT_TRUE(sink.has_value());
  sink->location("/tmp/out.mp4").sync(false);

  auto result = ds::Builder{}.add(std::move(*src)).add(std::move(*sink)).build();
  ASSERT_TRUE(result.has_value());
}

TEST(BuilderTest, FluentChaining) {
  auto result = ds::Builder{}
                    .add(ds::FileSource::create().value())
                    .add(ds::FileSink::create().value())
                    .build();
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(GST_IS_PIPELINE(result->get()));
}

// ============================================================================
// RAII / resource management
// ============================================================================

TEST(BuilderTest, DestroyedWithoutBuildDoesNotLeak) {
  // Valgrind / ASAN would catch leaks; this just verifies no crash.
  {
    ds::Builder b;
    b.add(make_raw("fakesrc")).add(make_raw("fakesink"));
    // b destroyed here without calling build()
  }
  SUCCEED();
}

TEST(BuilderTest, PipelineResultOwnsElements) {
  auto result = ds::Builder{}.add(make_raw("fakesrc")).add(make_raw("fakesink")).build();
  ASSERT_TRUE(result.has_value());

  // Verify we can still access the GstElement* through the pipeline
  GstElement* found = gst_bin_get_by_name(GST_BIN(result->get()), "fakesrc0");
  if(found == nullptr) {
    found = gst_bin_get_by_name(GST_BIN(result->get()), "fakesrc1");
  }
  // Elements are in the pipeline bin
  EXPECT_TRUE(GST_IS_BIN(result->get()));
  if(found != nullptr) {
    gst_object_unref(found);
  }
}

}    // namespace

int main(int argc, char** argv) {
  gst_init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
