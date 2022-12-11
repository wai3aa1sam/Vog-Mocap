#include "vogpch.h"
#include "LayerStack.h"

namespace vog {

	LayerStack::~LayerStack()
	{
		destroy();
	}

	void LayerStack::destroy()
	{
		for (auto& layer : m_layerStack)
		{
			layer->onDetach();
			delete layer;
		}
		m_layerStack.clear();
	}

	void LayerStack::pushLayer(Layer* layer_)
	{
		m_layerStack.emplace(m_layerStack.begin() + m_layInsertIndex, layer_);
		m_layInsertIndex++;
	}

	void LayerStack::pushOverlayer(Layer* overlayer_)
	{
		m_layerStack.emplace_back(overlayer_);
	}

	void LayerStack::popLayer(Layer* layer_)
	{
		auto it = std::find(m_layerStack.begin(), m_layerStack.begin() + m_layInsertIndex, layer_);
		if (it != m_layerStack.begin() + m_layInsertIndex)
		{
			layer_->onDetach();
			m_layerStack.erase(it);
			m_layInsertIndex--;
		}
	}

	void LayerStack::popOverlayer(Layer* overlayer_)
	{
		auto it = std::find(m_layerStack.begin() + m_layInsertIndex, m_layerStack.end(), overlayer_);
		if (it != m_layerStack.end())
		{
			overlayer_->onDetach();
			m_layerStack.erase(it);
		}
	}
}
