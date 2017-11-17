#include "../ColorString.hpp"
#include "ProgressBar.hpp"

#define SCROLLBAR_MIN 0
#define SCROLLBAR_MAX 100

namespace ng {

ProgressBar::ProgressBar(std::string id) {
	SetId(id);
	setType( "progressbar" );
	m_style.font = Fonts::GetFont( "default", 13 );
	m_value = 0;
	m_style.hoverborder_color = m_style.border_color;
	bar_color = 0xffffffff;
	progress_text.tex = 0;
}

ProgressBar::~ProgressBar() {
}

void ProgressBar::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	int text_space = 20;
	int brightness = 100;
	
	Drawing().FillRect(r.x+pos.x, r.y+pos.y, m_value*r.w/100, r.h, bar_color > 0 ? bar_color : Color::GetColor(brightness - (m_value * brightness / 100), m_value * brightness / 100, 0, 255));
	Drawing().TexRect(r.x+pos.x + r.w / 2 - progress_text.w/2, r.y+pos.y, progress_text.w, progress_text.h, progress_text.tex, false, progress_text.w, progress_text.h);
	
	Control::Render(pos, isSelected);
}


void ProgressBar::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetValue( std::stoi(value) );
		_case("bar_color"):
			bar_color = Color::ParseColor(value);
	}
}

ProgressBar* ProgressBar::Clone() {
	ProgressBar* s = new ProgressBar();
	copyStyle(s);
	s->bar_color = bar_color;
	return s;
}

int ProgressBar::GetValue() {
	return m_value;
}


void ProgressBar::SetValue( int value ) {
	ColorString str(std::to_string(value) + "%");
	Image* img = str.get_image(m_style.font, 0);
	m_value = value;
}


}
