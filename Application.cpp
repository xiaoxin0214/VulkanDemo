#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Application.h"
#include <iostream>
#include <stdexcept>
#include "SwapChain.h"
#include "GraphicsPipeLine.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "commands/Command.h"
#include "commands/SetViewport.h"
#include "commands/SetScissor.h"
#include "commands/Draw.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance,pCreateInfo,pAllocator,pMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,VkDebugUtilsMessengerEXT messenger,const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, messenger, pAllocator);
	}
}

VkFormat HelloTriangleApplication::getSwapChainImageFormat()
{
	return m_pSwapChain->getSwapChainImageFormat();
}

VkRenderPass HelloTriangleApplication::getRenderPass()
{
	return m_pGraphicsPipeline->getRenderPass();
}

void HelloTriangleApplication::initVulkan()
{
	createInstance();
	setupDebugCallback();
	createSurface();
	pickupPhysicDevice();
	queryQueueFamilyIndices();
	createDevice();
	getQueues();

	createSwapChain();
	createGraphicsPipeline();
	createFrameBuffers();
	createCommandPool();
	createSyncObjects();
}

void HelloTriangleApplication::mainLoop() {
	while (!glfwWindowShouldClose(m_pWindow))
	{
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(m_vkDevice);
}

void HelloTriangleApplication::cleanup() {
	
	vkDestroySemaphore(m_vkDevice, m_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_vkDevice, m_renderingFinishedSemaphore, nullptr);
	vkDestroyFence(m_vkDevice, m_inFlightFence, nullptr);

	delete m_pCommandPool;
	m_pCommandPool = nullptr;

	for (auto& framebuffer : m_vkFrameBuffers)
	{
		vkDestroyFramebuffer(m_vkDevice, framebuffer, nullptr);
	}

	delete m_pSwapChain;
	m_pSwapChain = nullptr;

	delete m_pGraphicsPipeline;
	m_pGraphicsPipeline = nullptr;

	vkDestroyDevice(m_vkDevice, nullptr);
	vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_vkInstance,m_debugMessager,nullptr);
	}
	vkDestroyInstance(m_vkInstance, nullptr);

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

std::vector<const char*> HelloTriangleApplication::getRequiredExtensions()
{
	std::vector<const char*> requiredExtensions;
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	requiredExtensions.insert(requiredExtensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers)
	{
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return requiredExtensions;
}

void HelloTriangleApplication::setupDebugCallback()
{
	if (!enableValidationLayers)
		return ;

	VkDebugUtilsMessengerCreateInfoEXT debugMessagerCreateInfo;
	debugMessagerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMessagerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugMessagerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMessagerCreateInfo.pNext = nullptr;
	debugMessagerCreateInfo.pUserData = nullptr;
	debugMessagerCreateInfo.flags = 0;
	debugMessagerCreateInfo.pfnUserCallback = debugCallback;

	if (CreateDebugUtilsMessengerEXT(m_vkInstance, &debugMessagerCreateInfo, nullptr, &m_debugMessager) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create debug messager!");
	}
}

void HelloTriangleApplication::initWindow()
{
	glfwInit();

	// 不创建OpenGL上下文
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// 暂时不允许窗口改变大小
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_viewport.width = 1600;
	m_viewport.height = 1200;
	m_pWindow = glfwCreateWindow(m_viewport.width, m_viewport.height, "Vulkan Demo", nullptr, nullptr);
}

void HelloTriangleApplication::createSurface()
{
	if (glfwCreateWindowSurface(m_vkInstance, m_pWindow, nullptr, &m_surface) !=VK_SUCCESS)
	{
		throw std::runtime_error("failed to create surface!");
	}
}

void HelloTriangleApplication::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Demo";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.pNext = nullptr;

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	auto requiredExtensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = &requiredExtensions[0];

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = &validationLayers[0];
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:\n";
	for (const auto& extension : extensions) {
		std::cout << '\t' << extension.extensionName << '\n';
	}


	if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

}

bool HelloTriangleApplication::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

