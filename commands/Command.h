#pragma once

#include "vulkan/vulkan.h"
class CommandBuffer;
class Command
{
public:
	Command();
	virtual ~Command();
public:
	virtual void record(CommandBuffer&) = 0;
};