#ifndef NG_SCROLLBAR_HPP
#define NG_SCROLLBAR_HPP

#include "../Control.hpp"
namespace ng {
enum {
	EVENT_SCROLLBAR_CHANGE
};

class ScrollBar : public Control {
	private:
		int m_slider_pix;
		int m_slider_size;
		int m_slider_max;
		
		int m_last;
		bool m_on_it;
		bool m_is_vertical;
		bool m_is_readonly;
		int m_value;
		int m_mwheel_const;
		
		Rect getSliderRect();
		void onChange();
		void onPositionChange();
		void setValue( int value );
		int getValue();
		void updateSlider();
		#ifdef USE_SFML		
			sf::RectangleShape m_outline;
			sf::RectangleShape m_slider;
		#elif USE_SDL
			SDL_Rect m_slider;
		#endif
		
	public:
		ScrollBar();
		~ScrollBar();
		
		#ifdef USE_SFML
			void Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected );
		#elif USE_SDL
			void Render( SDL_Rect position, bool isSelected );
		#endif
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY );
		void OnLostFocus();
		void OnMWheel( int updown );
		
		
		void SetVertical( bool isVertical ) { m_is_vertical = isVertical; }
		
		int GetPercentageValue();
		float GetPercentageValueFloat();
		int GetValue();
		int GetDifference();
		void SetValue( int value );
		void SetSliderSize( int s ) { m_slider_size = s; }
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		void SetMaxRange( int vmax );
		void SetMouseWheelConstant( int mw ) { m_mwheel_const = mw; }
};
}
#endif
