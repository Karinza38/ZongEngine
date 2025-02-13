#include "pch.h"
#include "AnimationGraphNodes.h"

#include "Engine/Utilities/StringUtils.h"

#include <array>

namespace Alias = Engine::AnimationGraph::Alias;
namespace AG = Engine::AnimationGraph;

namespace Engine::AnimationGraph {

	namespace Animation {

		static Node* Output()
		{
			const std::string nodeName = choc::text::replace(__func__, "_", " ");
			auto* node = new Node(nodeName.c_str(), ImColor(255, 128, 128));
			node->Inputs.push_back(Types::CreatePinForType(Types::EPinType::Pose, "Pose"));
			return node;
		}

		static Node* Event()
		{
			const std::string nodeName = choc::text::replace(__func__, "_", " ");
			auto* node = new Node(nodeName.c_str(), ImColor(255, 128, 128));
			node->Inputs.push_back(Types::CreatePinForType(Types::EPinType::Flow, "On Event"));
			node->Inputs.push_back(Types::CreatePinForType(Types::EPinType::String, "Event ID"));
			return node;
		}

	}


	namespace Transition {

		static Node* Output()
		{
			auto* node = new Node("Output", ImColor(255, 128, 128));
			node->Inputs.push_back(Types::CreatePinForType(Types::EPinType::Bool, "Transition"));
			node->Inputs.push_back(Types::CreatePinForType(Types::EPinType::Bool, "Synchronize"));
			node->Inputs.push_back(Types::CreatePinForType(Types::EPinType::Float, "Duration"));
			return node;
		}
	}


	namespace Utility {

		static Node* Comment()
		{
			const std::string nodeName = choc::text::replace(__func__, "_", " ");

			auto* node = new Types::Node(nodeName.c_str(), ImColor(255, 255, 255, 20));
			node->Type = NodeType::Comment;
			node->Size = ImVec2(300, 200);

			return node;
		}
	}


	template<class TNodeType>
	static Node* CreateNode(std::string_view name, const ImColor& color, NodeType type)
	{
		static_assert(DescribedNode<TNodeType>::value, "NodeProcessor type must be described.");
		Node* node = Impl::CreateNodeForProcessor<TNodeType>();
		node->Name = name;
		node->Color = color;
		node->Type = type;

		return node;
	}


	Node* StateMachineNodeFactory::CreateTransitionNode()
	{
		return CreateNode<AG::TransitionNode>("Transition", ImColor(), NodeType::Subroutine);
	}


	template<class TNodeType>
	static std::pair<std::string, std::function<Node* ()>> CreateRegistryEntry(std::string_view typeName, const ImColor& colour, NodeType type, bool isAlias = false)
	{
		const std::string name = isAlias ? std::string(typeName) : Utils::CreateUserFriendlyTypeName(typeName);
		const auto constructor = [=] { return CreateNode<TNodeType>(name, colour, type); };
		return { name, constructor };
	}


	namespace Colors {
		static constexpr ImU32 AnimationAsset = Types::GetPinColor(Types::EPinType::AnimationAsset);
		static constexpr ImU32 Bool           = Types::GetPinColor(Types::EPinType::Bool);
		static constexpr ImU32 Enum           = Types::GetPinColor(Types::EPinType::Enum);
		static constexpr ImU32 Float          = Types::GetPinColor(Types::EPinType::Float);
		static constexpr ImU32 Int            = Types::GetPinColor(Types::EPinType::Int);
		static constexpr ImU32 Pose           = Types::GetPinColor(Types::EPinType::Pose);
		static constexpr ImU32 SkeletonAsset  = Types::GetPinColor(Types::EPinType::SkeletonAsset);
		static constexpr ImU32 String         = Types::GetPinColor(Types::EPinType::String);
		static constexpr ImU32 Trigger        = Types::GetPinColor(Types::EPinType::Flow);
	}

#define DECLARE_PIN_TYPE(MemberPointer, PinTypeOverride) { Nodes::SanitizeMemberName(#MemberPointer, true),  PinTypeOverride }
#define DECLARE_DEF_VALUE(MemberPointer, ValueOverride) { Nodes::SanitizeMemberName(#MemberPointer, true), choc::value::Value(ValueOverride) }

