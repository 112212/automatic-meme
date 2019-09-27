#ifndef NG_TRACKBAR_HPP
#define NG_TRACKBAR_HPP

#include "../Control.hpp"
namespace ng {




class TrackBar : public Control {
	private:
		int m_marks;
		int m_slider_min;
		int m_slider_max;
		int m_slider_pix;
		int m_slider_radius;
		Rect m_text_rect;
		bool show_num;
		
		bool m_on_it;
		bool m_is_vertical;
		bool m_is_readonly;
		int m_value;
		
		Rect getSliderRect();
		void onChange(bool force=false);
		void onRectChange();
		void setValue( int value );
		int getValue();
		void updateSlider();
		void updateTextLocation();
		bool canChange();
		Font* m_font;
		Image* tex_text;
		void OnSetStyle(std::string& style, std::string& value);
		
	public:
	
		TrackBar();
		~TrackBar();
		
		TrackBar* Clone();
		void Render( Point pos, bool isSelected );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY, MouseButton button );
		void OnLostFocus();
		
		void SetVertical( bool isVertical ) { m_is_vertical = isVertical; onChange(); }
		int GetPercentageValue();
		int GetValue();
		Range GetRange() { return (Range){m_slider_min,m_slider_max}; }
		int GetRangeMax() { return m_slider_max; }
		void SetValue( int value );
		void SetRange( int min, int max );
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		
};
}
#endif
