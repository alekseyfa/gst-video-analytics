/*******************************************************************************
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "openvino_image_inference.h"

#include "inference_backend/logger.h"
#include "inference_backend/pre_proc.h"
#include "inference_backend/safe_arithmetic.h"
#include "model_loader.h"
<<<<<<< HEAD
=======
#include "openvino_blob_wrapper.h"
#include "utils.h"
>>>>>>> 1bdbbc8... Release 2020.4
#include "wrap_image.h"

#include <chrono>
#include <functional>
#include <ie_compound_blob.h>
#include <ie_core.hpp>
#include <inference_engine.hpp>
#include <stdio.h>
#include <thread>

namespace IE = InferenceEngine;
using namespace InferenceBackend;

namespace {

<<<<<<< HEAD
std::string CreateNestedErrorMsg(const std::exception &e, int level = 0);

InferenceEngine::InputsDataMap modelInputsInfo(InferenceEngine::ExecutableNetwork &executable_network);

std::unique_ptr<InferenceBackend::PreProc> createPreProcessor(InferenceEngine::InputsDataMap &model_inputs_info,
                                                              size_t batch_size,
                                                              const std::map<std::string, std::string> &base_config);
=======
InferenceEngine::InputsDataMap modelInputsInfo(InferenceEngine::ExecutableNetwork &executable_network);

>>>>>>> 1bdbbc8... Release 2020.4
IE::ColorFormat FormatNameToIEColorFormat(const std::string &format);
inline size_t GetTensorSize(InferenceEngine::TensorDesc desc);
inline std::vector<std::string> split(const std::string &s, char delimiter);
size_t optimalNireq(const InferenceEngine::ExecutableNetwork &executable_network);

std::tuple<InferenceEngine::Blob::Ptr, InferenceBackend::Allocator::AllocContext *>
allocateBlob(const InferenceEngine::TensorDesc &tensor_desc, Allocator *allocator);

InferenceEngine::InputsDataMap modelInputsInfo(InferenceEngine::ExecutableNetwork &executable_network) {
    InferenceEngine::ConstInputsDataMap const_model_inputs_info = executable_network.GetInputsInfo();
    InferenceEngine::InputsDataMap model_inputs_info;

    // Workaround InferenceEngine API to fill model_inputs_info with mutable pointers
    std::for_each(const_model_inputs_info.begin(), const_model_inputs_info.end(),
                  [&](std::pair<const std::string, InferenceEngine::InputInfo::CPtr> pair) {
                      model_inputs_info.emplace(pair.first,
                                                std::const_pointer_cast<InferenceEngine::InputInfo>(pair.second));
                  });
    return model_inputs_info;
}

<<<<<<< HEAD
// TODO: code below is duplicated here to avoid changes in GVA BOM. When integraion to OpenvINO is stabilized, remove
// this definition and declaration above, and apply commit d4e410ead18fb99a3ca222363a1678c757629443, which makes it
// properly. Andrey & Igor for details
std::string CreateNestedErrorMsg(const std::exception &e, int level) {
    static std::string msg = "\n";
    msg += std::string(level, ' ') + e.what() + "\n";
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception &e) {
        CreateNestedErrorMsg(e, level + 1);
    }
    return msg;
}

std::unique_ptr<InferenceBackend::PreProc> createPreProcessor(InferenceEngine::InputsDataMap &model_inputs_info,
                                                              size_t batch_size,
=======
std::unique_ptr<InferenceBackend::PreProc> createPreProcessor(InferenceEngine::InputInfo::Ptr input, size_t batch_size,
>>>>>>> 1bdbbc8... Release 2020.4
                                                              const std::map<std::string, std::string> &base_config) {
    const std::string &image_format = base_config.count(KEY_IMAGE_FORMAT) ? base_config.at(KEY_IMAGE_FORMAT) : "";
    const std::string &pre_processor_type =
        base_config.count(KEY_PRE_PROCESSOR_TYPE) ? base_config.at(KEY_PRE_PROCESSOR_TYPE) : "";
<<<<<<< HEAD

    try {
        if (model_inputs_info.size() == 0) {
            throw std::invalid_argument("Failed to get model input layers info");
        }
        auto &input = model_inputs_info.begin()->second;
        input->setPrecision(InferenceEngine::Precision::U8);

=======
    if (not input) {
        throw std::invalid_argument("Inputs is empty");
    }
    try {
>>>>>>> 1bdbbc8... Release 2020.4
        std::unique_ptr<InferenceBackend::PreProc> pre_processor;
        if (pre_processor_type == "ie") {
            if (batch_size > 1)
                throw std::runtime_error("Inference Engine preprocessing with batching is not supported");
            input->getPreProcess().setResizeAlgorithm(InferenceEngine::ResizeAlgorithm::RESIZE_BILINEAR);
            input->getPreProcess().setColorFormat(FormatNameToIEColorFormat(image_format));
        } else {
            PreProcessType preProcessorType = PreProcessType::Invalid;
            if (pre_processor_type == "opencv")
                preProcessorType = PreProcessType::OpenCV;
            else if (pre_processor_type == "vaapi")
                preProcessorType = PreProcessType::VAAPI;
            else
                throw std::invalid_argument("'" + pre_processor_type + "' pre-processor is not implemented");

            pre_processor.reset(PreProc::Create(preProcessorType));
        }
        return pre_processor;
    } catch (const std::exception &e) {
        std::throw_with_nested(
            std::runtime_error("Failed to create preprocessor of '" + pre_processor_type + "' type"));
    }
}

IE::ColorFormat FormatNameToIEColorFormat(const std::string &format) {
    static const std::map<std::string, IE::ColorFormat> formatMap{
        {"NV12", IE::ColorFormat::NV12}, {"I420", IE::ColorFormat::I420}, {"RGB", IE::ColorFormat::RGB},
        {"BGR", IE::ColorFormat::BGR},   {"RGBX", IE::ColorFormat::RGBX}, {"BGRX", IE::ColorFormat::BGRX},
        {"RGBA", IE::ColorFormat::RGBX}, {"BGRA", IE::ColorFormat::BGRX}};
    auto iter = formatMap.find(format);
    if (iter != formatMap.end()) {
        return iter->second;
    } else {
        std::string err = "Color format '" + format +
                          "' is not supported by Inference Engine preprocessing. IE::ColorFormat::RAW will be set";
        GVA_ERROR(err.c_str());
        return IE::ColorFormat::RAW;
    }
}

inline size_t GetTensorSize(InferenceEngine::TensorDesc desc) {
    // TODO: merge with GetUnbatchedSizeInBytes to avoid double implementation
    auto dims = desc.getDims();
    size_t size = 1;
    for (auto dim : dims)
        size *= dim;
    switch (desc.getPrecision()) {
    case InferenceEngine::Precision::U8:
        return size;
    case InferenceEngine::Precision::FP32:
        return size * sizeof(float);
    default:
        throw std::invalid_argument("Failed to get tensor size for tensor with " + std::to_string(desc.getPrecision()) +
                                    " InferenceEngine::Precision");
    }
}

inline std::vector<std::string> split(const std::string &s, char delimiter) {
    std::string token;
    std::istringstream tokenStream(s);
    std::vector<std::string> tokens;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::tuple<InferenceEngine::Blob::Ptr, InferenceBackend::Allocator::AllocContext *>
allocateBlob(const InferenceEngine::TensorDesc &tensor_desc, Allocator *allocator) {
    try {
        void *buffer_ptr = nullptr;
        InferenceBackend::Allocator::AllocContext *alloc_context = nullptr;
        const size_t size = GetTensorSize(tensor_desc);
        allocator->Alloc(size, buffer_ptr, alloc_context);
        InferenceEngine::Blob::Ptr blob;
        if (buffer_ptr && alloc_context) {
            switch (tensor_desc.getPrecision()) {
            case InferenceEngine::Precision::U8:
                blob = InferenceEngine::make_shared_blob<uint8_t>(tensor_desc, reinterpret_cast<uint8_t *>(buffer_ptr));
                break;
            case InferenceEngine::Precision::FP32:
                blob = InferenceEngine::make_shared_blob<float>(tensor_desc, reinterpret_cast<float *>(buffer_ptr));
                break;
            default:
                throw std::invalid_argument("Failed to create Blob: InferenceEngine::Precision " +
                                            std::to_string(tensor_desc.getPrecision()) + " is not supported");
            }
        } else {
            throw std::runtime_error("Failed to allocate memory for blob");
        }
        return std::make_tuple(blob, alloc_context);
    } catch (const std::exception &e) {
        std::throw_with_nested(std::runtime_error("Failed to allocate InferenceEngine Blob"));
    }
}

size_t optimalNireq(const InferenceEngine::ExecutableNetwork &executable_network) {
    size_t nireq = 0;
    try {
        nireq = executable_network.GetMetric(IE::Metrics::METRIC_OPTIMAL_NUMBER_OF_INFER_REQUESTS).as<unsigned int>() +
                1; // One additional for pre-processing parallelization with inference
        std::string msg = "Setting the optimal number of inference requests: nireq=" + std::to_string(nireq);
        GVA_INFO(msg.c_str());
    } catch (const std::exception &e) {
        std::string err = std::string("Failed to get optimal number of inference requests: ") + e.what() +
                          std::string("\nNumber of inference requests will fallback to 1");
        GVA_ERROR(err.c_str());
        return 1;
    }
    return nireq;
}

void addExtension(IE::Core &core, const std::map<std::string, std::string> &base_config) {
    if (base_config.count(KEY_CPU_EXTENSION)) {
        try {
            const std::string &cpu_extension = base_config.at(KEY_CPU_EXTENSION);
            const auto extension_ptr = InferenceEngine::make_so_pointer<InferenceEngine::IExtension>(cpu_extension);
            core.AddExtension(extension_ptr, "CPU");
        } catch (const std::exception &e) {
            std::throw_with_nested(std::runtime_error("Failed to add CPU extension"));
        }
    }
    if (base_config.count(KEY_GPU_EXTENSION)) {
        try {
            // TODO is core.setConfig() same with inference_config
            const std::string &config_file = base_config.at(KEY_GPU_EXTENSION);
            core.SetConfig({{InferenceEngine::PluginConfigParams::KEY_CONFIG_FILE, config_file}}, "GPU");
        } catch (const std::exception &e) {
            std::throw_with_nested(std::runtime_error("Failed to add GPU extension"));
        }
    }
    if (base_config.count(KEY_VPU_EXTENSION)) {
        try {
            const std::string &config_file = base_config.at(KEY_VPU_EXTENSION);
            core.SetConfig({{InferenceEngine::PluginConfigParams::KEY_CONFIG_FILE, config_file}}, "VPU");
        } catch (const std::exception &e) {
            std::throw_with_nested(std::runtime_error("Failed to add VPU extension"));
        }
    }
}
<<<<<<< HEAD
=======

InferenceEngine::Precision getIePrecision(InferenceBackend::Blob::Precision p) {
    InferenceEngine::Precision ie_precision;
    switch (p) {
    case InferenceBackend::Blob::Precision::FP32:
        ie_precision = InferenceEngine::Precision::FP32;
        break;
    case InferenceBackend::Blob::Precision::U8:
        ie_precision = InferenceEngine::Precision::U8;
        break;
    default:
        throw std::invalid_argument("Unsupported precision");
    }
    return ie_precision;
}

>>>>>>> 1bdbbc8... Release 2020.4
struct EntityBuilder {
    EntityBuilder() = delete;
    EntityBuilder(const EntityBuilder &) = delete;

    virtual ~EntityBuilder() = default;

    virtual InferenceEngine::CNNNetwork createNetwork(InferenceEngine::Core &core, const std::string &model) {
        return loader->load(core, model, base_config);
    };

<<<<<<< HEAD
    virtual std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork>
=======
    virtual std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork, std::string>
>>>>>>> 1bdbbc8... Release 2020.4
    createPreProcAndExecutableNetwork(InferenceEngine::CNNNetwork &network, InferenceEngine::Core &core,
                                      const std::string &model) {
        addExtension(core, base_config);
        return createPreProcAndExecutableNetwork_impl(network, core, model);
    }

    virtual std::string getNetworkName(InferenceEngine::CNNNetwork &network) {
        return loader->name(network);
    }

  private:
<<<<<<< HEAD
    virtual std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork>
=======
    virtual std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork, std::string>
>>>>>>> 1bdbbc8... Release 2020.4
    createPreProcAndExecutableNetwork_impl(InferenceEngine::CNNNetwork &network, InferenceEngine::Core &core,
                                           const std::string &model) = 0;

  protected:
    EntityBuilder(std::unique_ptr<ModelLoader> &&loader,
                  const std::map<std::string, std::map<std::string, std::string>> &config)
        : loader(std::move(loader)), base_config(config.at(KEY_BASE)), inference_config(config.at(KEY_INFERENCE)),
<<<<<<< HEAD
=======
          layer_precision_config(config.at(KEY_LAYER_PRECISION)), layer_type_config(config.at(KEY_FORMAT)),
>>>>>>> 1bdbbc8... Release 2020.4
          batch_size(std::stoi(config.at(KEY_BASE).at(KEY_BATCH_SIZE))) {
    }

    std::unique_ptr<ModelLoader> loader;
    const std::map<std::string, std::string> &base_config;
    const std::map<std::string, std::string> &inference_config;
<<<<<<< HEAD
=======
    const std::map<std::string, std::string> &layer_precision_config;
    const std::map<std::string, std::string> &layer_type_config;
>>>>>>> 1bdbbc8... Release 2020.4
    const size_t batch_size;
};

struct IrBuilder : EntityBuilder {
    IrBuilder(const std::map<std::string, std::map<std::string, std::string>> &config)
        : EntityBuilder(std::unique_ptr<ModelLoader>(new IrModelLoader()), config) {
    }

  private:
<<<<<<< HEAD
    std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork>
    createPreProcAndExecutableNetwork_impl(InferenceEngine::CNNNetwork &network, InferenceEngine::Core &core,
                                           const std::string &model) override {
        auto model_inputs_info = network.getInputsInfo();
        std::unique_ptr<InferenceBackend::PreProc> pre_processor =
            createPreProcessor(model_inputs_info, batch_size, base_config);
        InferenceEngine::ExecutableNetwork executable_network =
            loader->import(network, model, core, base_config, inference_config);
        return std::make_tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork>(
            std::move(pre_processor), std::move(executable_network));
=======
    void configureNetworkLayers(const InferenceEngine::InputsDataMap &inputs_info, std::string &image_input_name) {
        if (inputs_info.size() == 0)
            std::invalid_argument("Network inputs info is empty");

        if (inputs_info.size() == 1) {
            auto info = inputs_info.begin();
            info->second->setPrecision(InferenceEngine::Precision::U8);
            image_input_name = info->first;
        } else {
            for (auto &input_info : inputs_info) {
                auto precision_it = layer_precision_config.find(input_info.first);
                auto type_it = layer_type_config.find(input_info.first);
                if (precision_it == layer_precision_config.end() or type_it == layer_type_config.end())
                    throw std::invalid_argument(
                        "Config for layer precision does not contain precision info for layer: " + input_info.first);
                if (type_it->second == KEY_image)
                    image_input_name = input_info.first;
                auto precision = static_cast<InferenceBackend::Blob::Precision>(std::stoi(precision_it->second));
                input_info.second->setPrecision(getIePrecision(precision));
            }
        }
    }

    std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork, std::string>
    createPreProcAndExecutableNetwork_impl(InferenceEngine::CNNNetwork &network, InferenceEngine::Core &core,
                                           const std::string &model) override {
        auto inputs_info = network.getInputsInfo();
        std::string image_input_name;
        configureNetworkLayers(inputs_info, image_input_name);
        std::unique_ptr<InferenceBackend::PreProc> pre_processor =
            createPreProcessor(inputs_info[image_input_name], batch_size, base_config);
        InferenceEngine::ExecutableNetwork executable_network =
            loader->import(network, model, core, base_config, inference_config);
        return std::make_tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork,
                               std::string>(std::move(pre_processor), std::move(executable_network),
                                            std::move(image_input_name));
>>>>>>> 1bdbbc8... Release 2020.4
    }
};

struct CompiledBuilder : EntityBuilder {
    CompiledBuilder(const std::map<std::string, std::map<std::string, std::string>> &config)
        : EntityBuilder(std::unique_ptr<ModelLoader>(new CompiledModelLoader()), config) {
    }

  private:
<<<<<<< HEAD
    std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork>
=======
    std::tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork, std::string>
>>>>>>> 1bdbbc8... Release 2020.4
    createPreProcAndExecutableNetwork_impl(InferenceEngine::CNNNetwork &network, InferenceEngine::Core &core,
                                           const std::string &model) override {
        InferenceEngine::ExecutableNetwork executable_network =
            loader->import(network, model, core, base_config, inference_config);
<<<<<<< HEAD
        auto model_inputs_info = modelInputsInfo(executable_network);
        std::unique_ptr<InferenceBackend::PreProc> pre_processor =
            createPreProcessor(model_inputs_info, batch_size, base_config);
        return std::make_tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork>(
            std::move(pre_processor), std::move(executable_network));
=======
        auto inputs_info = modelInputsInfo(executable_network);
        if (inputs_info.size() > 1)
            throw std::runtime_error("Not supported models with many inputs");
        auto info = inputs_info.begin();
        InferenceEngine::InputInfo::Ptr image_input = info->second;
        std::string image_input_name = info->first;
        std::unique_ptr<InferenceBackend::PreProc> pre_processor =
            createPreProcessor(image_input, batch_size, base_config);
        return std::make_tuple<std::unique_ptr<InferenceBackend::PreProc>, InferenceEngine::ExecutableNetwork,
                               std::string>(std::move(pre_processor), std::move(executable_network),
                                            std::move(image_input_name));
>>>>>>> 1bdbbc8... Release 2020.4
    }
};

class GvaErrorListener : public InferenceEngine::IErrorListener {
    void onError(const char *msg) noexcept override {
        GVA_ERROR(msg);
    }
};

} // namespace

<<<<<<< HEAD
//////////////////////////////////////////////////////////////////////////////////

class IEOutputBlob : public OutputBlob {
  public:
    IEOutputBlob(IE::Blob::Ptr blob) : blob(blob) {
    }

    virtual const std::vector<size_t> &GetDims() const {
        return blob->getTensorDesc().getDims();
    }

    virtual Layout GetLayout() const {
        return static_cast<Layout>((int)blob->getTensorDesc().getLayout());
    }

    virtual Precision GetPrecision() const {
        return static_cast<Precision>((int)blob->getTensorDesc().getPrecision());
    }

    virtual const void *GetData() const {
        return blob->buffer();
    }

    virtual ~IEOutputBlob() {
    }

  protected:
    IE::Blob::Ptr blob;
};

//////////////////////////////////////////////////////////////////////////////////

=======
>>>>>>> 1bdbbc8... Release 2020.4
OpenVINOImageInference::~OpenVINOImageInference() {
    GVA_DEBUG("Image Inference destruct");
    Close();
}

void OpenVINOImageInference::setCompletionCallback(std::shared_ptr<BatchRequest> &batch_request) {
    auto completion_callback = [this, batch_request](InferenceEngine::InferRequest, InferenceEngine::StatusCode code) {
        try {
<<<<<<< HEAD
=======
            ITT_TASK("completion_callback_lambda");
            size_t buffer_size = batch_request->buffers.size();

>>>>>>> 1bdbbc8... Release 2020.4
            if (code != InferenceEngine::StatusCode::OK) {
                std::string msg = "Inference request completion callback failed with InferenceEngine::StatusCode: " +
                                  std::to_string(code);
                GVA_ERROR(msg.c_str());
<<<<<<< HEAD
            }
            ITT_TASK("completion_callback_lambda");
            size_t buffer_size = batch_request->buffers.size();
            this->WorkingFunction(batch_request);
            this->requests_processing_ -= buffer_size;
            this->request_processed_.notify_all();
        } catch (const std::exception &e) {
            std::string msg = "Failed in inference request completion callback:\n" + CreateNestedErrorMsg(e);
=======
                this->handleError(batch_request->buffers);
            } else {
                this->WorkingFunction(batch_request);
            }

            batch_request->buffers.clear();
            this->freeRequests.push(batch_request);
            this->requests_processing_ -= buffer_size;
            this->request_processed_.notify_all();
        } catch (const std::exception &e) {
            std::string msg = "Failed in inference request completion callback:\n" + Utils::createNestedErrorMsg(e);
>>>>>>> 1bdbbc8... Release 2020.4
            GVA_ERROR(msg.c_str());
        }
    };
    batch_request->infer_request
        ->SetCompletionCallback<std::function<void(InferenceEngine::InferRequest, InferenceEngine::StatusCode)>>(
            completion_callback);
}

void OpenVINOImageInference::setBlobsToInferenceRequest(
    const std::map<std::string, InferenceEngine::TensorDesc> &layers, std::shared_ptr<BatchRequest> &batch_request,
    Allocator *allocator) {
    for (const auto &layer : layers) {
        InferenceBackend::Allocator::AllocContext *alloc_context = nullptr;
        InferenceEngine::Blob::Ptr blob;
        std::tie(blob, alloc_context) = allocateBlob(layer.second, allocator);
        batch_request->infer_request->SetBlob(layer.first, blob);
        batch_request->alloc_context.push_back(alloc_context);
    }
}

OpenVINOImageInference::OpenVINOImageInference(const std::string &model,
                                               const std::map<std::string, std::map<std::string, std::string>> &config,
<<<<<<< HEAD
                                               Allocator *allocator, CallbackFunc callback)
=======
                                               Allocator *allocator, CallbackFunc callback,
                                               ErrorHandlingFunc error_handler)
>>>>>>> 1bdbbc8... Release 2020.4
    : allocator(allocator), batch_size(std::stoi(config.at(KEY_BASE).at(KEY_BATCH_SIZE))), requests_processing_(0U) {

    GVA_DEBUG("OpenVINOImageInference constructor");

    try {
<<<<<<< HEAD
        const std::map<std::string, std::string> &base_config = config.at(KEY_BASE);
        std::string device = base_config.at(KEY_DEVICE);
        int nireq = std::stoi(base_config.at(KEY_NIREQ));
        // const std::string &cpu_extension = base_config.count(KEY_CPU_EXTENSION) ? base_config.at(KEY_CPU_EXTENSION) :
        // "";

=======
>>>>>>> 1bdbbc8... Release 2020.4
        std::unique_ptr<EntityBuilder> builder = ModelLoader::is_ir_model(model)
                                                     ? std::unique_ptr<EntityBuilder>(new IrBuilder(config))
                                                     : std::unique_ptr<EntityBuilder>(new CompiledBuilder(config));
        if (not builder)
            throw std::runtime_error("Failed to create DL model loader");
        InferenceEngine::CNNNetwork network = builder->createNetwork(core, model);
        model_name = builder->getNetworkName(network);

        static GvaErrorListener listner;
        core.SetLogCallback(listner);

        InferenceEngine::ExecutableNetwork executable_network;
<<<<<<< HEAD
        std::tie(pre_processor, executable_network) = builder->createPreProcAndExecutableNetwork(network, core, model);
=======
        std::tie(pre_processor, executable_network, image_layer) =
            builder->createPreProcAndExecutableNetwork(network, core, model);
>>>>>>> 1bdbbc8... Release 2020.4

        inputs = executable_network.GetInputsInfo();
        outputs = executable_network.GetOutputsInfo();
        std::map<std::string, InferenceEngine::TensorDesc> layers;
        for (auto input : inputs) {
            layers[input.first] = input.second->getTensorDesc();
        }
        for (auto output : outputs) {
            layers[output.first] = output.second->getTensorDesc();
        }

<<<<<<< HEAD
=======
        const std::map<std::string, std::string> &base_config = config.at(KEY_BASE);
        int nireq = std::stoi(base_config.at(KEY_NIREQ));
>>>>>>> 1bdbbc8... Release 2020.4
        if (nireq == 0) {
            nireq = optimalNireq(executable_network);
        }

        for (int i = 0; i < nireq; i++) {
            std::shared_ptr<BatchRequest> batch_request = std::make_shared<BatchRequest>();
            batch_request->infer_request = executable_network.CreateInferRequestPtr();
            setCompletionCallback(batch_request);
            if (allocator) {
                setBlobsToInferenceRequest(layers, batch_request, allocator);
            }
            freeRequests.push(batch_request);
        }

        initialized = true;
        this->callback = callback;
<<<<<<< HEAD
=======
        this->handleError = error_handler;
>>>>>>> 1bdbbc8... Release 2020.4
    } catch (const std::exception &e) {
        std::throw_with_nested(std::runtime_error("Failed to construct OpenVINOImageInference"));
    }
}

bool OpenVINOImageInference::IsQueueFull() {
    return freeRequests.empty();
}

namespace {
template <class T>
Image FillImage(const IE::Blob::Ptr &blob, const IE::SizeVector &dims, const size_t index) {
    Image image = Image();
    image.width = dims[3];
    image.height = dims[2];
    if (index >= dims[0]) {
<<<<<<< HEAD
        // throw std::out_of_range("Image index is out of range in batch blob");
=======
        throw std::out_of_range("Image index is out of range in batch blob");
>>>>>>> 1bdbbc8... Release 2020.4
    }
    size_t plane_size = image.width * image.height * sizeof(T);
    size_t buffer_offset = safe_mul(safe_mul(index, plane_size), dims[1]);

    image.planes[0] = blob->buffer().as<uint8_t *>() + buffer_offset;
    image.planes[1] = image.planes[0] + plane_size;
    image.planes[2] = image.planes[1] + plane_size;
    image.planes[3] = nullptr;

    image.stride[0] = image.width;
    image.stride[1] = image.width;
    image.stride[2] = image.width;
    image.stride[3] = 0;
    return image;
}

<<<<<<< HEAD
} // namespace

Image OpenVINOImageInference::GetNextImageBuffer(std::shared_ptr<BatchRequest> request) {
    GVA_DEBUG(__FUNCTION__);
    ITT_TASK(__FUNCTION__);

    if (inputs.empty())
        throw std::logic_error("Inputs map is empty");
    if (not request or not request->infer_request)
        throw std::invalid_argument("InferRequest is absent");

    std::string input_name = inputs.begin()->first; // assuming one input layer
    auto blob = request->infer_request->GetBlob(input_name);
=======
Image MapBlobBufferToImage(IE::Blob::Ptr blob, size_t batch_index) {
    GVA_DEBUG(__FUNCTION__);
    ITT_TASK(__FUNCTION__);
>>>>>>> 1bdbbc8... Release 2020.4
    auto desc = blob->getTensorDesc();
    auto dims = desc.getDims();
    if (desc.getLayout() != IE::Layout::NCHW) {
        throw std::runtime_error("Unsupported layout");
    }
    Image image = Image();
    switch (desc.getPrecision()) {
    case IE::Precision::FP32:
<<<<<<< HEAD
        image = FillImage<float>(blob, desc.getDims(), request->buffers.size());
        image.format = FOURCC_RGBP_F32;
        break;
    case IE::Precision::U8:
        image = FillImage<uint8_t>(blob, desc.getDims(), request->buffers.size());
        image.format = FOURCC_RGBP;
=======
        image = FillImage<float>(blob, desc.getDims(), batch_index);
        image.format = FourCC::FOURCC_RGBP_F32;
        break;
    case IE::Precision::U8:
        image = FillImage<uint8_t>(blob, desc.getDims(), batch_index);
        image.format = FourCC::FOURCC_RGBP;
>>>>>>> 1bdbbc8... Release 2020.4
        break;
    default:
        throw std::runtime_error("Unsupported precision");
        break;
    }
    return image;
}
<<<<<<< HEAD

void OpenVINOImageInference::SubmitImageSoftwarePreProcess(std::shared_ptr<BatchRequest> request, const Image &srcImg,
                                                           std::function<void(Image &)> preProcessor) {
    if (not pre_processor.get()) {
        ITT_TASK("SubmitImageSoftwarePreProcess::ie_preproc");
        IE::Blob::Ptr frameBlob = WrapImageToBlob(srcImg);

        if (inputs.empty())
            throw std::logic_error("Inputs map is empty");

        std::string inputName = inputs.begin()->first;
        if (not request or not request->infer_request)
            throw std::invalid_argument("InferRequest is absent");
        request->infer_request->SetBlob(inputName, frameBlob);
    } else {
        ITT_TASK("SubmitImageSoftwarePreProcess::not_ie_preproc");
        Image dstImg = GetNextImageBuffer(request);
        if (srcImg.planes[0] != dstImg.planes[0]) { // only convert if different buffers
            try {
                pre_processor->Convert(srcImg, dstImg);
                // You can check preProcessor result with the following code snippet:
                // ----------------------------------------------
                // cv::Mat mat(dst.height, dst.width, CV_8UC1, dst.planes[1], dst.stride[1]);
                // static int counter;
                // cv::imwrite(std::string("") + std::to_string(counter) + "_pre" + ".png", mat);
                // preProcessor(dst);
                // cv::imwrite(std::string("") + std::to_string(counter++) + "_post" + ".png", mat);
                preProcessor(dstImg);
            } catch (const std::exception &e) {
                std::throw_with_nested(std::runtime_error("Failed while software frame preprocessing"));
            }
=======
} // namespace

void OpenVINOImageInference::SubmitImageProcessing(const std::string &input_name, std::shared_ptr<BatchRequest> request,
                                                   const Image &src_img) {
    ITT_TASK("SubmitImageProcessing");
    if (not request or not request->infer_request)
        throw std::invalid_argument("InferRequest is absent");
    auto blob = request->infer_request->GetBlob(input_name);
    size_t batch_index = request->buffers.size();
    Image dst_img = MapBlobBufferToImage(blob, batch_index);
    if (src_img.planes[0] != dst_img.planes[0]) { // only convert if different buffers
        try {
            pre_processor->Convert(src_img, dst_img);
        } catch (const std::exception &e) {
            std::throw_with_nested(std::runtime_error("Failed while software frame preprocessing"));
>>>>>>> 1bdbbc8... Release 2020.4
        }
    }
}

<<<<<<< HEAD
void OpenVINOImageInference::StartAsync(std::shared_ptr<BatchRequest> &request) {
    ITT_TASK(__FUNCTION__);
    if (not request or not request->infer_request)
        throw std::invalid_argument("InferRequest is absent");
#if 1 // TODO: remove when license-plate-recognition-barrier model will take one input
    if (inputs.count("seq_ind")) {
        // 'seq_ind' input layer is some relic from the training
        // it should have the leading 0.0f and rest 1.0f
        IE::Blob::Ptr seqBlob = request->infer_request->GetBlob("seq_ind");
        int maxSequenceSizePerPlate = seqBlob->getTensorDesc().getDims()[0];
        float *blob_data = seqBlob->buffer().as<float *>();
        blob_data[0] = 0.0f;
        std::fill(blob_data + 1, blob_data + maxSequenceSizePerPlate, 1.0f);
    }
#endif
    request->infer_request->StartAsync();
}

void OpenVINOImageInference::SubmitImage(const Image &image, IFramePtr user_data,
                                         std::function<void(Image &)> preProcessor) {
=======
void OpenVINOImageInference::BypassImageProcessing(const std::string &input_name, std::shared_ptr<BatchRequest> request,
                                                   const Image &src_img) {
    ITT_TASK("BypassImage");
    if (not request or not request->infer_request)
        throw std::invalid_argument("InferRequest is absent");
    auto blob = WrapImageToBlob(src_img);
    request->infer_request->SetBlob(input_name, blob);
}

void OpenVINOImageInference::ApplyInputPreprocessors(
    std::shared_ptr<BatchRequest> &request, const std::map<std::string, InputLayerDesc::Ptr> &input_preprocessors) {
    ITT_TASK(__FUNCTION__);
    for (const auto &preprocessor : input_preprocessors) {
        std::string layer_name = preprocessor.second->name;
        if (preprocessor.first == KEY_image) {
            if (not pre_processor.get())
                continue;
            if (inputs.size() == 1)
                layer_name = image_layer;
        }
        if (inputs.count(layer_name)) {
            IE::Blob::Ptr ie_blob = request->infer_request->GetBlob(layer_name);
            InputBlob::Ptr blob = std::make_shared<OpenvinoInputBlob>(ie_blob);
            preprocessor.second->preprocessor(blob);
        } else {
            throw std::invalid_argument("Network does not contain layer: " + layer_name);
        }
    }
}

void OpenVINOImageInference::SubmitImage(const Image &image, IFramePtr user_data,
                                         const std::map<std::string, InputLayerDesc::Ptr> &input_preprocessors) {
>>>>>>> 1bdbbc8... Release 2020.4
    GVA_DEBUG(__FUNCTION__);
    ITT_TASK(__FUNCTION__);

    ++requests_processing_;
    auto request = freeRequests.pop();

<<<<<<< HEAD
    SubmitImageSoftwarePreProcess(request, image, preProcessor);

    request->buffers.push_back(user_data);
    // start inference asynchronously if enough buffers for batching
    if (request->buffers.size() >= (size_t)batch_size) {
        StartAsync(request);
=======
    if (pre_processor.get()) {
        SubmitImageProcessing(image_layer, request, image);
    } else {
        BypassImageProcessing(image_layer, request, image);
    }

    ApplyInputPreprocessors(request, input_preprocessors);

    request->buffers.push_back(user_data);

    // start inference asynchronously if enough buffers for batching
    if (request->buffers.size() >= (size_t)batch_size) {
        request->infer_request->StartAsync();
>>>>>>> 1bdbbc8... Release 2020.4
    } else {
        freeRequests.push_front(request);
    }
}

const std::string &OpenVINOImageInference::GetModelName() const {
    return model_name;
}

<<<<<<< HEAD
void OpenVINOImageInference::GetModelInputInfo(int *width, int *height, int *batch_size, int *format) const {
    if (inputs.empty())
        throw std::invalid_argument("DL model input layers info is empty");
    auto dims = inputs.begin()->second->getTensorDesc().getDims();
    std::reverse(dims.begin(), dims.end());
    if (dims.size() < 2)
        throw std::runtime_error("Incorrect model input dimensions");
    *width = dims[0];
    *height = dims[1];
    *batch_size = dims[3];
    *format = FOURCC_RGBP;
=======
void OpenVINOImageInference::GetModelImageInputInfo(size_t &width, size_t &height, size_t &batch_size,
                                                    int &format) const {
    if (inputs.empty())
        throw std::invalid_argument("DL model input layers info is empty");
    auto blob = inputs.find(image_layer);
    if (blob == inputs.end())
        throw std::invalid_argument("Can not get image input blob by name: " + image_layer);

    auto desc = blob->second->getTensorDesc();
    auto dims = desc.getDims();
    auto layout = desc.getLayout();
    switch (layout) {
    case InferenceEngine::Layout::NCHW:
        batch_size = dims[0];
        height = dims[2];
        width = dims[3];
        break;
    case InferenceEngine::Layout::NHWC:
        batch_size = dims[0];
        height = dims[1];
        width = dims[2];
        break;
    default:
        throw std::invalid_argument("Unsupported layout for image");
    }
    format = FourCC::FOURCC_RGBP;
>>>>>>> 1bdbbc8... Release 2020.4
}

void OpenVINOImageInference::Flush() {
    std::unique_lock<std::mutex> lk(mutex_);
    while (requests_processing_ != 0) {
        auto request = freeRequests.pop();
        if (request->buffers.size() > 0) {
<<<<<<< HEAD
            StartAsync(request);
=======
            request->infer_request->StartAsync();
>>>>>>> 1bdbbc8... Release 2020.4
        } else {
            freeRequests.push(request);
        }
        request_processed_.wait_for(lk, std::chrono::seconds(1), [this] { return requests_processing_ == 0; });
    }
}

void OpenVINOImageInference::Close() {
    Flush();
    while (!freeRequests.empty()) {
        auto req = freeRequests.pop();
        // as earlier set callbacks own shared pointers we need to set lambdas with the empty capture lists
        req->infer_request->SetCompletionCallback([] {});
        if (allocator) {
            for (auto ac : req->alloc_context)
                allocator->Free(ac);
        }
    }
}

void OpenVINOImageInference::WorkingFunction(const std::shared_ptr<BatchRequest> &request) {
    GVA_DEBUG(__FUNCTION__);
    std::map<std::string, OutputBlob::Ptr> output_blobs;
    for (auto output : outputs) {
        const std::string &name = output.first;
<<<<<<< HEAD
        output_blobs[name] = std::make_shared<IEOutputBlob>(request->infer_request->GetBlob(name));
    }
    callback(output_blobs, request->buffers);
    request->buffers.clear();
    freeRequests.push(request);
=======
        output_blobs[name] = std::make_shared<OpenvinoOutputBlob>(request->infer_request->GetBlob(name));
    }
    callback(output_blobs, request->buffers);
>>>>>>> 1bdbbc8... Release 2020.4
}
