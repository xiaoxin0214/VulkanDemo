#include "Draw.h"
#include "../CommandBuffer.h"
Draw::Draw(uint32_t vertexCnt,uint32_t firstVertex,uint32_t instanceCnt,uint32_t firstInstance)
	:m_vertexCnt(vertexCnt),m_firstVertex(firstVertex),m_instanceCnt(instanceCnt),m_firstInstance(firstInstance)
{

}

Draw::~Draw()
{

}

void Draw::record(CommandBuffer& cmdBuffer)
{
	vkCmdDraw(cmdBuffer,m_vertexCnt,m_instanceCnt,m_firstVertex,m_firstInstance);
}