bool HelloTriangleApplication::isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceProperties phyDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &phyDeviceProperties);

	VkPhysicalDeviceFeatures phyDeviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &phyDeviceFeatures);

	return true;
}

void HelloTriangleApplication::pickupPhysicDevice()
{
	uint32_t phyDevicesCount = 0;
	if (vkEnumeratePhysicalDevices(m_vkInstance, &phyDevicesCount, nullptr) != VK_SUCCESS || phyDevicesCount < 1)
	{
		throw std::runtime_error("failed to get PhysicalDevice!");
	}

	std::vector<VkPhysicalDevice> vkPhysicalDevices(phyDevicesCount);
	vkEnumeratePhysicalDevices(m_vkInstance, &phyDevicesCount, &vkPhysicalDevices[0]);
	m_vkPhysicalDevice = vkPhysicalDevices[0];
	for (auto& physicalDevice : vkPhysicalDevices)
	{
		if (isPhysicalDeviceSuitable(physicalDevice))
		{
			m_vkPhysicalDevice = physicalDevice;
			break;
		}
	}
}

void HelloTriangleApplication::queryQueueFamilyIndices()
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount, &queueFamilyProperties[0]);
	for (std::size_t i = 0; i < queueFamilyCount; ++i)
	{
		const auto& property = queueFamilyProperties[i];
		if (property.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
		{
			m_queueFamilyIndices.graphicsQueueIndex = i;
		}

		VkBool32 supportKHR = VK_FALSE;
		if (vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, i, m_surface, &supportKHR) == VK_SUCCESS && supportKHR)
		{
			m_queueFamilyIndices.presentQueueIndex = i;
		}
	}
}

void HelloTriangleApplication::createDevice()
{
	float priorities = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	if (m_queueFamilyIndices.graphicsQueueIndex.value() == m_queueFamilyIndices.presentQueueIndex.value())
	{
		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsQueueIndex.value();
		queueCreateInfo.pQueuePriorities = &priorities;
		queueCreateInfo.flags = 0;

		queueCreateInfos.push_back(std::move(queueCreateInfo));
	}
	else
	{
		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsQueueIndex.value();
		queueCreateInfo.pQueuePriorities = &priorities;
		queueCreateInfo.flags = 0;

		queueCreateInfos.push_back(queueCreateInfo);

		queueCreateInfo.queueFamilyIndex = m_queueFamilyIndices.presentQueueIndex.value();
		queueCreateInfos.push_back(queueCreateInfo);
	}


	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfos[0];
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();

	deviceCreateInfo.pEnabledFeatures = nullptr;

	std::vector<const char*> extensionNames{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	deviceCreateInfo.enabledExtensionCount = extensionNames.size();
	deviceCreateInfo.ppEnabledExtensionNames = &extensionNames[0];

	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;

	deviceCreateInfo.flags = 0;
	vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice);
}

void HelloTriangleApplication::getQueues()
{
	vkGetDeviceQueue(m_vkDevice, m_queueFamilyIndices.graphicsQueueIndex.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_vkDevice, m_queueFamilyIndices.presentQueueIndex.value(), 0, &m_presentQueue);
}

void HelloTriangleApplication::createSwapChain()
{
	m_pSwapChain = new SwapChain(this);
}

void HelloTriangleApplication::createGraphicsPipeline()
{
	std::string vsPath = "D:/VulkanTutorial/VulkanDemo/out/build/x64-debug/shaders/vert.spv";
	std::string fsPath = "D:/VulkanTutorial/VulkanDemo/out/build/x64-debug/shaders/frag.spv";
	m_pGraphicsPipeline = new GraphicsPipeLine(this,vsPath,fsPath);
}

