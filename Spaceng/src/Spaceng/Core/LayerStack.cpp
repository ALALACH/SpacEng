#include "PCH.h"
#include "LayerStack.h"


namespace Spaceng {
	
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		for (Layer* Layer : m_Layers)
			delete Layer;
	}

	void LayerStack::PushLayer(Layer* Layer)
	{
		m_Layers.emplace(m_Layers.begin()+ m_LayerIndex ,Layer);
		m_LayerIndex++; 
	}

	void LayerStack::PushOverLay(Layer* Layer)
	{
		m_Layers.emplace_back(Layer);
	}

	void LayerStack::PopLayer(Layer* Layer)
	{
		auto X_Layer = std::find(m_Layers.begin(), m_Layers.end(), Layer);
		if (X_Layer != m_Layers.end())
		{
			m_Layers.erase(X_Layer);
			m_LayerIndex--;
		}
	}
	void LayerStack::PopOverLay(Layer* Layer)
	{
		auto X_layer = std::find(m_Layers.begin(), m_Layers.end(), Layer);
		if (X_layer != m_Layers.end())
			m_Layers.erase(X_layer);
	}
}