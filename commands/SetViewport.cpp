#include "SetViewport.h"
#include "../CommandBuffer.h"
SetViewport::SetViewport(const std::vector<VkViewport>& viewports, uint32_t firstViewport):m_viewports(viewports), m_firstViewport(firstViewport)
{

}

SetViewport::~SetViewport()
{

}

void SetViewport::record(CommandBuffer& cmdBuffer)
{
	vkCmdSetViewport(cmdBuffer, m_firstViewport, m_viewports.size(), m_viewports.data());
}