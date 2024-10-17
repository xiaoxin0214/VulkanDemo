#pragma once
#include "Command.h"
#include <vector>
class SetScissor :public Command
{
public:
	SetScissor(const std::vector<VkRect2D>& scissors, uint32_t firstScissor=0);
	virtual ~SetScissor();
public:
	virtual void record(CommandBuffer& cmdBuffer)override;
private:
	std::vector<VkRect2D> m_scissors;
	uint32_t              m_firstScissor;
};