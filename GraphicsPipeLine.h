#pragma once

#include "vulkan/vulkan.h"
#include <string>
class HelloTriangleApplication;
class GraphicsPipeLine
{
public:
	GraphicsPipeLine(HelloTriangleApplication*pApp, const std::string&vsPath,const std::string&fsPath);
	~GraphicsPipeLine();

	VkRenderPass getRenderPass()
	{
		return m_vkRenderPass;
	}

	VkPipeline getPipeline()
	{
		return m_vkPipeline;
	}

private:
	void createRenderPass();
	void createPipelineLayout();
private:
	HelloTriangleApplication *m_pApp;
	VkRenderPass              m_vkRenderPass;
	VkPipelineLayout          m_vkPipelineLayout;
	VkPipeline                m_vkPipeline;
};