#ifndef NG_SCROLLBAR_HPP
#define NG_SCROLLBAR_HPP

#include "../Control.hpp"
namespace ng {

class ScrollBar : public Control {
	private:
		int m_slider_pix;
		int m_slider_size;
		
		bool m_on_it;
		bool m_is_vertical;
		bool m_is_readonly;
		int m_min_value;
		int m_max_value;
		int m_value;
		int m_mwheel_const;
		int m_slider_click_offset;
		
		Rect getSliderRect();
		void onChange();
		void onRectChange();
		void setValue( int value );
		int getValue();
		void updateSlider();
		Rect m_slider;
		void OnSetStyle(std::string& style, std::string& value);
	public:
		ScrollBar();
		~ScrollBar();
		
		void Render( Point position, bool isSelected );
		
		ScrollBar* Clone();
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY, MouseButton button );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void OnLostFocus();
		void OnMWheel( int updown );
		
		void SetVertical( bool isVertical ) { m_is_vertical = isVertical; }
		
		float GetPercentageValue();
		int GetValue();
		void SetValue( int value );
		void SetSliderSize( int s );
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		void SetRange( int min, int max );
		void SetMouseWheelConstant( int mw ) { m_mwheel_const = mw; }
};
}
#endif
