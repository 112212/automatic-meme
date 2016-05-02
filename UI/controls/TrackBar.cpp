#include "TrackBar.hpp"
namespace ng {
TrackBar::TrackBar() {
	setType( TYPE_TRACKBAR );
	m_marks = 0;
	m_slider_min = 0;
	m_slider_max = 100;
	m_slider_pix = 0;
	m_last = 0;
	m_slider_radius = 5;
	m_is_vertical = false;
	m_is_readonly = false;
	m_on_it = false;
	m_value = 0;
	
	m_circle.setRadius( 5 );
	m_rectShape.setFillColor( sf::Color::Transparent );
	m_rectShape.setOutlineColor( sf::Color::White );
	m_rectShape.setOutlineThickness( 1 );
	
	m_text_num.setFont( Fonts::GetFont( "default" ) );
	m_text_num.setCharacterSize( 13 );
}

TrackBar::~TrackBar() {
	
}

void TrackBar::Render( sf::RenderTarget& ren, sf::RenderStates states, bool isSelected ) {
	//~ ren.draw( m_rectShape );
	ren.draw( m_rect1, states );
	ren.draw( m_circle, states );
	ren.draw( m_text_num, states );
}


void TrackBar::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_readonly || !canChange()) return;
	
	if(mouseState) {
		if(m_is_vertical) {
			int y = m_rect.h - (mY - m_rect.y);
			if(y < 0) y=0;
			else if(y > m_rect.h) y = m_rect.h;
			if(y != m_slider_pix) {
				m_slider_pix = y;
				onChange();
			}
		} else {
			int x = mX - m_rect.x;
			if(x < 0) x = 0;
			else if(x > m_rect.w) x = m_rect.w;
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

void TrackBar::OnMouseDown( int mX, int mY ) {
	if(m_is_readonly || !canChange()) return;
	
	if(m_is_vertical) {
		int y = m_rect.h - (mY - m_rect.y);
		if(y < 0) y=0;
		else if(y > m_rect.h) y = m_rect.h;
		if(y != m_slider_pix) {
			m_slider_pix = y;
			onChange();
		}
	} else {
		int x = mX - m_rect.x;
		if(x < 0) x = 0;
		else if(x > m_rect.w) x = m_rect.w;
		if(x != m_slider_pix) {
			m_slider_pix = x;
			onChange();
		}
	}
}

int TrackBar::GetPercentageValue() {
	return (m_slider_pix*100) / (m_is_vertical ? m_rect.h : m_rect.w);
}

int TrackBar::getValue() {
	return m_slider_min+(m_slider_pix*m_slider_max) / (m_is_vertical ? m_rect.h : m_rect.w);
}

int TrackBar::GetValue() {
	return m_value;
}

void TrackBar::OnLostFocus() {
	m_on_it = false;
}

void TrackBar::SetRange( int _min, int _max ) {
	if(m_slider_max == _max && m_slider_min == _min) return;
	m_slider_min = std::max(0,_min);
	m_slider_max = std::max(_max-_min, 0);
	
	int last = m_value;
	// vrednost mora da ostane u granicama
	m_value = std::min( std::max( m_value, m_slider_min ), m_slider_max+m_slider_min );
	if(last != m_value) {
		//cout << "chae of rae caused chae of value ... " << endl;
		onChange();
	}
	
	updateSlider();
}

void TrackBar::onChange() {
	m_value = getValue();
	if(m_is_vertical)
		m_circle.setPosition( m_rect.x + m_rect.w*0.2, m_rect.y + m_slider_pix );
	else
		m_circle.setPosition( m_rect.x + m_slider_pix, m_rect.y + m_rect.h*0.2 );
		
	/*
	if(m_surf_num)
		SDL_FreeSurface( m_surf_num );
	if( m_font_index >= 0 )
		m_surf_num = TTF_RenderText_Solid( CFonts::GetFont( m_font_index ), SSTR( m_value ).c_str(), CColors::s_white );
	*/
	
	setValue( getValue() );
	updateTextLocation();
	
	emitEvent( EVENT_TRACKBAR_CHANGE );
	//cout << "%: " << GetPercentageValue() << endl;
	//cout << "val: " << GetValue() << endl;
}

void TrackBar::updateTextLocation() {
	
	Rect sl = getSliderRect();
	if(m_is_vertical) {
		m_text_loc.y = sl.y;
		m_text_loc.x = sl.x + 10;
	} else {
		m_text_loc.x = sl.x;
		m_text_loc.y = sl.y + 10;
	}
	m_text_num.setPosition( m_text_loc.x, m_text_loc.y );
}

Rect TrackBar::getSliderRect() {
	Rect r;
	if( m_is_vertical ) {
		r.x = m_rect.x + m_rect.w/2 - m_slider_radius;
		r.y = m_rect.y + m_rect.h - m_slider_pix - m_slider_radius;
	} else {
		r.x = m_rect.x + m_slider_pix - m_slider_radius;
		r.y = m_rect.y+m_rect.h/2-m_slider_radius;
	}
	r.w = m_slider_radius*2;
	r.h = m_slider_radius*2;
	return r;
}

void TrackBar::onPositionChange() {
	if( m_is_vertical ) {
		m_rect1.setSize( sf::Vector2f( 0.5, m_rect.h ) );
		m_rect1.setPosition( m_rect.x + m_rect.w*0.25, m_rect.y );
	} else {
		m_rect1.setSize( sf::Vector2f( m_rect.w, 0.5 ) );
		m_rect1.setPosition( m_rect.x, m_rect.y + m_rect.h*0.25 );
	}
	m_rectShape.setPosition( m_rect.x, m_rect.y );
	m_rectShape.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	onChange();
}

void TrackBar::setValue( int value ) {
	int last = m_value;
	// filter
	m_value = std::min( std::max(value, m_slider_min), m_slider_max );

	//~ if(last != m_value) {
		/*
		if(m_surf_num)
		SDL_FreeSurface( m_surf_num );
		if( m_font_index >= 0 )
			m_surf_num = TTF_RenderText_Solid( CFonts::GetFont( m_font_index ), SSTR( m_value ).c_str(), CColors::s_white );
		*/
		char buff[10];
		buff[9] = 0;
		binary_to_bcd( value, buff, 0, 9 );
		//~ debug( buff );
		m_text_num.setStri( std::string( buff ) );
	
		updateSlider();
	//~ }
}

void TrackBar::updateSlider() {
	int last = m_slider_pix;
	// pomeraj slidera
	if(m_slider_max > 0)
		m_slider_pix = (m_value * (m_is_vertical ? m_rect.h : m_rect.w) - m_slider_min) / m_slider_max;
		
	if(m_slider_pix != last) {
		updateTextLocation();
	}
}

void TrackBar::SetValue( int value ) {
	setValue( value );
	
	// resetovati difference
	m_last = value;
}

int TrackBar::GetDifference() {
	int val = m_value;
	int val_return = val - m_last;
	m_last = val;
	return val_return;
}

bool TrackBar::canChange() {
	return (m_slider_max != m_slider_min);
}
}
