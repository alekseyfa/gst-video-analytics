/*******************************************************************************
<<<<<<< HEAD
 * Copyright (C) 2018-2019 Intel Corporation
=======
 * Copyright (C) 2018-2020 Intel Corporation
>>>>>>> 1bdbbc8... Release 2020.4
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include "gstgvaclassify.h"

#include <gst/base/gstbasetransform.h>
#include <gst/gst.h>
#include <gst/video/video.h>

G_BEGIN_DECLS

struct ClassificationHistory;
struct ClassificationHistory *create_classification_history(GstGvaClassify *gva_classify);
void release_classification_history(struct ClassificationHistory *classification_history);
void fill_roi_params_from_history(struct ClassificationHistory *classification_history, GstBuffer *buffer);

G_END_DECLS

#ifdef __cplusplus
<<<<<<< HEAD
#include <map>
#include <mutex>

struct ClassificationHistory {
    struct ROIClassificationHistory {
        unsigned frame_of_last_update;
        std::map<std::string, GstStructure *> layers_to_roi_params;

        ~ROIClassificationHistory() {
            for (auto l : layers_to_roi_params)
                gst_structure_free(l.second);
=======
#include "gst_smart_pointer_types.hpp"
#include "lru_cache.h"

#include <map>
#include <mutex>

const size_t CLASSIFICATION_HISTORY_SIZE = 100;

struct ClassificationHistory {
    struct ROIClassificationHistory {
        unsigned frame_of_last_update;
        std::map<std::string, GstStructureSharedPtr> layers_to_roi_params;

        ROIClassificationHistory(unsigned frame_of_last_update = {},
                                 std::map<std::string, GstStructureSharedPtr> layers_to_roi_params = {})
            : frame_of_last_update(frame_of_last_update), layers_to_roi_params(layers_to_roi_params) {
>>>>>>> 1bdbbc8... Release 2020.4
        }
    };

    GstGvaClassify *gva_classify;
    unsigned current_num_frame;
<<<<<<< HEAD
    std::map<int, ROIClassificationHistory> history;
=======
    LRUCache<int, ROIClassificationHistory> history;
>>>>>>> 1bdbbc8... Release 2020.4
    std::mutex history_mutex;

    ClassificationHistory(GstGvaClassify *gva_classify);

    bool IsROIClassificationNeeded(GstVideoRegionOfInterestMeta *roi, unsigned current_num_frame);
<<<<<<< HEAD
    void UpdateROIParams(int roi_id, GstStructure *roi_param);
=======
    void UpdateROIParams(int roi_id, const GstStructure *roi_param);
>>>>>>> 1bdbbc8... Release 2020.4
    void FillROIParams(GstBuffer *buffer);
};
#endif
