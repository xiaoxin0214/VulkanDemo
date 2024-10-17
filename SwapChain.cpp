#include "SwapChain.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "Application.h"

SwapChain::SwapChain(HelloTriangleApplication* pApp) :m_pApp(pApp)
{
	querySwapChainInfo(pApp);
	VkSwapchainCreateInfoKHR createInfo;
	createInfo.clipped = true;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.imageArrayLayers = 1;
	createInfo.surface = pApp->getSurface();
	createInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageColorSpace = m_swapChainInfo.format.colorSpace;
	createInfo.imageFormat = m_swapChainInfo.format.format;
	createInfo.imageExtent = m_swapChainInfo.imageExtend;
	createInfo.presentMode = m_swapChainInfo.presentMode;
	createInfo.minImageCount = m_swapChainInfo.imageCount;

	auto queueFamilyIndices = pApp->getQueueFamilyIndices();

	createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
	createInfo.pQueueFamilyIndices = &queueFamilyIndices[0];

	createInfo.imageSharingMode = queueFamilyIndices.size() == 1 ? VkSharingMode::VK_SHARING_MODE_EXCLUSIVE : VkSharingMode::VK_SHARING_MODE_CONCURRENT;

	createInfo.flags = 0;
	createInfo.oldSwapchain = nullptr;
	createInfo.preTransform = m_swapChainInfo.transform;

	if (vkCreateSwapchainKHR(pApp->getDevice(), &createInfo, nullptr, &m_vkSwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swapchain！");
	}

	getImages();
	createImageViews();
}

SwapChain::~SwapChain()
{
	for (auto& imageView : m_vkImageViews)
	{
		vkDestroyImageView(m_pApp->getDevice(), imageView, nullptr);
	}

	vkDestroySwapchainKHR(m_pApp->getDevice(), m_vkSwapChain, nullptr);
}

void SwapChain::querySwapChainInfo(HelloTriangleApplication* pApp)
{
	auto physicalDevice = pApp->getPhysicalDevice();
	auto surface = pApp->getSurface();

	uint32_t formatCount = 0;
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr) != VK_SUCCESS || formatCount < 1)
	{
		throw std::runtime_error("failed to get surface format count!");
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, &surfaceFormats[0]);
	m_swapChainInfo.format = surfaceFormats[0];
	for (auto& surfaceFormat : surfaceFormats)
	{
		if (surfaceFormat.format == VkFormat::VK_FORMAT_R8G8B8A8_SRGB
			&& surfaceFormat.colorSpace == VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			m_swapChainInfo.format = surfaceFormat;
			break;
		}
	}

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to get surface capabilities！");
	}

	auto viewport = pApp->getViewPort();
	m_swapChainInfo.imageCount = std::clamp<uint32_t>(2, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
	m_swapChainInfo.imageExtend.width = std::clamp<uint32_t>(viewport.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
	m_swapChainInfo.imageExtend.height = std::clamp<uint32_t>(viewport.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

	m_swapChainInfo.transform = surfaceCapabilities.currentTransform;

	uint32_t presentModeCount = 0;
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr) != VK_SUCCESS || presentModeCount < 1)
	{
		throw std::runtime_error("failed to get surface present mode！");
	}

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, &presentModes[0]);
	m_swapChainInfo.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
	for (auto& presentMode : presentModes)
	{
		if (presentMode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
		{
			m_swapChainInfo.presentMode = presentMode;
			break;
		}
	}

	// VK_PRESENT_MODE_FIFO_KHR:多个图像要绘制到屏幕时，依次绘制
	// VK_PRESENT_MODE_FIFO_RELAXED_KHR:多个图像要绘制到屏幕时，前一张即使未绘制完，也放弃第一张，开始绘制新的，可能会出现tearing现象
	// VK_PRESENT_MODE_MAILBOX_KHR:多个图像需要绘制到屏幕时，会丢弃中间过时图像
}

void SwapChain::getImages()
{
	uint32_t imagesCount=0;
	if (vkGetSwapchainImagesKHR(m_pApp->getDevice(), m_vkSwapChain, &imagesCount, nullptr) != VK_SUCCESS || imagesCount < 1)
	{
		throw std::runtime_error("failed to get swapchain images！");
	}

	m_vkImages.resize(imagesCount);

	vkGetSwapchainImagesKHR(m_pApp->getDevice(), m_vkSwapChain, &imagesCount, &m_vkImages[0]);
}

void SwapChain::createImageViews()
{
	m_vkImageViews.resize(m_vkImages.size());
	for (std::size_t i = 0; i < m_vkImages.size(); ++i)
	{
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_vkImages[i];
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.format = m_swapChainInfo.format.format;
		imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R ,VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,VK_COMPONENT_SWIZZLE_A };
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.levelCount = 1;

		vkCreateImageView(m_pApp->getDevice(), &imageViewCreateInfo, nullptr, &m_vkImageViews[i]);
	}
}