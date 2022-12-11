#pragma once
#include "Vog/Core/Core.h"
#include "Vog/Core/Layer.h"

namespace vog {

	class VOG_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		virtual ~ImGuiLayer();

		virtual void onAttach();
		virtual void onDetach();
		void setBlockEvents(bool block_) { m_blockEvents = block_; }
		//virtual void onImGuiRender();

		void begin();
		void end();

		void setDarkThemeColors();

		virtual void onEvent(Event& event_);
	private:
		bool m_blockEvents = true;
	};
}