/*******************************************************************************
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef __BASE_INFERENCE_H__
#define __BASE_INFERENCE_H__

<<<<<<< HEAD
#include "gva_base_inference.h"
=======
#include "classification_history.h"
#include "common/input_model_preproc.h"
#include "gstgvaclassify.h"
#include "gva_base_inference.h"

#include "feature_toggling/ifeature_toggler.h"
>>>>>>> 1bdbbc8... Release 2020.4
#include "inference_backend/image_inference.h"

#include <gst/video/video.h>

#include <list>
#include <memory>
#include <mutex>

class InferenceImpl {
  public:
    struct Model {
        std::string name;
        std::shared_ptr<InferenceBackend::ImageInference> inference;
<<<<<<< HEAD
        std::map<std::string, GstStructure *> proc;
        GstStructure *input_preproc;
=======
        std::vector<ModelInputProcessorInfo::Ptr> input_processor_info;
        std::map<std::string, GstStructure *> output_processor_info;
        std::map<std::string, GValueArray *> labels;
>>>>>>> 1bdbbc8... Release 2020.4
    };

    InferenceImpl(GvaBaseInference *gva_base_inference);

    GstFlowReturn TransformFrameIp(GvaBaseInference *element, GstBuffer *buffer, GstVideoInfo *info);
    void SinkEvent(GstEvent *event);
    void FlushInference();
    const std::vector<Model> &GetModels() const;

    ~InferenceImpl();

  private:
    struct InferenceResult : public InferenceBackend::ImageInference::IFrameBase {
<<<<<<< HEAD
        InferenceFrame inference_frame;
=======
        std::shared_ptr<InferenceFrame> inference_frame;
>>>>>>> 1bdbbc8... Release 2020.4
        Model *model;
        std::shared_ptr<InferenceBackend::Image> image;
    };

    enum InferenceStatus {
        INFERENCE_EXECUTED = 1,
        INFERENCE_SKIPPED_PER_PROPERTY = 2, // frame skipped due to inference-interval set to value greater than 1
        INFERENCE_SKIPPED_NO_BLOCK = 3,     // frame skipped due to no-block policy
        INFERENCE_SKIPPED_ROI = 4           // roi skipped because is_roi_classification_needed() returned false
    };

<<<<<<< HEAD
    void PushOutput();
    void InferenceCompletionCallback(std::map<std::string, InferenceBackend::OutputBlob::Ptr> blobs,
                                     std::vector<std::shared_ptr<InferenceBackend::ImageInference::IFrameBase>> frames);
    Model CreateModel(GvaBaseInference *gva_base_inference, std::shared_ptr<InferenceBackend::Allocator> &allocator,
                      const std::string &model_file, const std::string &model_proc_path);

    GstFlowReturn SubmitImages(GvaBaseInference *gva_base_inference,
                               const std::vector<GstVideoRegionOfInterestMeta *> &metas, GstVideoInfo *info,
                               GstBuffer *buffer);
    std::shared_ptr<InferenceResult> MakeInferenceResult(GvaBaseInference *gva_base_inference, Model &model,
                                                         GstVideoRegionOfInterestMeta *meta,
                                                         std::shared_ptr<InferenceBackend::Image> &image,
                                                         GstBuffer *buffer);
    RoiPreProcessorFunction GetPreProcFunction(GvaBaseInference *gva_base_inference, GstStructure *input_preproc,
                                               GstVideoRegionOfInterestMeta *meta);

=======
>>>>>>> 1bdbbc8... Release 2020.4
    mutable std::mutex _mutex;
    int frame_num;
    std::vector<Model> models;
    std::shared_ptr<InferenceBackend::Allocator> allocator;
<<<<<<< HEAD
=======
    std::unique_ptr<FeatureToggling::Base::IFeatureToggler> feature_toggler;
>>>>>>> 1bdbbc8... Release 2020.4

    struct OutputFrame {
        GstBuffer *buffer;
        GstBuffer *writable_buffer;
        int inference_count;
        GvaBaseInference *filter;
<<<<<<< HEAD
=======
        std::vector<std::shared_ptr<InferenceFrame>> inference_rois;
>>>>>>> 1bdbbc8... Release 2020.4
    };

    std::list<OutputFrame> output_frames;
    std::mutex output_frames_mutex;
<<<<<<< HEAD
=======

    void PushOutput();
    void PushBufferToSrcPad(OutputFrame &output_frame);
    void PushFramesIfInferenceFailed(std::vector<std::shared_ptr<InferenceBackend::ImageInference::IFrameBase>> frames);
    void InferenceCompletionCallback(std::map<std::string, InferenceBackend::OutputBlob::Ptr> blobs,
                                     std::vector<std::shared_ptr<InferenceBackend::ImageInference::IFrameBase>> frames);
    Model CreateModel(GvaBaseInference *gva_base_inference, std::shared_ptr<InferenceBackend::Allocator> &allocator,
                      const std::string &model_file, const std::string &model_proc_path);

    GstFlowReturn SubmitImages(GvaBaseInference *gva_base_inference,
                               const std::vector<GstVideoRegionOfInterestMeta *> &metas, GstVideoInfo *info,
                               GstBuffer *buffer);
    std::shared_ptr<InferenceResult> MakeInferenceResult(GvaBaseInference *gva_base_inference, Model &model,
                                                         GstVideoRegionOfInterestMeta *meta,
                                                         std::shared_ptr<InferenceBackend::Image> &image,
                                                         GstBuffer *buffer);
>>>>>>> 1bdbbc8... Release 2020.4
};

#endif /* __BASE_INFERENCE_H__ */
