#include "ScrollBar.hpp"
#include <iostream>
#define SCROLLBAR_MIN 0
#define SCROLLBAR_MAX 100

namespace ng {

ScrollBar::ScrollBar() {
	setType( "scrollbar" );
	m_slider_pix = 0;
	m_slider_size = 5;
	m_is_vertical = false;
	m_is_readonly = false;
	m_on_it = false;
	m_value = 0;
	m_min_value = 0;
	m_max_value = 100;
	m_mwheel_const = 4;
	m_slider_click_offset = 0;
	m_slider_color = Color::White;
	m_is_dragging = false;
}

ScrollBar::~ScrollBar() {
}

void ScrollBar::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	Control::Render(pos, isSelected);
	Drawing().FillRect(m_slider.x+pos.x, m_slider.y+pos.y, m_slider.w, m_slider.h, m_slider_color);
}

void ScrollBar::SetSliderSize( int s ) {
	m_slider_size = s;
	if(m_is_vertical) {
		m_slider.h = m_slider_size;
	} else {
		m_slider.w = m_slider_size;
	}
	updateSlider();
}

void ScrollBar::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetValue( std::stoi(value) );
		_case("range"): {
			size_t i = value.find(",");
			if(i == std::string::npos) {
				SetRange( 0, std::stoi(value) );
			} else {
				SetRange( std::stoi(value.substr(0,i)), std::stoi(value.substr(i+1)) );
			}
		}
		_case("minvalue"):
			m_min_value = std::stoi(value);
		_case("maxvalue"):
			m_max_value = std::stoi(value);
		_case("slider_size"):
			m_slider_size = std::stoi(value);
			if(m_is_vertical) {
				m_slider.h = m_slider_size;
			} else {
				m_slider.w = m_slider_size;
			}
			updateSlider();
		_case("vertical"):
			SetVertical( toBool(value) );
			updateSlider();
		_case("orientation"):
			if(value == "vertical" || value == "horizontal") {
				SetVertical( value == "vertical" );
				updateSlider();
			}
	}
}

void ScrollBar::updateSlider() {
	const Rect& r = GetRect();
	if( m_is_vertical ) {
		m_slider = {r.x, r.y + m_slider_pix, r.w, m_slider_size};
	} else {
		m_slider = {r.x + m_slider_pix, r.y, m_slider_size, r.h};
	}
}

void ScrollBar::onRectChange() {
	updateSlider();
}

void ScrollBar::OnMouseUp( int mX, int mY, MouseButton button ) {
	// sendGuiCommand( GUI_UNLOCK );
	m_is_dragging = false;
}

ScrollBar* ScrollBar::Clone() {
	ScrollBar* s = new ScrollBar();
	copyStyle(s);
	return s;
}

void ScrollBar::SetVisibleRange(int range) {
	Rect r = GetRect();
	int slider_size = m_is_vertical ? r.h : r.w;
	if(m_min_value == m_max_value) {
		SetSliderSize( slider_size  );
	} else {
		SetSliderSize( clip( slider_size * range / (m_max_value - m_min_value), 10, slider_size ) );
	}
}

bool ScrollBar::IsDraggingSlider() {
	return m_is_dragging;
}

void ScrollBar::OnMouseDown( int mX, int mY, MouseButton button ) {
	if(m_is_readonly) {
		return;
	}
	
	const Rect& rect = GetRect();
	m_slider_click_offset = 0;
	
	m_on_it = false;
	Rect sr = getSliderRect();
	if( mX > sr.x && mX < sr.x + sr.w ) {
		if( mY > sr.y && mY < sr.y + sr.h ) {
			m_on_it = true;
		}
	}
	
	m_is_dragging = true;
	
	m_slider_click_offset = 0;
	if(m_on_it) {
		// TODO: check why did i use this??
		// sendGuiCommand( GUI_FOCUS_LOCK );
	}
	
	if(m_is_vertical) {
		int y = mY - m_slider_size/2;
		
		if(y != m_slider_pix) {
			if(m_on_it) {
				
				m_slider_click_offset = y - m_slider_pix;
			} else {
				m_on_it = true;
				m_slider_pix = clip(y - m_slider_click_offset, 0, rect.h - m_slider_size);
				onChange();
			}
				
		}
	} else {
		// int x = mX - rect.x - m_slider_size/2;
		int x = mX - m_slider_size/2;
		if(x != m_slider_pix) {
			if(m_on_it) {
				m_slider_click_offset = x - m_slider_pix;
			} else {
				m_on_it = true;
				m_slider_pix = clip(x - m_slider_click_offset, 0, rect.w - m_slider_size);
				onChange();
			}
		}
	}
}

void ScrollBar::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_readonly) {
		return;
	}
	
	const Rect& r = GetRect();
	
	if(m_on_it && mouseState) {
		if(m_is_vertical) {
			int y = mY - m_slider_size/2;
			if(y != m_slider_pix) {
				m_slider_pix = clip(y - m_slider_click_offset, 0, r.h - m_slider_size);
				onChange();
			}
		} else {
			int x = mX  - m_slider_size/2;
			if(x != m_slider_pix) {
				m_slider_pix = clip(x - m_slider_click_offset, 0, r.w - m_slider_size);
				onChange();
			}
		}
	} else if(!mouseState) {
		Rect sr = getSliderRect();
		if( mX > sr.x && mX < sr.x + sr.w ) {
			if( mY > sr.y && mY < sr.y + sr.h ) {
				m_on_it = true;
				return;
			}
		}
		m_on_it = false;
	}
}

float ScrollBar::GetPercentageValue() {
	float d = (m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size;
	if(d == 0.0f) {
		return 0;
	} else {
		return (m_slider_pix) / d;
	}
}

int ScrollBar::getValue() {
	int d = (m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size;
	if(d == 0) return 0;
	if(m_max_value == m_min_value) {
		return m_min_value;
	} else {
		return (m_slider_pix*(m_max_value-m_min_value)) / d + m_min_value;
	}
}

int ScrollBar::GetValue() {
	return m_value;
}

void ScrollBar::OnLostFocus() {
	m_on_it = false;
}

void ScrollBar::onChange() {
	m_value = getValue();
	updateSlider();
	emitEvent( "change", {m_value} );
}

Rect ScrollBar::getSliderRect() {
	Rect rect = GetRect();
	if( m_is_vertical ) {
		return Rect(0, m_slider_pix, rect.w, m_slider_size);
	} else {
		return Rect(m_slider_pix, 0, m_slider_size, rect.h);
	}
}

void ScrollBar::OnMWheel( int updown ) {
	setValue(m_value - updown * m_mwheel_const);
	emitEvent( "change", {m_value} );
}

void ScrollBar::setValue( int value ) {
	m_value = clip(value, m_min_value, m_max_value);
	int d = ( (m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size );
	if(m_min_value == m_max_value) {
		m_slider_pix = 0;
	} else {
		m_slider_pix = ( (m_value-m_min_value) * d ) / (m_max_value-m_min_value);
	}
	updateSlider();
}

void ScrollBar::SetValue( int value ) {
	setValue(value);
}

void ScrollBar::SetRange( int min, int max ) {
	m_min_value = min;
	m_max_value = max;
}

}
