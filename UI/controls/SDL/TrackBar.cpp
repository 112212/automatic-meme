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
	m_slider_radius = 10;
	m_is_vertical = false;
	show_num = true;
	m_is_readonly = false;
	m_on_it = false;
	m_value = 0;
	m_font = Fonts::GetFont( "default", 13 );
	tex_text = 0;
	m_bordercolor = 0;
}

TrackBar::~TrackBar() {
}

void TrackBar::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;

	if(m_is_vertical) {
		int x2 = x + rect.w/2;
		Drawing::Line( x2, y, x2, y+rect.h, Colors::White);
		if(m_marks > 0) {
			int dh = (float)rect.h / (float)m_marks;
			int y;
			for(int i=0; i <= m_marks; i++) {
				y = y + dh*i;
				Drawing::Line(x2-5, y, x2+5, y, Colors::White);
			}
		}
		Drawing::FillCircle( x2, y + rect.h - m_slider_pix, m_slider_radius, m_on_it ? Colors::Yellow : Colors::White);
	} else {
		int h = y+rect.h/2;
		Drawing::Line(x, h, x+rect.w, h, Colors::White);
		if(m_marks > 0) {
			float dw = (float)rect.w / (float)m_marks;
			for(int i=0; i <= m_marks; i++) {
				Drawing::Line(x + dw*i, h-5, x + dw*i, h+5, Colors::White);
			}
		}
		Drawing::FillCircle( x + m_slider_pix, h - 1, m_slider_radius, m_on_it ? Colors::Yellow : Colors::White);
	}
	
	Control::Render(pos,isSelected);
	
	if(tex_text != 0) {
		if(m_is_vertical) {
			pos.x += m_slider_radius+2;
			pos.y += m_slider_radius/2;
		} else {
			pos.y += m_slider_radius/2;
		}
		Drawing::TexRect(m_text_rect.x+pos.x, m_text_rect.y+pos.y, m_text_rect.w, m_text_rect.h, tex_text);
	}
	
}


void TrackBar::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(m_is_readonly || !canChange()) return;
	const Rect& rect = GetRect();
	if(mouseState) {
		if(m_is_vertical) {
			int y = rect.h - (mY - rect.y);
			if(y < 0) y=0;
			else if(y > rect.h) y = rect.h;
			if(y != m_slider_pix) {
				m_slider_pix = y;
				onChange();
			}
		} else {
			int x = mX - rect.x;
			if(x < 0) x = 0;
			else if(x > rect.w) x = rect.w;
			if(x != m_slider_pix) {
				m_slider_pix = x;
				onChange();
			}
		}
	} else {
		Rect sr = getSliderRect();
		int x = mX - (sr.x+sr.w/2);
		int y = mY - (sr.y+sr.h/2);
		if(x*x + y*y < m_slider_radius*m_slider_radius) {
			m_on_it = true;
			return;
		}
		m_on_it = false;
	}
}

void TrackBar::STYLE_FUNC(value) {
	STYLE_SWITCH {
		_case("value"):
			setValue( std::stoi(value) );
		_case("range"): {
				size_t i = value.find(",");
				if(i == std::string::npos)
					SetRange( 0, std::stoi(value) );
				else
					SetRange( std::stoi(value.substr(0,i)), std::stoi(value.substr(i+1)) );
			}
		_case("orientation"):
			if(value == "vertical" || value == "horizontal") {
				SetVertical( value == "vertical" ? true : false );
				updateSlider();
			}
		_case("slider_size"):
			m_slider_radius = std::stoi(value);
			updateTextLocation();
			updateSlider();
		_case("show_number"):
			show_num = (value == "true");
			if(tex_text != 0)
				Drawing::DeleteTexture(tex_text);
			tex_text = 0;
	}
}

void TrackBar::OnMouseDown( int mX, int mY ) {
	if(m_is_readonly || !canChange()) return;
	const Rect& rect = GetRect();
	if(m_is_vertical) {
		int y = rect.h - (mY - rect.y);
		if(y < 0) y=0;
		else if(y > rect.h) y = rect.h;
		if(y != m_slider_pix) {
			m_slider_pix = y;
			onChange();
		}
	} else {
		int x = mX - rect.x;
		if(x < 0) x = 0;
		else if(x > rect.w) x = rect.w;
		if(x != m_slider_pix) {
			m_slider_pix = x;
			onChange();
		}
	}
}

int TrackBar::GetPercentageValue() {
	return (m_slider_pix*100) / (m_is_vertical ? GetRect().h : GetRect().w);
}

int TrackBar::getValue() {
	return m_slider_min+(m_slider_pix*m_slider_max) / (m_is_vertical ? GetRect().h : GetRect().w);
}

int TrackBar::GetValue() {
	return m_value;
}

void TrackBar::OnLostFocus() {
	m_on_it = false;
}

void TrackBar::OnLostControl() {
	m_on_it = false;
}

void TrackBar::SetRange( int _min, int _max ) {
	if(m_slider_max+m_slider_min == _max && m_slider_min == _min) return;
	m_slider_min = std::max(0,_min);
	m_slider_max = std::max(_max-_min, 0);
	
	int last = m_value;
	m_value = std::min( std::max( m_value, m_slider_min ), m_slider_max+m_slider_min );
	updateSlider();
	onChange();
}

void TrackBar::onChange() {
	m_value = getValue();
	
	if(show_num) {
		SDL_Surface* surf_num = TTF_RenderText_Blended( m_font, std::to_string( m_value ).c_str(), {255,255,255} );
		tex_text = Drawing::GetTextureFromSurface(surf_num, tex_text);
		m_text_rect.w = surf_num->w;
		m_text_rect.h = surf_num->h;
		SDL_FreeSurface( surf_num );
		updateTextLocation();
	}
	
	emitEvent( EVENT_TRACKBAR_CHANGE );
}

void TrackBar::updateTextLocation() {
	
	Rect sl = getSliderRect();
	if(m_is_vertical) {
		m_text_rect.y = sl.y;
		m_text_rect.x = sl.x + 15 - m_text_rect.w/2;
	} else {
		m_text_rect.x = sl.x + 10 - m_text_rect.w/2;
		m_text_rect.y = sl.y + 15;
	}
}

Rect TrackBar::getSliderRect() {
	Rect r;
	const Rect& rect = GetRect();
	if( m_is_vertical ) {
		r.x = rect.x + rect.w/2 - m_slider_radius;
		r.y = rect.y + rect.h - m_slider_pix - m_slider_radius;
	} else {
		r.x = rect.x + m_slider_pix - m_slider_radius;
		r.y = rect.y+rect.h/2-m_slider_radius;
	}
	r.w = m_slider_radius*2;
	r.h = m_slider_radius*2;
	return r;
}

void TrackBar::onPositionChange() {
	onChange();
}

void TrackBar::setValue( int value ) {
	m_value = std::min( std::max(value, m_slider_min), m_slider_max );
	updateSlider();
}

void TrackBar::updateSlider() {
	int last = m_slider_pix;
	if(m_slider_max > 0) {
		m_slider_pix = ((m_value - m_slider_min) * (m_is_vertical ? GetRect().h : GetRect().w)) / (m_slider_max-m_slider_min);
	}
	if(m_slider_pix != last) {
		updateTextLocation();
	}
}

void TrackBar::SetValue( int value ) {
	setValue( value );
}

bool TrackBar::canChange() {
	return (m_slider_max != 0);
}
}
