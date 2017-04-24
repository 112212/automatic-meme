#ifdef USE_SDL
#include "../../common/SDL/Drawing.hpp"
#endif
#include "../../common/Colorstring.hpp"
#include "ProgressBar.hpp"

#define SCROLLBAR_MIN 0
#define SCROLLBAR_MAX 100

namespace ng {

ProgressBar::ProgressBar() {
	setType( "progressbar" );
	m_style.font = Fonts::GetFont( "default", 13 );
	m_value = 0;
	m_style.hoverborder_color = m_style.border_color;
	bar_color = 0xffffffff;
	progress_text.tex = 0xffffffff;
}

ProgressBar::~ProgressBar() {
}

void ProgressBar::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	int text_space = 20;
	Drawing::FillRect(r.x+pos.x, r.y+pos.y, m_value*r.w/100, r.h, bar_color);
	Drawing::TexRect(r.x+pos.x + m_value*r.w/100 + text_space, r.y+pos.y, progress_text.w, progress_text.h, progress_text.tex, false, progress_text.w, progress_text.h);
	
	Control::Render(pos, isSelected);
}


void ProgressBar::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetValue( std::stoi(value) );
		_case("bar_color"):
			bar_color = Colors::ParseColor(value);
	}
}

ProgressBar* ProgressBar::Clone() {
	ProgressBar* s = new ProgressBar();
	copyStyle(s);
	return s;
}

int ProgressBar::GetValue() {
	return m_value;
}


void ProgressBar::SetValue( int value ) {
	Colorstring str(std::to_string(value) + "%");
	SDL_Surface* surf = str.get_surface(m_style.font, 0);
	progress_text.tex = Drawing::GetTextureFromSurface( surf, progress_text.tex );
	progress_text.w = surf->w;
	progress_text.h = surf->h;
	SDL_FreeSurface(surf);
	m_value = value;
}


}
