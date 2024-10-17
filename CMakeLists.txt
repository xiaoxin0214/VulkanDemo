# CMakeList.txt : CMake project for VulkanDemo, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VulkanDemo")

include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty)
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/glfw/include)
link_libraries(${CMAKE_CURRENT_LIST_DIR}/3rdparty/glfw/lib-vc2022/glfw3.lib)

if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

set(Vulkan_MIN_VERSION 1.1.70.0)

find_package(Vulkan ${Vulkan_MIN_VERSION} REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
link_libraries(${Vulkan_LIBRARIES})

# Add source to this project's executable.
add_executable (VulkanDemo
"VulkanDemo.cpp" "VulkanDemo.h"
"Application.h" "Application.cpp"
"SwapChain.h" "SwapChain.cpp"
"GraphicsPipeLine.h" "GraphicPipeLine.cpp"
"CommandPool.h" "CommandPool.cpp"
"CommandBuffer.h" "CommandBuffer.cpp"
 "commands/Command.h" "commands/Command.cpp" 
 "commands/SetViewport.h" "commands/SetViewport.cpp" 
 "commands/SetScissor.h" "commands/SetScissor.cpp" 
 "commands/Draw.h" "commands/Draw.cpp" "vulkan/Device.h" "vulkan/Device.cpp" "vulkan/PhysicalDevice.h" "vulkan/PhysicalDevice.cpp" "vulkan/Instance.h" "vulkan/Instance.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanDemo PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

make_directory(${CMAKE_CURRENT_BINARY_DIR}/shaders)
execute_process(COMMAND ${Vulkan_INCLUDE_DIR}/../bin/glslc.exe ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.vert -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/vert.spv)
execute_process(COMMAND ${Vulkan_INCLUDE_DIR}/../bin/glslc.exe ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.frag -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/frag.spv)