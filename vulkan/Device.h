#pragma once
#include "vulkan/vulkan.h"

class Device
{
public:
	Device();
	~Device();

	operator VkDevice()const {
		return m_vkDevice;
	}

private:
	VkDevice m_vkDevice;
};