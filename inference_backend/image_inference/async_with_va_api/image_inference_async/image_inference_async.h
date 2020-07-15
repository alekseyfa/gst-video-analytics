/*******************************************************************************
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include "config.h"

#include "thread_pool.h"

#include "inference_backend/image_inference.h"
#include "inference_backend/logger.h"
#include "inference_backend/pre_proc.h"

#include <map>

namespace InferenceBackend {

class VaApiConverter;
class VaApiImagePool;
class VaApiImage;
class VaApiContext;

class ImageInferenceAsync : public ImageInference {
  public:
    ImageInferenceAsync(ImageInference::Ptr &&inference, int image_pool_size);

    ~ImageInferenceAsync() override;

<<<<<<< HEAD
    void SubmitImage(const Image &image, IFramePtr user_data, std::function<void(Image &)> pre_rocessor) override;

    const std::string &GetModelName() const override;

    void GetModelInputInfo(int *width, int *height, int *batch_size, int *format) const override;
=======
    void SubmitImage(const Image &image, IFramePtr user_data,
                     const std::map<std::string, InputLayerDesc::Ptr> &input_preprocessors) override;

    const std::string &GetModelName() const override;

    void GetModelImageInputInfo(size_t &width, size_t &height, size_t &batch_size, int &format) const override;
>>>>>>> 1bdbbc8... Release 2020.4

    bool IsQueueFull() override;

    void Flush() override;

    void Close() override;

  private:
    ImageInference::Ptr inference;
    std::unique_ptr<VaApiContext> _va_context;
    std::unique_ptr<VaApiConverter> _va_converter;
    std::unique_ptr<VaApiImagePool> _va_image_pool;
    ThreadPool _thread_pool;
    const size_t _VA_IMAGE_POOL_SIZE;

    // mutex for inference

<<<<<<< HEAD
    void SubmitInference(VaApiImage *va_api_image, IFramePtr user_data, std::function<void(Image &)> pre_processor);
=======
    void SubmitInference(VaApiImage *va_api_image, IFramePtr user_data,
                         const std::map<std::string, InputLayerDesc::Ptr> &input_preprocessors);
>>>>>>> 1bdbbc8... Release 2020.4
};

} // namespace InferenceBackend