	// TODO: add static_assert "Described"
	static const std::unordered_map<std::string, Types::EPinType> PinTypeOverrides =
	{
		DECLARE_PIN_TYPE(AG::AnimationPlayer::out_Pose,                AG::Types::EPinType::Pose),
		DECLARE_PIN_TYPE(AG::RangedBlend::out_Pose,                    AG::Types::EPinType::Pose),
		DECLARE_PIN_TYPE(AG::StateMachine::out_Pose,                   AG::Types::EPinType::Pose),
		DECLARE_PIN_TYPE(AG::State::out_Pose,                          AG::Types::EPinType::Pose),
	};

	// TODO: should this info be just stored inside of the NodeProcessor?
	static const std::unordered_map<std::string, choc::value::Value> DefaultPinValues =
	{
		DECLARE_DEF_VALUE(AG::AnimationPlayer::in_Animation,           AG::AnimationPlayer::DefaultAnimation),
		DECLARE_DEF_VALUE(AG::AnimationPlayer::in_PlaybackSpeed,       AG::AnimationPlayer::DefaultPlaybackSpeed),
		DECLARE_DEF_VALUE(AG::AnimationPlayer::in_Offset,              AG::AnimationPlayer::DefaultOffset),
		DECLARE_DEF_VALUE(AG::AnimationPlayer::in_Loop,                AG::AnimationPlayer::DefaultLoop),

		DECLARE_DEF_VALUE(AG::RangedBlend::in_AnimationA,              AG::RangedBlend::DefaultAnimation),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_AnimationB,              AG::RangedBlend::DefaultAnimation),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_PlaybackSpeedA,          AG::RangedBlend::DefaultPlaybackSpeed),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_PlaybackSpeedB,          AG::RangedBlend::DefaultPlaybackSpeed),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_Synchronize,             AG::RangedBlend::DefaultSynchronize),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_OffsetA,                 AG::RangedBlend::DefaultOffset),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_OffsetB,                 AG::RangedBlend::DefaultOffset),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_Loop,                    AG::RangedBlend::DefaultLoop),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_RangeA,                  AG::RangedBlend::DefaultRangeA),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_RangeB,                  AG::RangedBlend::DefaultRangeB),
		DECLARE_DEF_VALUE(AG::RangedBlend::in_Value,                   AG::RangedBlend::DefaultValue),
	};

#undef DECLARE_PIN_TYPE
#undef DECLARE_DEF_VALUE


	namespace EnumTokens {}


	std::optional<choc::value::Value> GetPinDefaultValueOverride(std::string_view nodeName, std::string_view memberNameSanitized)
	{
		// Passed in names must not contain namespace
		ZONG_CORE_ASSERT(nodeName.find("::") == std::string_view::npos);
		ZONG_CORE_ASSERT(memberNameSanitized.find("::") == std::string_view::npos);

		const std::string fullName = choc::text::joinStrings<std::array<std::string_view, 2>>({ nodeName, memberNameSanitized }, "::");
		if (!DefaultPinValues.count(fullName))
		{
			//return choc::value::Value(1);
			return {};
		}

		return DefaultPinValues.at(fullName);
	}


	std::optional<Types::EPinType> GetPinTypeForMemberOverride(std::string_view nodeName, std::string_view memberNameSanitized)
	{
		// Passed in names must not contain namespace
		ZONG_CORE_ASSERT(nodeName.find("::") == std::string_view::npos);
		ZONG_CORE_ASSERT(memberNameSanitized.find("::") == std::string_view::npos);

		const std::string fullName = choc::text::joinStrings<std::array<std::string_view, 2>>({ nodeName, memberNameSanitized }, "::");
		if (!PinTypeOverrides.count(fullName))
			return {};

		return PinTypeOverrides.at(fullName);
	}


	Node* AnimationGraphNodeFactory::SpawnGraphPropertyNode(const Ref<AnimationGraphAsset>& graph, std::string_view propertyName, EPropertyType type, std::optional<bool> isLocalVarGetter)
	{
		ZONG_CORE_ASSERT(graph);
		ZONG_CORE_ASSERT(((type != EPropertyType::LocalVariable) ^ isLocalVarGetter.has_value()), "'isLocalVarGetter' property must be set if 'type' is 'LocalVariable'");

		const bool isLocalVariable = isLocalVarGetter.has_value() && (type == EPropertyType::LocalVariable);

		const Utils::PropertySet* properties = (type == EPropertyType::Input) ? &graph->Inputs
			: (type == EPropertyType::Output) ? &graph->Outputs
			: (type == EPropertyType::LocalVariable) ? &graph->LocalVariables
			: nullptr;

		if (!properties)
			return nullptr;

		if (!properties->HasValue(propertyName))
		{
			ZONG_CORE_ERROR("SpawnGraphPropertyNode() - property with the name \"{}\" doesn't exist!", propertyName);
			return nullptr;
		}

		choc::value::Value value = properties->GetValue(propertyName);

		const ImColor typeColour = GetValueColorStatic(value);
		const auto [pinType, storageKind] = GetPinTypeAndStorageKindForValueStatic(value);

		if (!isLocalVariable && value.isVoid())
		{
			ZONG_CORE_ERROR("SpawnGraphPropertyNode() - invalid Graph Property value to spawn Graph Property node. Property name {}", propertyName);
			return nullptr;
		}

		const std::string propertyTypeString = Utils::SplitAtUpperCase(magic_enum::enum_name<EPropertyType>(type));
		const std::string nodeName(isLocalVariable ? propertyName : propertyTypeString);

		Node* newNode = new Types::Node(nodeName, typeColour);
		newNode->Category = propertyTypeString;

		if (isLocalVarGetter.has_value())
			newNode->Type = NodeType::Simple;

		Pin* newPin = Types::CreatePinForType(pinType, propertyName);
		newPin->Storage = storageKind;
		newPin->Value = value;

		if (type == EPropertyType::Input || (isLocalVariable && *isLocalVarGetter))
		{
			newPin->Kind = PinKind::Output;
			newNode->Outputs.push_back(newPin);
		}
		else
		{
			newPin->Kind = PinKind::Input;
			newNode->Inputs.push_back(newPin);
		}

		return newNode;
	}

} // namespace Engine::AnimationGraph


