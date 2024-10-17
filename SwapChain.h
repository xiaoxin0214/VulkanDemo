#pragma once

#include <vulkan/vulkan.h>
#include <vector>
class HelloTriangleApplication;
class SwapChain final
{
public:
	SwapChain(HelloTriangleApplication*pApp);
	~SwapChain();
	
	struct SwapChainInfo
	{
		VkExtent2D                       imageExtend;
		VkSurfaceFormatKHR               format;
		uint32_t                         imageCount;
		VkSurfaceTransformFlagBitsKHR    transform;
		VkPresentModeKHR                 presentMode;
	};

	 operator VkSwapchainKHR() const {
		return m_vkSwapChain;
	}

	VkFormat getSwapChainImageFormat()
	{
		return m_swapChainInfo.format.format;
	}

	std::vector<VkImageView>& getImageViews()
	{
		return m_vkImageViews;
	}

private:
	void querySwapChainInfo(HelloTriangleApplication* pApp);
	void getImages();
	void createImageViews();
private:
	HelloTriangleApplication*    m_pApp;
	VkSwapchainKHR               m_vkSwapChain;
	SwapChainInfo                m_swapChainInfo;
	std::vector<VkImage>         m_vkImages;
	std::vector<VkImageView>     m_vkImageViews;
};