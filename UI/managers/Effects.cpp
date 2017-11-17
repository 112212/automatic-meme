#include <iostream>

#include "Effects.hpp"
#include "../Gui.hpp"
namespace ng {
namespace Effects {
	AutoFade::AutoFade(double delay, double fade_speed) : Effect("AutoFade") {
		this->delay = delay;
		timer = 0;
		fading = true;
		alpha = 0.0;
		appearing = false;
		this->fade_speed = fade_speed;	
	}
	
	void AutoFade::Init() {
		// std::cout << "init effect " << name << " for " << control->GetId() << "\n";
		control->OnEvent("leave", [&](Args& args) {
			timer_count = true;
			appearing = true;
			// std::cout << "leave " << control->GetId() << "\n";
		});
		control->OnEvent("hover", [&](Args& args) {
			fading = false;
			appearing = true;
			timer_count = false;
			// std::cout << "enter " << control->GetId() << "\n";
		});
	}
	
	void AutoFade::PreRender() {
		double dt = delta_time;
		if(timer_count) {
			timer += dt;
			if(timer > delay) {
				timer = 0;
				fading = true;
				timer_count = false;
			}
		}
		
		if(fading) {
			alpha = std::max(0.0, alpha - fade_speed * dt);
			Drawing().SetMaxAlpha(alpha);
		} else if(appearing) {
			alpha = std::min(1.0, alpha + fade_speed * dt);
			Drawing().SetMaxAlpha(alpha);
		}
		
	}
	void AutoFade::PostRender() {
		if(fading || appearing) {
			Drawing().SetMaxAlpha(1.0f);
		}
	}
	
	void AutoFade::Appear() {
		appearing = true;
		fading = false;
		timer_count = true;
		timer = 0;
	}
	
	// -----------------------------------------
	
	void Move::Init() {
		const Rect &r = control->GetRect();
		start_point = Point(r.x,r.y);
	}
	
	void Move::PreRender() {
		const Rect &r = control->GetRect();
		double p = time / end_time;
		// control->SetPosition( r.x + (target_point.x - r.x) * p,
							  // r.y + (target_point.y - r.y) * p );
		control->SetPosition( start_point.x + (target_point.x - start_point.x) * p,
							  start_point.y + (target_point.y - start_point.y) * p );
	}
	
	void Move::PostRender() {
		
	}
	
	Move::Move(Point target_point, double time) : OneShotEffect("Move") {
		this->target_point = target_point;
		end_time = time;
	}
	
	// ----------------------------------
	
	void Resize::Init() {
		const Rect &r = control->GetRect();
		start_size = Size(r.w,r.h);
	}
	
	void Resize::PreRender() {
		const Rect &r = control->GetRect();
		double p = time / end_time;
		control->SetSize( start_size.w + (target_size.w - start_size.w) * p,
						  start_size.h + (target_size.h - start_size.h) * p );
	}
	
	void Resize::PostRender() {
		
	}
	
	Resize::Resize(Point target_size, double time) : OneShotEffect("Resize") {
		this->target_size = target_size;
		end_time = time;
	}
	
}
}
