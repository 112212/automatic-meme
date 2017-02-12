#include "common/common.hpp"
#include "common/ControlManager.hpp"
#include "Control.hpp"
#include <iostream>

#include <SDL2/SDL_image.h>

#include "Gui.hpp"
#include "Widget.hpp"

#include <algorithm>

#ifdef USE_SDL
	#include "common/SDL/Drawing.hpp"
#endif

namespace ng {
Control::Control() : id("0"), engine(0), widget(0), 
z_index(0), type("control"), isWidget(false),
interactible(true), visible(true), anchor({{0,0},0}) 
{
	m_rect.x = m_rect.y = 0;
	m_rect.w = m_rect.h = 50;
	m_style.border_color = 0xff333376;
	m_style.hoverborder_color = 0xff1228D1;
	m_style.background_color = 0;
	m_style.font = Fonts::GetFont( "default", 13 );
	m_style.image_tex = 0xffffffff;
	m_style.alpha = 1.0f;
	m_style.draggable = false;
}

Control::~Control() {
}

const Point Control::GetOffset() {
	if(engine)
		return engine->sel_widget_offset;
	else
		return {0,0};
}

const Point Control::GetGlobalPosition() {
	Point ofs = GetOffset();
	ofs.x += m_rect.x;
	ofs.y += m_rect.y;
	return ofs;
}

void Control::SetDraggable( bool draggable ) {
	m_style.draggable = draggable;
}

void Control::SetZIndex( int zindex ) {
	if(zindex == z_index) return;
	if(widget) {
		widget->setZIndex(this, zindex);
	} else if(engine) {
		engine->setZIndex(this, zindex);
	}
}

void Control::update_cache(CacheUpdateFlag flag) {
	if(widget) {
		widget->updateCache(this,flag);
	} else if(engine) {
		engine->updateCache(this,flag);
	}
}

bool Control::isSelected() {
	return engine && engine->selected_control == this;
}

bool Control::isActive() {
	return engine && engine->active_control == this;
}

void Control::SetVisible(bool visible) {
	if(this->visible != visible) {
		this->interactible = visible;
		this->visible = visible;
		if(!visible && engine && engine->active_control == this) {
			engine->active_control = 0;
		}
		update_cache(CacheUpdateFlag::attributes);
		if(!visible && isWidget && ((Widget*)this)->inSelectedBranch() ) {
			sendGuiCommand(GUI_UNSELECT_WIDGETS);
		}
	}
}

void Control::SetRenderable(bool visible) {
	if(this->visible != visible) {
		this->visible = visible;
		update_cache(CacheUpdateFlag::attributes);
	}
}

void Control::setInteractible(bool interactible) {
	if(this->interactible != interactible) {
		this->interactible = interactible;
		update_cache(CacheUpdateFlag::attributes);
	}
}

void Control::SetId(std::string id) {
	this->id = id;
}


void Control::setType( const char* type ) {
	this->type = type;
	update_cache(CacheUpdateFlag::all);
}

// emit global event
void Control::emitEvent( std::string event_id, Argv& argv ) {
	#ifdef USE_EVENT_QUEUE
	if(engine) {
		engine->m_events.push( { event_id, this, argv } );
	}
	#endif
	
	for(auto sub : subscribers) {
		if(sub.event_id == event_id) {
			std::vector<std::string> v;
			v.reserve(argv.size() + sub.args.size());
			v.insert(v.begin(), argv.begin(), argv.end());
			v.insert(v.end(), sub.args.begin(), sub.args.end());
			GuiEngine::function_map[ sub.function_name ](this, v);
		}
	}
	
}

void Control::Focus() {
	if(engine) {
		engine->Focus(this);
	}
}

void Control::Activate() {
	if(engine) {
		engine->Activate(this);
	}
}

bool Control::IsSelected() {
	if(IsWidget()) {
		return ((Widget*)this)->inSelectedBranch();
	} else {
		return engine && engine->GetSelectedControl() == this;
	}
}

static int last_anon_id = 0;
void Control::OnEvent( std::string event_type, std::function<void(Control*, Argv&)> callback ) {
	std::string anon_id = std::string("_anon") + std::to_string(last_anon_id++);
	subscribers.emplace_back(event_type, anon_id);
	GuiEngine::function_map[ anon_id ] = callback;
}

void Control::sendGuiCommand( int eventId ) {
	if(engine) {
		engine->processControlEvent(eventId);
	}
}

void Control::tabToNextControl() {
	std::vector<Control*> controls = getParentControls();
	std::sort(controls.begin(), controls.end(), [](Control* a, Control* b) -> bool {
		return a->GetRect().y < b->GetRect().y || (a->GetRect().y == b->GetRect().y && a->GetRect().x <= b->GetRect().x);
	});
	auto f = std::find(controls.begin(), controls.end(), this);
	auto p = std::find_if(f+1, controls.end(), [](Control* c) { return c->interactible; });
	if(p == controls.end())
		p = std::find_if(controls.begin(), f, [](Control* c) { return c->interactible; });
	if(p != f) {
		(*p)->Activate();
	}
}

std::vector<Control*> empty_vector;

const std::vector<Control*>& Control::getParentControls() {
	if(widget)
		return widget->controls;
	else if(engine)
		return engine->GetControls();
	else
		return empty_vector;
}

const std::vector<Control*>& Control::getWidgetControls() {
	if(widget)
		return widget->controls;
	else
		return empty_vector;
}
const std::vector<Control*>& Control::getEngineControls() {
	if(engine)
		return engine->GetControls();
	else
		return empty_vector;
}

bool Control::check_collision(int x, int y) {
	Rect& r = m_rect;
	return (x >= r.x && x <= r.x+r.w && y >= r.y && y <= r.y+r.h);
}

void Control::SetRect( int x, int y, int w, int h ) {

	if(isWidget)
		((Widget*)this)->setRect(x,y,w,h);
	
	m_rect.x = x;
	m_rect.y = y;
	m_rect.w = w;
	m_rect.h = h;

	Widget* widget = getWidget();
	GuiEngine* engine = getEngine();
	if(widget) {
		widget->updateCache(this, CacheUpdateFlag::position);
	} else if(engine) {
		engine->updateCache(this, CacheUpdateFlag::position);
	}
	
	onPositionChange();
}

bool Control::custom_check = false;

void Control::SetTooltip(std::string tooltip) {
	this->m_style.tooltip = tooltip;
}

void Control::SetRect( Rect r ) { 
	SetRect(r.x, r.y, r.w, r.h);
}

void Control::SetAnchor( float W, float w, float x, float H, float h, float y ) {
	this->anchor = (Anchor){ .coord = anchor.coord, .x = x, .y = y, .W = W, .w = w, .H = H, .h = h };
}

void Control::SetAnchor( Rect r ) {
	this->anchor = Anchor{{0,0},0};
	this->anchor.x = r.x;
	this->anchor.y = r.y;
	this->anchor.w_min[0] = r.w;
	this->anchor.h_min[0] = r.h;
}

void Control::SetAnchor( const Anchor& anchor ) {
	this->anchor = anchor;
}
const Anchor& Control::GetAnchor() {
	return anchor;
}


// make these virtuals optional
void Control::OnMouseMove( int mX, int mY, bool mouseState ) {}
void Control::OnMouseDown( int mX, int mY, MouseButton button ) {}
void Control::OnMouseUp( int mX, int mY, MouseButton button ) {}
#ifdef USE_SFML
	void Control::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {}
	void Control::OnKeyDown( sf::Event::KeyEvent &sym ) {}
	void Control::OnKeyUp( sf::Event::KeyEvent &sym ) {}
#elif USE_SDL
	void Control::Render( Point pos, bool selected ) {
		pos.x += m_rect.x;
		pos.y += m_rect.y;
		if(m_style.alpha != 1.0f) {
			Drawing::SetMaxAlpha(m_style.alpha);
		}
		Drawing::FillRect( pos.x, pos.y, m_rect.w, m_rect.h, m_style.background_color );
		if(m_style.image_tex > 0) {
			Drawing::TexRect(pos.x, pos.y, m_rect.w, m_rect.h, 
				m_style.image_tex, m_style.image_repeat, m_style.image_size.w, m_style.image_size.h);
		}
		#ifdef SELECTION_MARK
			Drawing::Rect(pos.x, pos.y, m_rect.w, m_rect.h, 
				selected ? m_style.hoverborder_color : m_style.border_color );
		#else
			Drawing::Rect(pos.x, pos.y, m_rect.w, m_rect.h, m_bordercolor );
		#endif
	}
	void Control::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {}
	void Control::OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod ) {}
	void Control::SetFont( std::string name, int size ) { 
		TTF_Font* f = Fonts::GetFont(name, size); 
		if(f) m_style.font = f; 
	}
