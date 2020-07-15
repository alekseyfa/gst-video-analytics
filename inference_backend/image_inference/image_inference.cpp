/*******************************************************************************
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "image_inference_async/image_inference_async.h"
#include "openvino_image_inference.h"

using namespace InferenceBackend;

ImageInference::Ptr ImageInference::make_shared(MemoryType /*type*/, const std::string &model,
                                                const std::map<std::string, std::map<std::string, std::string>> &config,
<<<<<<< HEAD
                                                Allocator *allocator, CallbackFunc callback) {
=======
                                                Allocator *allocator, CallbackFunc callback,
                                                ErrorHandlingFunc error_handler) {
>>>>>>> 1bdbbc8... Release 2020.4
#ifdef HAVE_VAAPI
    const std::map<std::string, std::string> &base = config.at(KEY_BASE);
    auto it = base.find(KEY_PRE_PROCESSOR_TYPE);
    if (it != base.end() && it->second == "vaapi") {
<<<<<<< HEAD
        auto infer = std::make_shared<OpenVINOImageInference>(model, config, allocator, callback);
        return std::make_shared<ImageInferenceAsync>(std::move(infer), 5);
    }
#endif
    return std::make_shared<OpenVINOImageInference>(model, config, allocator, callback);
=======
        auto infer = std::make_shared<OpenVINOImageInference>(model, config, allocator, callback, error_handler);
        return std::make_shared<ImageInferenceAsync>(std::move(infer), 5);
    }
#endif
    return std::make_shared<OpenVINOImageInference>(model, config, allocator, callback, error_handler);
>>>>>>> 1bdbbc8... Release 2020.4
}
