#pragma once 
#include "Event.h"
#include <string>

namespace Spaceng {

	class Layer {
	public:
		Layer(const std::string Name = "Layer");
		~Layer();
		
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float Timestep) {}
		virtual void OnEvent(Event& Event) {}

		inline const std::string& GetName() const { return m_DebugName;}

	protected:
		std::string m_DebugName;
	};
}