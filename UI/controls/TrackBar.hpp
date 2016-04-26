#ifndef NG_TRACKBAR_HPP
#define NG_TRACKBAR_HPP

#ifdef USE_SDL
	#include "SDL/TrackBar.hpp"
#elif USE_SFML
#include "../Control.hpp"



typedef Point Range;
namespace ng {
enum {
	EVENT_TRACKBAR_CHANGE
}
class TrackBar : public Control {
	private:
		sf::Text m_text_num;
		sf::CircleShape m_circle;
		sf::RectangleShape m_rect1;
		sf::RectangleShape m_rectShape;
		int m_marks;
		int m_slider_min;
		int m_slider_max;
		int m_slider_pix;
		int m_slider_radius;
		Point m_text_loc;
		
		int m_last;
		bool m_on_it;
		bool m_is_vertical;
		bool m_is_readonly;
		int m_value;
		
		Rect getSliderRect();
		void onChange();
		void onPositionChange();
		void setValue( int value );
		int getValue();
		void updateSlider();
		void updateTextLocation();
		bool canChange();
		static int m_font_index;
		
		
	public:
		TrackBar();
		~TrackBar();
		
		void Render( sf::RenderTarget& ren, sf::RenderStates states, bool isSelected );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY );
		void OnLostFocus();
		
		
		void SetVertical( bool isVertical ) { m_is_vertical = isVertical; onChange(); }
		
		int GetPercentageValue();
		int GetValue();
		int GetDifference();
		Range GetRange() { return (Range){m_slider_min,m_slider_max}; }
		int GetRangeMax() { return m_slider_max; }
		void SetValue( int value );
		
		void SetRange( int min, int max );
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		
};
}
#endif
#endif
