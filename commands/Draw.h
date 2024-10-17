#pragma once
#include "Command.h"

class Draw :public Command
{
public:
	Draw(uint32_t vertexCnt, uint32_t firstVertex=0, uint32_t instanceCnt=1, uint32_t firstInstance=0);
	virtual ~Draw();
	virtual void record(CommandBuffer& cmdBuffer)override;
private:
	uint32_t m_vertexCnt;
	uint32_t m_firstVertex;
	uint32_t m_instanceCnt;
	uint32_t m_firstInstance;
};