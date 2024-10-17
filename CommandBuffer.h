#pragma once
#include "vulkan/vulkan.h"
class CommandPool;
class CommandBuffer
{
public:
	CommandBuffer(CommandPool*pCmdPool,VkCommandBufferLevel level);
	~CommandBuffer();

public:
	operator VkCommandBuffer()const
	{
		return m_vkCommandBuffer;
	}

	void reset();
private:
	VkCommandBuffer m_vkCommandBuffer;
	VkCommandBufferLevel m_level;
};