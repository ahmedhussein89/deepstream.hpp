#include <cstring>

#include <glib.h>
#include <gtest/gtest.h>

#include <metadata.hpp>

// ============================================================================
// Helpers — build a GList (NvDsMetaList) from stack-allocated nodes
// ============================================================================
namespace {

// Appends ptr to *list via g_list_append and returns the new head.
template <typename T>
GList* append(GList* list, T* ptr) {
  return g_list_append(list, static_cast<gpointer>(ptr));
}

}    // namespace

// ============================================================================
// BoundingBox
// ============================================================================

TEST(BoundingBoxTest, DerivedGeometry) {
  const ds::BoundingBox box{10.f, 20.f, 100.f, 50.f};
  EXPECT_FLOAT_EQ(box.right(), 110.f);
  EXPECT_FLOAT_EQ(box.bottom(), 70.f);
  EXPECT_FLOAT_EQ(box.area(), 5000.f);
}

// ============================================================================
// LabelInfoView
// ============================================================================

TEST(LabelInfoViewTest, Fields) {
  NvDsLabelInfo info{};
  std::strncpy(info.result_label, "cat", sizeof(info.result_label) - 1);
  info.result_class_id = 3;
  info.result_prob = 0.97f;
  info.label_id = 1;

  const ds::LabelInfoView view{&info};
  EXPECT_EQ(view.label(), "cat");
  EXPECT_EQ(view.class_id(), 3);
  EXPECT_FLOAT_EQ(view.probability(), 0.97f);
  EXPECT_EQ(view.label_id(), 1u);
  EXPECT_EQ(view.get(), &info);
}

// ============================================================================
// ClassifierMetaView
// ============================================================================

TEST(ClassifierMetaViewTest, LabelIteration) {
  NvDsLabelInfo label0{};
  std::strncpy(label0.result_label, "dog", sizeof(label0.result_label) - 1);
  label0.result_class_id = 0;
  label0.result_prob = 0.9f;

  NvDsLabelInfo label1{};
  std::strncpy(label1.result_label, "cat", sizeof(label1.result_label) - 1);
  label1.result_class_id = 1;
  label1.result_prob = 0.1f;

  GList* labels = nullptr;
  labels = append(labels, &label0);
  labels = append(labels, &label1);

  NvDsClassifierMeta cm{};
  cm.label_info_list = labels;
  cm.unique_component_id = 7;
  cm.classifier_type = "primary";

  const ds::ClassifierMetaView view{&cm};
  EXPECT_EQ(view.unique_component_id(), 7);
  EXPECT_EQ(view.classifier_type(), "primary");

  int count = 0;
  for(const auto lv : view.labels()) {
    ++count;
    (void)lv;
  }
  EXPECT_EQ(count, 2);

  auto it = view.labels().begin();
  EXPECT_EQ((*it).label(), "dog");
  ++it;
  EXPECT_EQ((*it).label(), "cat");

  g_list_free(labels);
}

TEST(ClassifierMetaViewTest, EmptyLabels) {
  NvDsClassifierMeta cm{};
  cm.label_info_list = nullptr;

  const ds::ClassifierMetaView view{&cm};
  EXPECT_TRUE(view.labels().empty());
  int count = 0;
  for(const auto lv : view.labels()) {
    ++count;
    (void)lv;
  }
  EXPECT_EQ(count, 0);
}

// ============================================================================
// ObjectMetaView
// ============================================================================

TEST(ObjectMetaViewTest, BasicFields) {
  NvDsObjectMeta obj{};
  obj.class_id = 2;
  obj.object_id = 42ULL;
  obj.confidence = 0.88f;
  obj.tracker_confidence = 0.75f;
  std::strncpy(obj.obj_label, "vehicle", sizeof(obj.obj_label) - 1);
  obj.unique_component_id = 5;
  obj.rect_params.left = 10.f;
  obj.rect_params.top = 20.f;
  obj.rect_params.width = 80.f;
  obj.rect_params.height = 60.f;

  const ds::ObjectMetaView view{&obj};
  EXPECT_EQ(view.class_id(), 2);
  EXPECT_EQ(view.object_id(), 42ULL);
  EXPECT_FLOAT_EQ(view.confidence(), 0.88f);
  EXPECT_FLOAT_EQ(view.tracker_confidence(), 0.75f);
  EXPECT_EQ(view.label(), "vehicle");
  EXPECT_EQ(view.unique_component_id(), 5);
  EXPECT_EQ(view.get(), &obj);

  const auto box = view.rect();
  EXPECT_FLOAT_EQ(box.left, 10.f);
  EXPECT_FLOAT_EQ(box.top, 20.f);
  EXPECT_FLOAT_EQ(box.width, 80.f);
  EXPECT_FLOAT_EQ(box.height, 60.f);
  EXPECT_FLOAT_EQ(box.right(), 90.f);
  EXPECT_FLOAT_EQ(box.bottom(), 80.f);
}

