#include "SetScissor.h"
#include "../CommandBuffer.h"

SetScissor::SetScissor(const std::vector<VkRect2D>& scissors, uint32_t firstScissor):m_scissors(scissors),m_firstScissor(firstScissor)
{

}

SetScissor::~SetScissor()
{

}

void SetScissor::record(CommandBuffer& cmdBuffer)
{
	vkCmdSetScissor(cmdBuffer, m_firstScissor, m_scissors.size(), m_scissors.data());
}