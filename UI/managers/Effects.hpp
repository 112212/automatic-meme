#ifndef NG_EFFECTS_HPP
#define NG_EFFECTS_HPP
#include "../Effect.hpp"
namespace ng {
namespace Effects {
		
	class AutoFade : public Effect {
		private:
			double delay;
			double timer;
			double alpha;
			double fade_speed;
			bool fading;
			bool appearing;
			bool timer_count;
		
			void Init();
			void PreRender();
			void PostRender();
			
			
		public:
			/*
				fade_speed - how much alpha loss per sec
			*/
			AutoFade(double delay=5, double fade_speed=0.5);
			void Appear();
	};
	
	class Move : public OneShotEffect {
		private:
			Point start_point;
			Point target_point;
			void Init();
			void PreRender();
			void PostRender();
		public:
			Move(Point target_point, double time);
	};
	
	class Resize : public OneShotEffect {
		private:
			Size start_size;
			Point target_size;
			void Init();
			void PreRender();
			void PostRender();
		public:
			Resize(Size target_size, double time);
	};
		
}
}

#endif
