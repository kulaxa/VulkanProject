#pragma once
#include <string>
#include <vector>
#include "lve_device.hpp"


namespace lve
{

    struct PipelineConfiguInfo{
        
        PipelineConfiguInfo(const PipelineConfiguInfo&) =delete;
        PipelineConfiguInfo& operator=(const PipelineConfiguInfo&)=delete;
        
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicsStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class LvePipeline{
        public:
        LvePipeline( LveDevice &device,const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfiguInfo& configInfo);

        ~LvePipeline();

        LvePipeline(const LvePipeline& )=delete;
        LvePipeline& operator=(const LvePipeline) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfiguInfo& configInfo);
        private:
            static std::vector<char> readFile(const std::string& filepath);

            void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,  const PipelineConfiguInfo& configInfo);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            LveDevice& lveDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
} // namespace lve
