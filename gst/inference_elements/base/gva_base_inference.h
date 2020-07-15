/*******************************************************************************
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include "config.h"

#include "inference_singleton.h"
#include "processor_types.h"

#include <gst/base/gstbasetransform.h>
#include <gst/gst.h>
#include <gst/video/video.h>

G_BEGIN_DECLS

#define GST_TYPE_GVA_BASE_INFERENCE (gva_base_inference_get_type())
#define GVA_BASE_INFERENCE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_GVA_BASE_INFERENCE, GvaBaseInference))
<<<<<<< HEAD
=======
#define GVA_BASE_INFERENCE_CLASS(klass)                                                                                \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_GVA_BASE_INFERENCE, GvaBaseInferenceClass))
#define GVA_BASE_INFERENCE_GET_CLASS(obj)                                                                              \
    (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_GVA_BASE_INFERENCE, GvaBaseInferenceClass))

typedef void (*OnBaseInferenceInitializedFunction)(GvaBaseInference *base_inference);
>>>>>>> 1bdbbc8... Release 2020.4

typedef struct _GvaBaseInference {
    GstBaseTransform base_transform;

    // properties
    gchar *model;
    gchar *model_proc;
    gchar *device;
    guint inference_interval;
    gboolean reshape;
    guint batch_size;
    guint reshape_width;
    guint reshape_height;
    gboolean no_block;
    guint nireq;
    gchar *model_instance_id;
    guint cpu_streams;
    guint gpu_streams;
    gchar *ie_config;
    gchar *allocator_name;
    gchar *pre_proc_name;
    gchar *device_extensions;

    // other fields
    GstVideoInfo *info;
    gboolean is_full_frame;

    InferenceImpl *inference;

    IsROIClassificationNeededFunction is_roi_classification_needed;
    PreProcFunction pre_proc;
<<<<<<< HEAD
    GetROIPreProcFunction get_roi_pre_proc;
    PostProcFunction post_proc;
=======
    InputPreprocessorsFactory input_prerocessors_factory;
    PostProcessor *post_proc;
>>>>>>> 1bdbbc8... Release 2020.4

    gboolean initialized;
    guint num_skipped_frames;
} GvaBaseInference;

typedef struct _GvaBaseInferenceClass {
    GstBaseTransformClass base_transform_class;
<<<<<<< HEAD
=======

    OnBaseInferenceInitializedFunction on_initialized;
>>>>>>> 1bdbbc8... Release 2020.4
} GvaBaseInferenceClass;

GType gva_base_inference_get_type(void);
gboolean check_gva_base_inference_stopped(GvaBaseInference *base_inference);

G_END_DECLS
