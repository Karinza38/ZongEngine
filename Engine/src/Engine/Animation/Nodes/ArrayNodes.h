#pragma once

#include "Engine/Animation/NodeProcessor.h"

#define DECLARE_ID(name) static constexpr Identifier name{ #name }

namespace Engine::AnimationGraph {

	template<typename T>
	struct Get : public NodeProcessor
	{
		struct IDs
		{
			DECLARE_ID(Array);
		private:
			IDs() = delete;
		};

		explicit Get(const char* dbgName, UUID id);

		T* in_Array = nullptr;
		int32_t* in_Index = nullptr;

		T out_Element{ 0 };

	public:
		void Init() override;
		float Process(float) override;
	};

} // namespace Engine::AnimationGraph

#undef DECLARE_ID
