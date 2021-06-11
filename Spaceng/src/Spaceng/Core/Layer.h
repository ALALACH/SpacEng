#pragma once 
#include "Event.h"
#include <string>

namespace Spaceng {

	class Layer {
	public:
		Layer(const std::string Name = "Layer");
		~Layer();
		
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate(float Timestep) = 0;
		virtual void OnEvent(Event& Event) = 0;

		inline const std::string& GetName() const { return m_DebugName;}

	protected:
		std::string m_DebugName;
	};
}