#endif
void Control::OnLostFocus() {}
void Control::onPositionChange() {}
void Control::onFontChange() {}

void Control::OnGetFocus() {}
void Control::OnLostControl() {}
void Control::OnMWheel( int updown ) {}
bool Control::customBoundary( int x, int y ) {}

void Control::Unselect() {
	if(engine && (engine->GetSelectedControl() == this)) {
		engine->UnselectControl();
		engine->unselectWidget();
	}
}

Control* Control::Clone() {
	Control* c = new Control;
	copyStyle(c);
	return c;
}

void Control::copyStyle(Control* copy_to) {
	copy_to->m_style = this->m_style;
}

void Control::SetAlpha(float alpha) {
	m_style.alpha = alpha;
}

static void split_string(const std::string& str, std::vector<std::string>& values, char delimiter) {
	size_t prevpos = 0;
	size_t pos = str.find(delimiter, prevpos);
	while(pos != str.npos) {
		int len = pos-prevpos;
		if(len != 0)
			values.push_back( str.substr(prevpos, len) );
		prevpos = pos + 1;
		pos = str.find(delimiter, prevpos);
	}
	if(prevpos < str.size())
		values.push_back( str.substr(prevpos) );
}


void monomial_expr_parse(const char* str, const char *names, float *result) {
	int current_index=0;
	const char* s = str;
	int names_length = strlen(names);
	int is_first = 1;
	int is_minus = 0;
	memset(result, 0, sizeof(float)*(names_length+1));
	float res = 0;
	while(1) {
		const char c = *s;
		if(c == '\0' || c == '+' || c == '-') {
			if(c == '-') {
				is_minus = 1;
			}
			result[current_index] += res;
			is_first = 1;
			if(c == '\0')
				break;
			res = 0;
			current_index = 0;
		} else if(isdigit(c) || c == '.') {
			float num = strtof(s, (char**)&s);
			if(is_minus) {
				is_minus = 0;
				num = -num;
			}
			if(is_first) {
				res = num;
				is_first = 0;
			} else {
				res *= num;
			}
			continue;
		} else if(c == '%') {
			res *= 0.01;
		} else {
			int i;
			for(i=0; i < names_length; i++) {
				if(names[i] == c) {
					if(res == 0) res = 1;
					current_index=i+1;
					break;
				}
			}
		}
		s++;
	}
}