TEST(ObjectMetaViewTest, ClassifierIteration) {
  NvDsClassifierMeta cm0{};
  cm0.unique_component_id = 1;
  NvDsClassifierMeta cm1{};
  cm1.unique_component_id = 2;

  GList* classifiers = nullptr;
  classifiers = append(classifiers, &cm0);
  classifiers = append(classifiers, &cm1);

  NvDsObjectMeta obj{};
  obj.classifier_meta_list = classifiers;

  const ds::ObjectMetaView view{&obj};
  int count = 0;
  for(const auto cv : view.classifiers()) {
    ++count;
    (void)cv;
  }
  EXPECT_EQ(count, 2);

  g_list_free(classifiers);
}

TEST(ObjectMetaViewTest, EmptyClassifiers) {
  NvDsObjectMeta obj{};
  obj.classifier_meta_list = nullptr;
  const ds::ObjectMetaView view{&obj};
  EXPECT_TRUE(view.classifiers().empty());
}

// ============================================================================
// FrameMetaView
// ============================================================================

TEST(FrameMetaViewTest, BasicFields) {
  NvDsFrameMeta frame{};
  frame.batch_id = 0;
  frame.pad_index = 1;
  frame.frame_num = 100;
  frame.buf_pts = 999ULL;
  frame.num_obj_meta = 3;
  frame.source_id = 2;

  const ds::FrameMetaView view{&frame};
  EXPECT_EQ(view.batch_id(), 0u);
  EXPECT_EQ(view.pad_index(), 1u);
  EXPECT_EQ(view.frame_num(), 100);
  EXPECT_EQ(view.buf_pts(), 999ULL);
  EXPECT_EQ(view.num_objects(), 3u);
  EXPECT_EQ(view.source_id(), 2u);
  EXPECT_EQ(view.get(), &frame);
}

TEST(FrameMetaViewTest, ObjectIteration) {
  NvDsObjectMeta o0{};
  o0.class_id = 0;
  NvDsObjectMeta o1{};
  o1.class_id = 1;
  NvDsObjectMeta o2{};
  o2.class_id = 2;

  GList* objs = nullptr;
  objs = append(objs, &o0);
  objs = append(objs, &o1);
  objs = append(objs, &o2);

  NvDsFrameMeta frame{};
  frame.obj_meta_list = objs;

  const ds::FrameMetaView view{&frame};
  int count = 0;
  std::int32_t id = 0;
  for(const auto ov : view.objects()) {
    EXPECT_EQ(ov.class_id(), id++);
    ++count;
  }
  EXPECT_EQ(count, 3);

  g_list_free(objs);
}

TEST(FrameMetaViewTest, EmptyObjects) {
  NvDsFrameMeta frame{};
  frame.obj_meta_list = nullptr;
  const ds::FrameMetaView view{&frame};
  EXPECT_TRUE(view.objects().empty());
}

// ============================================================================
// BatchMetaView
// ============================================================================

TEST(BatchMetaViewTest, FrameIteration) {
  NvDsFrameMeta f0{};
  f0.batch_id = 0;
  NvDsFrameMeta f1{};
  f1.batch_id = 1;

  GList* frames = nullptr;
  frames = append(frames, &f0);
  frames = append(frames, &f1);

  NvDsBatchMeta batch{};
  batch.frame_meta_list = frames;

  const ds::BatchMetaView view{&batch};
  int count = 0;
  for(const auto fv : view.frames()) {
    EXPECT_EQ(fv.batch_id(), static_cast<std::uint32_t>(count));
    ++count;
  }
  EXPECT_EQ(count, 2);

  g_list_free(frames);
}

TEST(BatchMetaViewTest, EmptyFrames) {
  NvDsBatchMeta batch{};
  batch.frame_meta_list = nullptr;
  const ds::BatchMetaView view{&batch};
  EXPECT_TRUE(view.frames().empty());
  EXPECT_EQ(view.get(), &batch);
}

// ============================================================================
// TensorLayerView / TensorMetaView
// ============================================================================