//! To register a new node, see comments in AnimationNodes.h

#define NODE_CATEGORY(category, nodeFactories, ...) { std::string { #category }, std::map<std::string, std::function<Node*()>>{ nodeFactories, __VA_ARGS__ }}
#define DECLARE_NODE(category, name) {choc::text::replace(#name, "_", " "), category::name}
#define DECLARE_ARGS(name, args) {#name, args}
#define DECLARE_NODE_N(TNodeType, Color, Type) AG::CreateRegistryEntry<TNodeType>(#TNodeType, Color, Type)
#define DECLARE_NODE_ALIAS(AliasName, TNodeType, Color, Type) AG::CreateRegistryEntry<TNodeType>(AliasName, Color, Type, true)

namespace Colors = AG::Colors;

template<>
const Engine::Nodes::Registry Engine::Nodes::Factory<AG::AnimationGraphNodeFactory>::Registry =
{
	NODE_CATEGORY(Animation,
		DECLARE_NODE(AG::Animation, Output),
		DECLARE_NODE(AG::Animation, Event),
		DECLARE_NODE_N(AG::AnimationPlayer,                                    ImColor(54, 207, 145),  NodeType::Blueprint),
		DECLARE_NODE_N(AG::StateMachine,                                       ImColor(236, 158, 36),  NodeType::Subroutine),
	),
	NODE_CATEGORY(Array,
		DECLARE_NODE_N(AG::Get<float>,                                         Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Get<int>,                                           Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_ALIAS(Alias::GetAnimation, AG::Get<int64_t>,              Colors::AnimationAsset, NodeType::Blueprint),
	),
	NODE_CATEGORY(Blend,
		DECLARE_NODE_N(AG::RangedBlend,                                        Colors::Pose,           NodeType::Blueprint),
	),
	NODE_CATEGORY(Logic,
		DECLARE_NODE_N(AG::CheckEqual<float>,                                  Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckEqual<int>,                                    Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckNotEqual<float>,                               Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckNotEqual<int>,                                 Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLess<float>,                                   Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLess<int>,                                     Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLessEqual<float>,                              Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLessEqual<int>,                                Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreater<float>,                                Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreater<int>,                                  Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreaterEqual<float>,                           Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreaterEqual<int>,                             Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::And,                                                Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::Or,                                                 Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::Not,                                                Colors::Bool,           NodeType::Simple),
	),
	NODE_CATEGORY(Math,
		DECLARE_NODE_N(AG::Add<float>,                                         Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Add<int>,                                           Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Divide<float>,                                      Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Divide<int>,                                        Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Log,                                                Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Modulo,                                             Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Multiply<float>,                                    Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Multiply<int>,                                      Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Power,                                              Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Subtract<float>,                                    Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Subtract<int>,                                      Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::MapRange<float>,                                    Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::MapRange<int>,                                      Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_N(AG::Min<float>,                                         Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Min<int>,                                           Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_N(AG::Max<float>,                                         Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Max<int>,                                           Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_N(AG::Clamp<float>,                                       Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Clamp<int>,                                         Colors::Int,            NodeType::Blueprint),
	),
	NODE_CATEGORY(Trigger,
		DECLARE_NODE_N(AG::BoolTrigger,                                        Colors::Trigger,        NodeType::Blueprint),
	),
	NODE_CATEGORY(Utility,
		 DECLARE_NODE(AG::Utility, Comment),
	)
};


template<>
const Engine::Nodes::Registry Engine::Nodes::Factory<AG::StateMachineNodeFactory>::Registry =
{
	NODE_CATEGORY(StateMachine,
		DECLARE_NODE_N(AG::StateMachine, ImColor(236, 158, 36),  NodeType::Subroutine),
		DECLARE_NODE_N(AG::State,        ImColor(143, 188, 143), NodeType::Subroutine),
		DECLARE_NODE_N(AG::QuickState,   ImColor(51, 150, 215),  NodeType::Blueprint),
	),
	NODE_CATEGORY(Utility,
		DECLARE_NODE(AG::Utility, Comment),
	)
};


template<>
const Engine::Nodes::Registry Engine::Nodes::Factory<AG::TransitionGraphNodeFactory>::Registry =
{
	NODE_CATEGORY(Array,
		DECLARE_NODE_N(AG::Get<float>,                                         Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Get<int>,                                           Colors::Int,            NodeType::Blueprint),
	),
	NODE_CATEGORY(Logic,
		DECLARE_NODE_N(AG::CheckEqual<float>,                                  Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckEqual<int>,                                    Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckNotEqual<float>,                               Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckNotEqual<int>,                                 Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLess<float>,                                   Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLess<int>,                                     Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLessEqual<float>,                              Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckLessEqual<int>,                                Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreater<float>,                                Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreater<int>,                                  Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreaterEqual<float>,                           Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::CheckGreaterEqual<int>,                             Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::And,                                                Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::Or,                                                 Colors::Bool,           NodeType::Simple),
		DECLARE_NODE_N(AG::Not,                                                Colors::Bool,           NodeType::Simple),
	),
	NODE_CATEGORY(Math,
		DECLARE_NODE_N(AG::Add<float>,                                         Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Add<int>,                                           Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Divide<float>,                                      Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Divide<int>,                                        Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Log,                                                Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Modulo,                                             Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Multiply<float>,                                    Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Multiply<int>,                                      Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::Power,                                              Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Subtract<float>,                                    Colors::Float,          NodeType::Simple),
		DECLARE_NODE_N(AG::Subtract<int>,                                      Colors::Int,            NodeType::Simple),
		DECLARE_NODE_N(AG::MapRange<float>,                                    Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::MapRange<int>,                                      Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_N(AG::Min<float>,                                         Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Min<int>,                                           Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_N(AG::Max<float>,                                         Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Max<int>,                                           Colors::Int,            NodeType::Blueprint),
		DECLARE_NODE_N(AG::Clamp<float>,                                       Colors::Float,          NodeType::Blueprint),
		DECLARE_NODE_N(AG::Clamp<int>,                                         Colors::Int,            NodeType::Blueprint),
	),
	NODE_CATEGORY(Transition,
		DECLARE_NODE(AG::Transition, Output),
		DECLARE_NODE(AG::Animation, Event),
	),
	NODE_CATEGORY(Trigger,
		DECLARE_NODE_N(AG::BoolTrigger,                                        Colors::Trigger,        NodeType::Blueprint),
	),
	NODE_CATEGORY(Utility,
		 DECLARE_NODE(AG::Utility, Comment),
	)
};


#undef NODE_CATEGORY
#undef DECLARE_NODE
#undef DECLARE_ARGS
#undef DECLARE_NODE_N
#undef DECLARE_NODE_ALIAS

#define DECLARE_PIN_ENUM(MemberPointer, Tokens) { std::string(Engine::Nodes::SanitizeMemberName(#MemberPointer, true)),  Tokens }

template<>
const Engine::Nodes::EnumTokensRegistry Engine::Nodes::Factory<AG::AnimationGraphNodeFactory>::EnumTokensRegistry =
{
};

template<>
const Engine::Nodes::EnumTokensRegistry Engine::Nodes::Factory<AG::StateMachineNodeFactory>::EnumTokensRegistry =
{
};

template<>
const Engine::Nodes::EnumTokensRegistry Engine::Nodes::Factory<AG::TransitionGraphNodeFactory>::EnumTokensRegistry =
{
};

#undef DECLARE_PIN_ENUM
