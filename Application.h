#pragma once

#include <cstdlib>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include <memory>

class GLFWwindow;
class SwapChain;
class GraphicsPipeLine;
class CommandPool;
class CommandBuffer;
class HelloTriangleApplication {
public:
	struct QueueFamilyIndices final
	{
		std::optional<uint32_t> graphicsQueueIndex;
		std::optional<uint32_t> presentQueueIndex;
	};

public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	VkDevice getDevice()
	{
		return m_vkDevice;
	}

	VkPhysicalDevice getPhysicalDevice()
	{
		return m_vkPhysicalDevice;
	}

	VkSurfaceKHR getSurface()
	{
		return m_surface;
	}

	VkFormat getSwapChainImageFormat();
	VkRenderPass getRenderPass();

	VkExtent2D getViewPort()
	{
		return m_viewport;
	}



	std::vector<uint32_t> getQueueFamilyIndices()
	{
		std::vector<uint32_t> queueFamilyIndices;
		if (m_queueFamilyIndices.graphicsQueueIndex.value() == m_queueFamilyIndices.presentQueueIndex.value())
		{
			queueFamilyIndices.push_back(m_queueFamilyIndices.graphicsQueueIndex.value());
		}
		else
		{
			queueFamilyIndices.push_back(m_queueFamilyIndices.graphicsQueueIndex.value());
			queueFamilyIndices.push_back(m_queueFamilyIndices.presentQueueIndex.value());
		}

		return queueFamilyIndices;
	}

	QueueFamilyIndices m_queueFamilyIndices;
private:
	void initVulkan();
	void mainLoop();
	void cleanup();
	void initWindow();
	std::vector<const char*> getRequiredExtensions();
	void setupDebugCallback();
	void createSurface();
	void createInstance();
	bool checkValidationLayerSupport();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
	void pickupPhysicDevice();
	void queryQueueFamilyIndices();
	void createDevice();
	void getQueues();
	void createSwapChain();
	void createGraphicsPipeline();
	void createFrameBuffers();
	void createCommandPool();
	void recordCommandBuffer(uint32_t imageIndex);
	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
private:
	GLFWwindow* m_pWindow;
	VkInstance                    m_vkInstance;
	VkPhysicalDevice              m_vkPhysicalDevice;
	VkDevice                      m_vkDevice;
	VkQueue                       m_graphicsQueue;
	VkQueue                       m_presentQueue;
	VkSurfaceKHR                  m_surface;
	VkExtent2D                    m_viewport;
	SwapChain* m_pSwapChain;
	GraphicsPipeLine* m_pGraphicsPipeline;
	VkDebugUtilsMessengerEXT      m_debugMessager;
	std::vector<VkFramebuffer>    m_vkFrameBuffers;
	CommandPool* m_pCommandPool;
	std::shared_ptr<CommandBuffer> m_cmdBuffer;

	VkSemaphore                   m_imageAvailableSemaphore;
	VkSemaphore                   m_renderingFinishedSemaphore;
	VkFence                       m_inFlightFence;
};