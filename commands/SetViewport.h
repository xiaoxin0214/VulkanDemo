#include "Command.h"
#include <vector>
class SetViewport :public Command
{
public:
	SetViewport(const std::vector<VkViewport>& viewpots, uint32_t firstViewport = 0);
	virtual ~SetViewport();
	virtual void record(CommandBuffer& cmdBuffer)override;
private:
	std::vector<VkViewport>   m_viewports;
	uint32_t                  m_firstViewport;
};