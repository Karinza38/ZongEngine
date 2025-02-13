#include "pch.h"
#include "UniformBufferSet.h"

#include "Engine/Renderer/Renderer.h"

#include "Engine/Platform/Vulkan/VulkanUniformBufferSet.h"

#include "Engine/Renderer/RendererAPI.h"

namespace Engine {

	Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t size, uint32_t framesInFlight)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:   return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanUniformBufferSet>::Create(size, framesInFlight);
		}

		ZONG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}