TEST(TensorLayerViewTest, Fields) {
  NvDsInferLayerInfo info{};
  info.layerName = "output";
  info.dataType = FLOAT;
  info.inferDims.numDims = 3;
  info.inferDims.d[0] = 1;
  info.inferDims.d[1] = 80;
  info.inferDims.d[2] = 80;
  info.inferDims.numElements = 6400;

  const ds::TensorLayerView view{&info};
  EXPECT_EQ(view.name(), "output");
  EXPECT_EQ(view.data_type(), FLOAT);
  EXPECT_EQ(view.num_dims(), 3u);
  EXPECT_EQ(view.num_elements(), 6400u);

  const auto shape = view.shape();
  EXPECT_EQ(shape.size(), 3u);
  EXPECT_EQ(shape[0], 1u);
  EXPECT_EQ(shape[1], 80u);
  EXPECT_EQ(shape[2], 80u);
}

TEST(TensorMetaViewTest, OutputLayerIteration) {
  NvDsInferLayerInfo layers[2]{};
  layers[0].layerName = "cls";
  layers[0].dataType = FLOAT;
  layers[1].layerName = "bbox";
  layers[1].dataType = FLOAT;

  NvDsInferTensorMeta meta{};
  meta.unique_id = 1;
  meta.num_output_layers = 2;
  meta.output_layers_info = layers;
  meta.gpu_id = 0;

  const ds::TensorMetaView view{&meta};
  EXPECT_EQ(view.unique_id(), 1);
  EXPECT_EQ(view.num_output_layers(), 2u);
  EXPECT_EQ(view.gpu_id(), 0);

  EXPECT_EQ(view.output_layer(0).name(), "cls");
  EXPECT_EQ(view.output_layer(1).name(), "bbox");

  int count = 0;
  for(const auto lv : view.output_layers()) {
    ++count;
    (void)lv;
  }
  EXPECT_EQ(count, 2);

  EXPECT_FALSE(view.output_layers().empty());
  EXPECT_EQ(view.output_layers().size(), 2u);
}

TEST(TensorMetaViewTest, EmptyLayers) {
  NvDsInferTensorMeta meta{};
  meta.num_output_layers = 0;
  meta.output_layers_info = nullptr;

  const ds::TensorMetaView view{&meta};
  EXPECT_EQ(view.num_output_layers(), 0u);
  EXPECT_TRUE(view.output_layers().empty());
}

// ============================================================================
// UserMetaView
// ============================================================================

TEST(UserMetaViewTest, NonTensorType) {
  NvDsUserMeta um{};
  um.base_meta.meta_type = NVDS_BATCH_META;
  um.user_meta_data = nullptr;

  const ds::UserMetaView view{&um};
  EXPECT_EQ(view.meta_type(), NVDS_BATCH_META);
  EXPECT_FALSE(view.as_tensor_meta().has_value());
  EXPECT_EQ(view.raw_data(), nullptr);
  EXPECT_EQ(view.get(), &um);
}

TEST(UserMetaViewTest, TensorType) {
  NvDsInferLayerInfo layer{};
  layer.layerName = "scores";
  layer.dataType = FLOAT;
  layer.inferDims.numDims = 1;
  layer.inferDims.d[0] = 80;
  layer.inferDims.numElements = 80;

  NvDsInferTensorMeta tensor{};
  tensor.unique_id = 42;
  tensor.num_output_layers = 1;
  tensor.output_layers_info = &layer;

  NvDsUserMeta um{};
  um.base_meta.meta_type = NVDSINFER_TENSOR_OUTPUT_META;
  um.user_meta_data = &tensor;

  const ds::UserMetaView view{&um};
  EXPECT_EQ(view.meta_type(), NVDSINFER_TENSOR_OUTPUT_META);

  auto opt_tensor = view.as_tensor_meta();
  ASSERT_TRUE(opt_tensor.has_value());
  EXPECT_EQ(opt_tensor->unique_id(), 42);
  EXPECT_EQ(opt_tensor->num_output_layers(), 1u);
  EXPECT_EQ(opt_tensor->output_layer(0).name(), "scores");
}

// ============================================================================
// MetaListView range semantics
// ============================================================================

TEST(MetaListViewTest, ForwardIteratorPostIncrement) {
  NvDsObjectMeta o0{};
  o0.class_id = 10;
  NvDsObjectMeta o1{};
  o1.class_id = 20;

  GList* list = nullptr;
  list = append(list, &o0);
  list = append(list, &o1);

  NvDsFrameMeta frame{};
  frame.obj_meta_list = list;
  const ds::FrameMetaView view{&frame};

  auto it = view.objects().begin();
  const auto first = *it++;    // post-increment
  EXPECT_EQ(first.class_id(), 10);
  EXPECT_EQ((*it).class_id(), 20);

  g_list_free(list);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
