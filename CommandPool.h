#pragma once
#include "vulkan/vulkan.h"
#include <memory>
class CommandBuffer;
class CommandPool
{
public:
	CommandPool(VkDevice device, uint32_t queueFamilyIndex);
	~CommandPool();
public:
	operator VkCommandPool()const
	{
		return m_vkCommandPool;
	}

	VkDevice getDevice()const
	{
		return m_device;
	}

	std::shared_ptr<CommandBuffer> allocate(VkCommandBufferLevel level= VK_COMMAND_BUFFER_LEVEL_PRIMARY);
private:
	VkCommandPool m_vkCommandPool;
	VkDevice      m_device;
};