void HelloTriangleApplication::createFrameBuffers()
{
	auto&imageViews = m_pSwapChain->getImageViews();
	m_vkFrameBuffers.resize(imageViews.size());
	for (std::size_t i = 0; i < imageViews.size(); ++i)
	{
		VkImageView attachment[1]{ imageViews[i] };
		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.width = m_viewport.width;
		framebufferCreateInfo.height = m_viewport.height;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachment;
		framebufferCreateInfo.renderPass = m_pGraphicsPipeline->getRenderPass();

		if (vkCreateFramebuffer(m_vkDevice, &framebufferCreateInfo, nullptr, &m_vkFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void HelloTriangleApplication::createCommandPool()
{
	m_pCommandPool = new CommandPool(m_vkDevice,m_queueFamilyIndices.graphicsQueueIndex.value());
	m_cmdBuffer = m_pCommandPool->allocate();
}

void HelloTriangleApplication::recordCommandBuffer(uint32_t imageIndex)
{
	m_cmdBuffer->reset();
	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBeginInfo.pNext = nullptr;
	cmdBufferBeginInfo.pInheritanceInfo = nullptr;
	cmdBufferBeginInfo.flags = 0;
	if (vkBeginCommandBuffer(*m_cmdBuffer,&cmdBufferBeginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin command buffer!");
	}

	VkClearValue clearValue{ {{0.0f,0.0f,0.0f,1.0f}} };
	VkRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;
	renderPassBeginInfo.renderPass = m_pGraphicsPipeline->getRenderPass();
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = {m_viewport.width,m_viewport.height};
	renderPassBeginInfo.framebuffer = m_vkFrameBuffers[imageIndex];

	vkCmdBeginRenderPass(*m_cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(*m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,m_pGraphicsPipeline->getPipeline());
	std::vector<std::shared_ptr<Command>> cmds;
	std::vector<VkViewport> viewports{ {0.0f,0.0f,(float)m_viewport.width,(float)m_viewport.height,0.0f,1.0f} };
	std::vector<VkRect2D>   scissors{ {{0,0},{m_viewport.width,m_viewport.height}} };
	cmds.push_back(std::make_shared<SetViewport>(viewports));
	cmds.push_back(std::make_shared<SetScissor>(scissors));
	cmds.push_back(std::make_shared<Draw>(3));
	for (auto& cmd : cmds)
	{
		cmd->record(*m_cmdBuffer);
	}

	vkCmdEndRenderPass(*m_cmdBuffer);

	if (vkEndCommandBuffer(*m_cmdBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed end command buffer!");
	}
}

void HelloTriangleApplication::createSyncObjects()
{
	VkSemaphoreCreateInfo imageAvailableSemaphoreCreateInfo{};
	imageAvailableSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAvailableSemaphoreCreateInfo.flags = 0;
	imageAvailableSemaphoreCreateInfo.pNext = nullptr;

	VkSemaphoreCreateInfo renderingFinishedSemaphoreCreateInfo{};
	renderingFinishedSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	renderingFinishedSemaphoreCreateInfo.flags = 0;
	renderingFinishedSemaphoreCreateInfo.pNext = nullptr;

	VkFenceCreateInfo inFlightFenceCreateInfo{};
	inFlightFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	inFlightFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	inFlightFenceCreateInfo.pNext = nullptr;

	if (vkCreateSemaphore(m_vkDevice, &imageAvailableSemaphoreCreateInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS
		|| vkCreateSemaphore(m_vkDevice, &renderingFinishedSemaphoreCreateInfo, nullptr, &m_renderingFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(m_vkDevice, &inFlightFenceCreateInfo, nullptr, &m_inFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create sync objects!");
	}
}

void HelloTriangleApplication::drawFrame()
{
	vkWaitForFences(m_vkDevice,1,&m_inFlightFence,true,UINT64_MAX);
	vkResetFences(m_vkDevice, 1, &m_inFlightFence);
	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(m_vkDevice, *m_pSwapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	recordCommandBuffer(imageIndex);

	VkCommandBuffer cmdBuffer = *m_cmdBuffer;
	VkPipelineStageFlags pipelineStateMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask=&pipelineStateMask;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_renderingFinishedSemaphore;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit qeueue!");
	}


	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_renderingFinishedSemaphore;

	VkSwapchainKHR swapChains[] = { *m_pSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(m_presentQueue, &presentInfo);
}

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}