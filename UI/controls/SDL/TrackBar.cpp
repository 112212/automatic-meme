#include "TrackBar.hpp"
#include "../../common/SDL/Drawing.hpp"
namespace ng {
TrackBar::TrackBar() {
	setType( TYPE_TRACKBAR );
	initEventVector(1);
	m_marks = 0;
	m_slider_min = 0;
	m_slider_max = 100;
	m_slider_pix = 0;
	m_last = 0;
	m_slider_radius = 5;
	m_is_vertical = false;
	m_surf_num = 0;
	m_is_readonly = false;
	m_on_it = false;
	m_value = 0;
	m_font = Fonts::GetFont( "default", 13 );
}

TrackBar::~TrackBar() {
	SDL_FreeSurface( m_surf_num );
}

void TrackBar::Render( SDL_Rect pos, bool isSelected ) {
	int x = m_rect.x + pos.x;
	int y = m_rect.y + pos.y;
	#ifdef SELECTION_MARK
	if(isSelected)
		Drawing::Rect( x, y, m_rect.w, m_rect.h, Colors::Yellow);
	#endif
	if(m_is_vertical) {
		int x2 = x + m_rect.w/2;
		Drawing::Line( x2, y, x2, y+m_rect.h, Colors::White);
		if(m_marks > 0) {
			int dh = (float)m_rect.h / (float)m_marks;
			int y;
			for(int i=0; i <= m_marks; i++) {
				y = y + dh*i;
				Drawing::Line(x2-5, y, x2+5, y, Colors::White);
			}
		}
		Drawing::FillCircle( x2, y + m_rect.h - m_slider_pix, 5, m_on_it ? Colors::Yellow : Colors::White);
	} else {
		int h = y+m_rect.h/2;
		Drawing::Line(x, h, x+m_rect.w, h, Colors::White);
		if(m_marks > 0) {
			float dw = (float)m_rect.w / (float)m_marks;
			for(int i=0; i <= m_marks; i++) {
				Drawing::Line(x + dw*i, h-5, x + dw*i, h+5, Colors::White);
			}
		}
		Drawing::FillCircle( x + m_slider_pix, h - 1, 10, m_on_it ? Colors::Yellow : Colors::White);
	}
	if(m_surf_num) {
		// TODO: fix this
		// CSurface::OnDraw( ren, m_surf_num, m_text_loc.x, m_text_loc.y );
	}
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
		SDL_Rect sr = getSliderRect();
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
	m_value = std::min( std::max( m_value, m_slider_min ), m_slider_max );
	if(last != m_value) {
		//cout << "chae of rae caused chae of value ... " << endl;
		onChange();
	}
	
	updateSlider();
}

void TrackBar::onChange() {
	m_value = getValue();
	
	if(m_surf_num)
		SDL_FreeSurface( m_surf_num );
	m_surf_num = TTF_RenderText_Solid( m_font, SSTR( m_value ).c_str(), {255,255,255} );
		
	updateTextLocation();
	
	emitEvent( EVENT_TRACKBAR_CHANGE );
	//cout << "%: " << GetPercentageValue() << endl;
	//cout << "val: " << GetValue() << endl;
}

void TrackBar::updateTextLocation() {
	
	SDL_Rect sl = getSliderRect();
	if(m_is_vertical) {
		m_text_loc.y = sl.y;
		m_text_loc.x = sl.x + 15;
	} else {
		m_text_loc.x = sl.x;
		m_text_loc.y = sl.y + 15;
	}
}

SDL_Rect TrackBar::getSliderRect() {
	SDL_Rect r;
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
	onChange();
}

void TrackBar::setValue( int value ) {
	int last = m_value;
	// filter
	m_value = std::min( std::max(value, m_slider_min), m_slider_max );

	if(last != m_value) {
		if(m_surf_num)
		SDL_FreeSurface( m_surf_num );
		m_surf_num = TTF_RenderText_Solid( m_font, SSTR( m_value ).c_str(), {255,255,255} );
		updateSlider();
	}
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
