#include "CommandPool.h"
#include <stdexcept>
#include "CommandBuffer.h"

CommandPool::CommandPool(VkDevice device, uint32_t queueFamilyIndex):m_device(device)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_vkCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

std::shared_ptr<CommandBuffer> CommandPool::allocate(VkCommandBufferLevel level)
{
	return std::make_shared<CommandBuffer>(this, level);
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(m_device, m_vkCommandPool, nullptr);
}