Anchor Anchor::parseRect(std::string s) {
	Anchor a{{0,0},0};
	std::vector<std::string> parts;
	split_string(s, parts, ',');
	const char* names[] = { "wW", "hH", "W", "H" };
	float res[4];
	int p = 0;
	if(s[0] == 'a') {
		std::cout << "ABS\n";
		a.absolute_coordinates = true;
	}
	a.w_func = a.h_func = fit;
	
	a.w_min[0] = 0;
	a.w_max[0] = 9999;
	a.w_min[1] = 0;
	a.w_max[1] = 0;
	
	a.h_min[0] = 0;
	a.h_max[0] = 9999;
	a.h_min[1] = 0;
	a.h_max[1] = 0;
					
	for(int i=0; i < parts.size(); i++) {
		const std::string& cur = parts[i];
		if(p <= 1) {
			monomial_expr_parse(cur.c_str(), names[p], res);
			std::cout << "parsing rect(" << p << "):" << cur << " => " << res[0] << ", " << res[1] << ", " << res[2] << std::endl;
			size_t pos;
			if(p == 0) {
				if((pos=cur.find_first_of("LR")) != std::string::npos) {
					if(cur[pos] == 'L') {
						std::cout << "found L\n";
						a.absolute_coordinates = true;
					} else if(cur[pos] == 'R') {
						std::cout << "found R\n";
						a.W += 1;
						a.w += -1;
						a.absolute_coordinates = true;
					}
				}
				a.x += res[0];
				a.w += res[1];
				a.W += res[2];
			} else if(p == 1) {
				if((pos=cur.find_first_of("UDB")) != std::string::npos) {
					if(cur[pos] == 'U') {
						a.absolute_coordinates = true;
					} else if(cur.find_first_of("DB",pos) != std::string::npos) {
						a.H += 1;
						a.h += -1;
						a.absolute_coordinates = true;
						std::cout << "found B\n";
					}
				}
				a.y += res[0];
				a.h += res[1];
				a.H += res[2];
			}
		} else if(p >= 2) {
			size_t pos = 0;
			
			if((pos = parts[i].find("(", pos)) != std::string::npos) {
					monomial_expr_parse(parts[i].c_str()+pos, names[p], res);
					if(i+1 >= parts.size() || parts[i].find(")", pos+1) != std::string::npos) {
						// has only min
						if(p == 2) {
							a.w_min[0] = res[0];
							a.w_min[1] += res[1];
						} else {
							a.h_min[0] = res[0];
							a.h_min[1] += res[1];
						}
						continue;
					}
					if(p == 2) {
						a.w_min[0] = res[0];
						a.w_min[1] += res[1];
					} else {
						a.h_min[0] = res[0];
						a.h_min[1] += res[1];
					}
					monomial_expr_parse(parts[i+1].c_str(), names[p], res);
					if(p == 2) {
						a.w_max[0] = res[0];
						a.w_max[1] += res[1];
					} else {
						a.h_max[0] = res[0];
						a.h_max[1] += res[1];
					}
					
					// std::cout << "parsing rect(" << parts[i+1] << "):" << cur << " => " << res[0] << ", " << res[1] << std::endl;
					i++;
			} else {
				monomial_expr_parse(cur.c_str(), names[p], res);
				std::cout << "parsing rect(" << p << "): " << cur << " => " << res[0] << ", " << res[1] << std::endl;
				if(p == 2) {
					a.w_min[0] += res[0];
					a.w_min[1] += res[1];
					a.w_max[0] = a.w_min[0];
					a.w_max[1] = a.w_min[1];
					a.w_func = Anchor::SizeFunction::none;
				} else if(p == 3) {
					a.h_min[0] += res[0];
					a.h_min[1] += res[1];
					a.h_max[0] = a.h_min[0];
					a.h_max[1] = a.h_min[1];
					a.h_func = Anchor::SizeFunction::none;
				}
			}
			auto set_func = [&](Anchor::SizeFunction f){
				if(p == 2) {
					a.w_func = f;
				} else if(p == 3) {
					a.h_func = f;
				}
			};
			
			if(parts[i].find("keep", pos) != std::string::npos) {
				set_func(Anchor::SizeFunction::keep);
			} else if(parts[i].find("fit", pos) != std::string::npos) {
				set_func(Anchor::SizeFunction::fit);
			} else if(parts[i].find("expand", pos) != std::string::npos) {
				set_func(Anchor::SizeFunction::expand);
			}
		}
		p++;
	}
	std::cout << a << std::endl;
	return a;
}


