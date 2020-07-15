/*******************************************************************************
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#ifdef __cplusplus

<<<<<<< HEAD
=======
#include "common/input_model_preproc.h"
>>>>>>> 1bdbbc8... Release 2020.4
#include "inference_backend/image_inference.h"

#include <gst/gst.h>
#include <gst/video/video.h>

#include <functional>

struct _GvaBaseInference;
typedef struct _GvaBaseInference GvaBaseInference;

struct InferenceFrame {
    GstBuffer *buffer;
<<<<<<< HEAD
    GstVideoInfo *info;
    GstVideoRegionOfInterestMeta roi;

    GvaBaseInference *gva_base_inference;
    InferenceFrame() : roi() {
        buffer = nullptr;
        info = nullptr;
        gva_base_inference = nullptr;
    }
    InferenceFrame(GstBuffer *_buff, GstVideoInfo *_info, GstVideoRegionOfInterestMeta _roi, GvaBaseInference *_gbi) {
        buffer = _buff;
        roi = _roi;
        gva_base_inference = _gbi;
        info = (_info) ? gst_video_info_copy(_info) : nullptr;
    }
    InferenceFrame(const InferenceFrame &inf) {
        buffer = inf.buffer;
        roi = inf.roi;
        gva_base_inference = inf.gva_base_inference;
=======
    GstVideoRegionOfInterestMeta roi;
    std::vector<GstStructure *> roi_classifications; // length equals to output layers count
    GvaBaseInference *gva_base_inference;
    GstVideoInfo *info;

    InferenceFrame() = default;
    InferenceFrame(GstBuffer *_buf, GstVideoRegionOfInterestMeta _roi, std::vector<GstStructure *> _roi_classifications,
                   GvaBaseInference *_gva_base_inference, GstVideoInfo *_info)
        : buffer(_buf), roi(_roi), roi_classifications(_roi_classifications), gva_base_inference(_gva_base_inference) {
        info = (_info) ? gst_video_info_copy(_info) : nullptr;
    }
    InferenceFrame(const InferenceFrame &inf)
        : buffer(inf.buffer), roi(inf.roi), roi_classifications(inf.roi_classifications),
          gva_base_inference(inf.gva_base_inference) {
>>>>>>> 1bdbbc8... Release 2020.4
        this->info = (inf.info) ? gst_video_info_copy(inf.info) : nullptr;
    }
    InferenceFrame &operator=(const InferenceFrame &rhs) {
        buffer = rhs.buffer;
        roi = rhs.roi;
<<<<<<< HEAD
=======
        roi_classifications = rhs.roi_classifications;
>>>>>>> 1bdbbc8... Release 2020.4
        gva_base_inference = rhs.gva_base_inference;
        if (this->info) {
            gst_video_info_free(this->info);
            this->info = nullptr;
        }
        if (rhs.info)
            this->info = gst_video_info_copy(rhs.info);

        return *this;
    }
    ~InferenceFrame() {
        if (info) {
            gst_video_info_free(info);
            info = nullptr;
        }
    }
};

<<<<<<< HEAD
using RoiPreProcessorFunction = std::function<void(InferenceBackend::Image &)>;

typedef void (*PreProcFunction)(GstStructure *preproc, InferenceBackend::Image &image);

typedef RoiPreProcessorFunction (*GetROIPreProcFunction)(GstStructure *preproc, GstVideoRegionOfInterestMeta *roi_meta);

typedef void (*PostProcFunction)(const std::map<std::string, InferenceBackend::OutputBlob::Ptr> &output_blobs,
                                 std::vector<InferenceFrame> frames,
                                 const std::map<std::string, GstStructure *> &model_proc, const gchar *model_name);

typedef bool (*IsROIClassificationNeededFunction)(GvaBaseInference *gva_base_inference, guint current_num_frame,
                                                  GstBuffer *buffer, GstVideoRegionOfInterestMeta *roi);

#else // __cplusplus

typedef void *PreProcFunction;
typedef void *PostProcFunction;
typedef void *GetROIPreProcFunction;
=======
using InputPreprocessingFunction = std::function<void(const InferenceBackend::InputBlob::Ptr &)>;

typedef InputPreprocessingFunction (*InputPreprocessingFunctionGetter)(
    const std::shared_ptr<InferenceBackend::ImageInference> &inference, GstStructure *preproc,
    GstVideoRegionOfInterestMeta *roi_meta);
typedef std::map<std::string, InferenceBackend::InputLayerDesc::Ptr> (*InputPreprocessorsFactory)(
    const std::shared_ptr<InferenceBackend::ImageInference> &inference,
    const std::vector<ModelInputProcessorInfo::Ptr> &model_input_processor_info, GstVideoRegionOfInterestMeta *roi);
typedef void (*PreProcFunction)(GstStructure *preproc, InferenceBackend::Image &image);
typedef bool (*IsROIClassificationNeededFunction)(GvaBaseInference *gva_base_inference, guint current_num_frame,
                                                  GstBuffer *buffer, GstVideoRegionOfInterestMeta *roi);

#include "../common/post_processor.h"

#else // __cplusplus

typedef void *PreProcFunction;
typedef void *InputPreprocessorsFactory;
typedef void *InputPreprocessingFunctionGetter;
typedef struct PostProcessor PostProcessor;
>>>>>>> 1bdbbc8... Release 2020.4
typedef void *IsROIClassificationNeededFunction;

#endif // __cplusplus
