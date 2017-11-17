#ifndef EFFECT_HPP
#define EFFECT_HPP
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

		virtual void Init() {};
		virtual void PreRender() = 0;
		virtual void PostRender() = 0;
		
		inline Screen& Drawing() { return control->Drawing(); }
		inline Speaker& Sound() { return control->Sound(); }
		inline System& GetSystem() { return control->GetSystem(); }
		
	public:
		Effect() : name("unknown_effect") {}
		Effect(std::string name) : name(name) {}
};

class OneShotEffect : public Effect {
	private:
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
				std::cout << "removing effect " << name << "\n";
				control->RemoveEffect(this);
			}
		}
	protected:
		double time;
		double end_time;
		
		virtual void Init() {};
		virtual void PreRender() = 0;
		virtual void PostRender() = 0;
	public:
		OneShotEffect(std::string name) : Effect(name) {}
};

}

#endif
