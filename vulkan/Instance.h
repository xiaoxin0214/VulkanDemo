#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include <memory>

class PhysicalDevice;
class Instance
{
public:
	Instance(std::vector<const char*>&requiredExtensions,std::vector<const char*>&layers);
	~Instance();

	operator VkInstance()const
	{
		return m_vkInstance;
	}

private:
	VkInstance m_vkInstance;
	std::vector<std::shared_ptr<PhysicalDevice>> m_physicalDevices;
};