#pragma once
#include "vogpch.h"
#include "Vog/Core/Core.h"
#include "Vog/Utilities/MyCommon.h"

#include "Vog/Events/Event.h"

namespace vog {

	class VOG_API Layer : public NonCopyable
	{
	public:
		Layer(const std::string& name_);
		virtual ~Layer() = default;

		virtual void onAttach() {};
		virtual void onDetach() {};
		virtual void onUpdate(float dt_) {};
		virtual void onImGuiRender() {};
		virtual void onDebugRender() {};
		virtual void onEvent(Event& event_) {};

		inline const std::string& getName() const { return m_name; }

	protected:
		std::string m_name;
	};

}