#ifdef USE_SDL
void Control::SetImage(std::string image, bool repeat) {
	SDL_Surface* surf = IMG_Load(image.c_str());
	m_style.image_size.w = surf->w;
	m_style.image_size.h = surf->h;
	if(surf) {
		m_style.image_tex = Drawing::GetTextureFromSurface2(surf, m_style.image_tex);
		SDL_FreeSurface(surf);
	}
	
	m_style.image_repeat = repeat;
}
#endif



void Control::SetStyle(std::string style, std::string value) {
	const Rect& r = GetRect();
	STYLE_SWITCH {
		_case("rect"): {
			Anchor a = Anchor::parseRect(value);
			SetAnchor(a);
			SetRect(a.x, a.y, a.w_min[0], a.h_min[0]);
		}
		_case("zindex"):
			SetZIndex(std::stoi(value));
		_case("id"):
			SetId(value);
		_case("interactible"):
			setInteractible(value == "true");
		_case("visible"):
			SetVisible(value=="true");
		_case("bordercolor"):
			m_style.border_color = Colors::ParseColor(value);
		_case("background_color"):
			m_style.background_color = Colors::ParseColor(value);
		_case("hoverbordercolor"):
			m_style.hoverborder_color = Colors::ParseColor(value);
		_case("border"):
		_case("font"): {
			TTF_Font* f = Fonts::GetParsedFont(value);
			if(f) {
				m_style.font = f;
			}
			onFontChange();
		}
		_case("tooltip"):
			m_style.tooltip = value;
		_case("image"): {
			size_t pos = value.find(',');
			if(pos != std::string::npos && value.substr(pos+1) == "repeat") {
				SetImage(value.substr(0, pos), true);
			} else {
				SetImage(value, false);
			}
		}
		_case("draggable"): {
			m_style.draggable = value == "true";
		}
			break;
		default:
			// if style start with on_...
			if(tolower(style[0]) == 'o' && tolower(style[1]) == 'n') {
				// parse value and add calls to subscribers
				std::string type = style.substr(2);
				std::vector<std::string> commands;
				std::vector<std::string> params;
				split_string(value, commands, ';');
				for(auto &cmd : commands) {
					params.clear();
					split_string(cmd, params, ' ');
					subscribers.emplace_back( type, params[0] );
					subscribers.back().args.insert(subscribers.back().args.begin(), params.begin()+1, params.end());
				}
			}
			
			OnSetStyle(style, value);
	}

}

void Control::STYLE_FUNC(value) {}

Anchor& Anchor::operator+=(const Anchor& b) {
	W += b.W;
	w += b.w;
	x += b.x;
	
	H += b.H;
	h += b.h;
	y += b.y;
	
	absolute_coordinates |= b.absolute_coordinates;
	
	return *this;
}

std::ostream& operator<< (std::ostream& stream, const Anchor& a) {
	return stream << a.W << ", " << a.w << ", " << a.x << " ; " <<
		a.H << ", " << a.h << ", " << a.y << " ; " <<
		a.w_min[1] << "+" << a.w_min[0] << ", " << a.h_min[1] << "+" << a.h_min[0] << " ; (" << 
		a.coord.x << ", " << a.coord.y << " a: " << a.absolute_coordinates << std::endl;
}


}


