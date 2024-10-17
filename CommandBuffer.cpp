#include "CommandBuffer.h"
#include "CommandPool.h"
#include <stdexcept>

CommandBuffer::CommandBuffer(CommandPool* pCmdPool,VkCommandBufferLevel level)
	:m_level(level)
{
	VkCommandBufferAllocateInfo cmdBufferAllocateInfo{};
	cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.pNext = nullptr;
	cmdBufferAllocateInfo.commandPool = *pCmdPool;
	cmdBufferAllocateInfo.commandBufferCount = 1;
	cmdBufferAllocateInfo.level = level;

	if (vkAllocateCommandBuffers(pCmdPool->getDevice(), &cmdBufferAllocateInfo, &m_vkCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

CommandBuffer::~CommandBuffer()
{

}

void CommandBuffer::reset()
{
	vkResetCommandBuffer(m_vkCommandBuffer,0);
}
