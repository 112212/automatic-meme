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
	m_last = 0;
	m_slider_size = 5;
	m_is_vertical = false;
	m_is_readonly = false;
	m_on_it = false;
	m_value = 0;
	m_slider_max = 100;
	m_mwheel_const = 5; // 5% promena default :)
	
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
	// pomeraj slidera
	m_slider_pix = (m_value * ((m_is_vertical ? m_rect.h : m_rect.w) - m_slider_size)) / m_slider_max;
	
	if( m_is_vertical ) {
		m_slider.setSize( sf::Vector2f( m_rect.w, m_slider_size ) );
		m_slider.setPosition( m_rect.x, m_rect.y + m_slider_pix );
	}
	else {
		m_slider.setSize( sf::Vector2f( m_slider_size, m_rect.h ) );
		m_slider.setPosition( m_rect.x + m_slider_pix, m_rect.y );
	}
	
}
void ScrollBar::onPositionChange() {
	//onChange();
	m_outline.setPosition( m_rect.x, m_rect.y );
	m_outline.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	updateSlider();
}
#elif USE_SDL
void ScrollBar::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
	#ifdef SELECTION_MARK
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, isSelected ? Colors::Yellow : Colors::White );
	#else
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, Colors::White );
	#endif
		
	
	Drawing::Rect(m_slider.x+pos.x, m_slider.y+pos.y, m_slider.w, m_slider.h, Colors::White);
	// boxColor(ren, m_slider.x+pos.x, m_slider.y+pos.y, pos.x+m_slider.x+m_slider.w, pos.y+m_slider.y+m_slider.h, Colors::White);
	// rectangleColor(ren, m_rect.x+pos.x, m_rect.y+pos.y, m_rect.x+pos.x+m_rect.w, pos.y+m_rect.y+m_rect.h, Colors::White);
}

void ScrollBar::updateSlider() {
	// pomeraj slidera
	m_slider_pix = (m_value * ((m_is_vertical ? m_rect.h : m_rect.w) - m_slider_size)) / m_slider_max;
	
	if( m_is_vertical ) {
		m_slider = {m_rect.x, m_rect.y + m_slider_pix, m_rect.w, m_slider_size};
	}
	else {
		m_slider = {m_rect.x + m_slider_pix, m_rect.y, m_slider_size, m_rect.h};
	}
	
}
void ScrollBar::onPositionChange() {
	updateSlider();
}
#endif

void ScrollBar::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_readonly) return;
	
	if(mouseState) {
		if(m_is_vertical) {
			int y = mY - m_rect.y  - m_slider_size/2;
			if(y < 0) y=0;
			else if(y > m_rect.h - m_slider_size) y = m_rect.h - m_slider_size;
			if(y != m_slider_pix) {
				m_slider_pix = y;
				onChange();
			}
		} else {
			int x = mX - m_rect.x  - m_slider_size/2;
			if(x < 0) x = 0;
			else if(x > m_rect.w - m_slider_size) x = m_rect.w - m_slider_size;
			if(x != m_slider_pix) {
				m_slider_pix = x;
				onChange();
			}
		}
	} else {
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

void ScrollBar::OnMouseDown( int mX, int mY ) {
	if(m_is_readonly) return;
	
	if(m_is_vertical) {
		int y = mY - m_rect.y - m_slider_size/2;
		if(y < 0) y=0;
		else if(y > m_rect.h - m_slider_size) y = m_rect.h - m_slider_size;
		if(y != m_slider_pix) {
			m_slider_pix = y;
			onChange();
		}
	} else {
		int x = mX - m_rect.x - m_slider_size/2;
		if(x < 0) x = 0;
		else if(x > m_rect.w - m_slider_size) x = m_rect.w - m_slider_size;
		if(x != m_slider_pix) {
			m_slider_pix = x;
			onChange();
		}
	}
}

int ScrollBar::GetPercentageValue() {
	return (m_slider_pix*100) / ((m_is_vertical ? m_rect.h : m_rect.w) - m_slider_size);
}

float ScrollBar::GetPercentageValueFloat() {
	return (m_slider_pix) / (float)((m_is_vertical ? m_rect.h : m_rect.w) - m_slider_size);
}

int ScrollBar::getValue() {
	return (m_slider_pix*m_slider_max) / ((m_is_vertical ? m_rect.h : m_rect.w) - m_slider_size);
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
	if( m_is_vertical ) {
		r.x = m_rect.x;
		r.y = m_rect.y + m_slider_pix;
		r.w = m_rect.w;
		r.h = m_slider_size;
	} else {
		r.x = m_rect.x + m_slider_pix;
		r.y = m_rect.y;
		r.h = m_rect.h;
		r.w = m_slider_size;
	}
	return r;
}



void ScrollBar::OnMWheel( int updown ) {
	m_value = std::max<int>(0, std::min<int>(m_slider_max, m_value - updown * m_mwheel_const));
	updateSlider();
	emitEvent( EVENT_SCROLLBAR_CHANGE );
}

void ScrollBar::setValue( int value ) {
	int last = m_value;
	// filter
	m_value = std::min( std::max(value, 0), m_slider_max );

	if(last != m_value) {
		updateSlider();
	}
}



void ScrollBar::SetValue( int value ) {
	setValue( value );
	// resetovati difference
	m_last = value;
}

int ScrollBar::GetDifference() {
	int val = m_value;
	int val_return = val - m_last;
	m_last = val;
	return val_return;
}

void ScrollBar::SetMaxRange( int vmax ) {
	m_slider_max = vmax;
}
}
