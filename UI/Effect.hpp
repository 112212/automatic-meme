#ifndef NG_EFFECT_HPP
#define NG_EFFECT_HPP
#include <string>
#include <iostream>
#include "Control.hpp"
#include "backend/Screen.hpp"
#include "backend/Speaker.hpp"
#include "backend/System.hpp"
namespace ng {
class Gui;
class Effect {
	private:
		virtual void prerender() {
			PreRender();
		}
		virtual void postrender() {
			PostRender();
		}
		
	protected:
		friend class Control;
		std::string name;
		Gui* gui;
		Control* control;
		double delta_time;
		
		bool is_exclusive;

		virtual void Init() {};
		virtual void PreRender() = 0;
		virtual void PostRender() = 0;
		
		inline Screen& Drawing() { return control->Drawing(); }
		inline Speaker& Sound() { return control->Sound(); }
		inline System& GetSystem() { return control->GetSystem(); }
		
	public:
		Effect() : name("unknown_effect"), is_exclusive(true) {}
		Effect(std::string name) : name(name), is_exclusive(true) {}
};

class OneShotEffect : public Effect {
	private:
		std::function<void()> m_on_remove_effect;
		virtual void prerender() {
			time += delta_time;
			if(time > end_time) {
				time = end_time;
			}
			PreRender();
		}
		virtual void postrender() {
			PostRender();
			if(time >= end_time) {
				// std::cout << "removing effect " << name << "\n";
				if(m_on_remove_effect) {
					m_on_remove_effect();
				}
				OnRemove();
				control->RemoveEffect(this);
			}
		}
	protected:
		double time;
		double end_time;
		
		virtual void Init() {};
		virtual void PreRender() = 0;
		virtual void PostRender() = 0;
		virtual void OnRemove() {}
	public:
		void SetOnRemoveEffect(std::function<void()> f) {
			m_on_remove_effect = f;
		}
		OneShotEffect(std::string name, double end_time) : Effect(name), end_time(end_time), time(0) {}
};

}

#endif
