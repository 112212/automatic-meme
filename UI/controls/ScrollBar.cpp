#ifdef USE_SDL
#include "../common/SDL/Drawing.hpp"
#endif
#include "ScrollBar.hpp"

#define SCROLLBAR_MIN 0
#define SCROLLBAR_MAX 100

namespace ng {

ScrollBar::ScrollBar() {
	setType( TYPE_SCROLLBAR );
	initEventVector(1);
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
	// m_bordercolor = 0xffffffff;
		
	#ifdef USE_SFML
		m_outline.setFillColor( sf::Color::Transparent );
		m_outline.setOutlineColor( sf::Color::White );
		m_outline.setOutlineThickness( 1 );
	#elif USE_SDL
	#endif
}

ScrollBar::~ScrollBar() {
}

#ifdef USE_SFML
	void ScrollBar::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {
		ren.draw(m_slider, state);
		ren.draw(m_outline, state);
	}
	void ScrollBar::updateSlider() {
		if( m_is_vertical ) {
			m_slider.setSize( sf::Vector2f( m_rect.w, m_slider_size ) );
			m_slider.setPosition( m_rect.x, m_rect.y + m_slider_pix );
		} else {
			m_slider.setSize( sf::Vector2f( m_slider_size, m_rect.h ) );
			m_slider.setPosition( m_rect.x + m_slider_pix, m_rect.y );
		}
	}
	void ScrollBar::onPositionChange() {
		m_outline.setPosition( m_rect.x, m_rect.y );
		m_outline.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
		updateSlider();
	}
	
#elif USE_SDL

void ScrollBar::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	Drawing::FillRect(m_slider.x+pos.x, m_slider.y+pos.y, m_slider.w, m_slider.h, Colors::White);
	Control::Render(pos, isSelected);
}

#endif

void ScrollBar::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			SetValue( std::stoi(value) );
		_case("range"): {
			size_t i = value.find(",");
			if(i == std::string::npos)
				SetRange( 0, std::stoi(value) );
			else
				SetRange( std::stoi(value.substr(0,i)), std::stoi(value.substr(i+1)) );
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

void ScrollBar::onPositionChange() {
	updateSlider();
}

void ScrollBar::OnMouseUp( int mX, int mY ) {
	sendGuiCommand( GUI_UNLOCK );
}

ScrollBar* ScrollBar::Clone() {
	ScrollBar* s = new ScrollBar;
	*s = *this;
	return s;
}

void ScrollBar::OnMouseDown( int mX, int mY ) {
	if(m_is_readonly) return;
	const Rect& rect = GetRect();
	m_slider_click_offset = 0;
	
	m_on_it = false;
	Rect sr = getSliderRect();
	if( mX > sr.x && mX < sr.x + sr.w ) {
		if( mY > sr.y && mY < sr.y + sr.h ) {
			m_on_it = true;
		}
	}
	
	m_slider_click_offset = 0;
	if(m_on_it)
		sendGuiCommand( GUI_FOCUS_LOCK );
	
	if(m_is_vertical) {
		int y = mY - rect.y - m_slider_size/2;
		if(y != m_slider_pix) {
			if(m_on_it)
				m_slider_click_offset = y - m_slider_pix;
			else {
				m_on_it = true;
				m_slider_pix = std::max(0, std::min(rect.h - m_slider_size, y - m_slider_click_offset));
				onChange();
			}
				
		}
	} else {
		int x = mX - rect.x - m_slider_size/2;
		if(x != m_slider_pix) {
			if(m_on_it)
				m_slider_click_offset = x - m_slider_pix;
			else {
				m_on_it = true;
				m_slider_pix = std::max(0, std::min(rect.w - m_slider_size, x - m_slider_click_offset));
				onChange();
			}
		}
	}
}

void ScrollBar::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_readonly) return;
	const Rect& r = GetRect();
	
	if(m_on_it && mouseState) {
		if(m_is_vertical) {
			int y = mY - r.y  - m_slider_size/2;
			if(y != m_slider_pix) {
				m_slider_pix = std::max(0, std::min(r.h - m_slider_size, y - m_slider_click_offset));
				onChange();
			}
		} else {
			int x = mX - r.x  - m_slider_size/2;
			if(x != m_slider_pix) {
				m_slider_pix = std::max(0, std::min(r.w - m_slider_size, x - m_slider_click_offset));
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
	return (m_slider_pix*100.0f) / (float)((m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size);
}

int ScrollBar::getValue() {
	// f( m_slider_pix )
	return (m_slider_pix*(m_max_value-m_min_value)) / ((m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size) + m_min_value;
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
	emitEvent( EVENT_SCROLLBAR_CHANGE );
}

Rect ScrollBar::getSliderRect() {
	Rect r;
	const Rect& rect = GetRect();
	if( m_is_vertical ) {
		r.x = rect.x;
		r.y = rect.y + m_slider_pix;
		r.w = rect.w;
		r.h = m_slider_size;
	} else {
		r.x = rect.x + m_slider_pix;
		r.y = rect.y;
		r.h = rect.h;
		r.w = m_slider_size;
	}
	return r;
}

void ScrollBar::OnMWheel( int updown ) {
	m_value = std::max<int>(m_min_value, std::min<int>(m_max_value, m_value - updown * m_mwheel_const));
	m_slider_pix = ((m_value-m_min_value) * ( (m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size) ) / (m_max_value-m_min_value);
	updateSlider();
	emitEvent( EVENT_SCROLLBAR_CHANGE );
}

void ScrollBar::setValue( int value ) {
	m_value = std::min( std::max(value, m_min_value), m_max_value );
	m_slider_pix = ((m_value-m_min_value) * ( (m_is_vertical ? GetRect().h : GetRect().w) - m_slider_size) ) / (m_max_value-m_min_value);
	updateSlider();
}

void ScrollBar::SetValue( int value ) {
	setValue( value );
}

void ScrollBar::SetRange( int min, int max ) {
	m_min_value = min;
	m_max_value = max;
}

}
