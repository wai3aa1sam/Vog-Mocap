#pragma once
#include "vogpch.h"
#include "Vog/Core/Core.h"
#include "Vog/Core/Layer.h"

// iterator for each / revEach reference
// https://github.com/SimpleTalkCpp/SimpleTalkCpp_Tutorial/blob/main/Advance%20C%2B%2B/001%20MyVector/MyVector.h

namespace vog {

	class VOG_API LayerStack 
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void destroy();

		void pushLayer(Layer* layer_);
		void popLayer(Layer* layer_);

		void pushOverlayer(Layer* overlayer_);
		void popOverlayer(Layer* overlayer_);

#pragma region iterator_overload
		std::vector<Layer*>::iterator begin() { return m_layerStack.begin(); }
		std::vector<Layer*>::iterator end() { return m_layerStack.end(); }

		std::vector<Layer*>::const_iterator begin() const { return m_layerStack.cbegin(); }
		std::vector<Layer*>::const_iterator end() const { return m_layerStack.cend(); }

		std::vector<Layer*>::reverse_iterator rbegin() { return m_layerStack.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_layerStack.rend(); }

		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_layerStack.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return m_layerStack.rend(); }

		template<typename A>
		class RevEnumerator_
		{
		public:
			RevEnumerator_(A begin_, A end_)
				:
				m_begin(begin_), m_end(end_)
			{}

			A begin() { return m_begin; }
			A end() { return m_end; }

		private:
			A m_begin;
			A m_end;
		};

		using RevEnumerator = RevEnumerator_<std::vector<Layer*>::reverse_iterator>;
		using CRevIterator = RevEnumerator_<std::vector<Layer*>::const_reverse_iterator>;

		RevEnumerator revEach() { return { rbegin(), rend() }; }
		CRevIterator revEach() const { return { rbegin(), rend() }; }
#pragma endregion

	private:
		std::vector<Layer*> m_layerStack;
		uint32_t m_layInsertIndex = 0;
	};

}