#pragma once

#include "vulkan/vulkan.h"

class PhysicalDevice
{
public:
	PhysicalDevice();
	~PhysicalDevice();

	operator VkPhysicalDevice()const
	{
		return m_vkPhysicalDevice;
	}

private:
	VkPhysicalDevice m_vkPhysicalDevice;
};