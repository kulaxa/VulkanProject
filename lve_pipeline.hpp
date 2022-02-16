#pragma once
#include <string>
#include <vector>
#include "lve_device.hpp"


namespace lve
{

    struct PipelineConfiguInfo{};
    class LvePipeline{
        public:
        LvePipeline( LveDevice &device,const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfiguInfo& configInfo);

        ~LvePipeline(){}

        LvePipeline(const LvePipeline& )=delete;
        void operator=(const LvePipeline) = delete;

        static PipelineConfiguInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        private:
            static std::vector<char> readFile(const std::string& filepath);

            void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,  const PipelineConfiguInfo& configInfo);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            LveDevice& lveDevice;
            VkPipeline VkGraphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
} // namespace lve
