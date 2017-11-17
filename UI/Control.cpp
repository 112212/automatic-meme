#include "common.hpp"
// #include "ControlManager.hpp"
#include "Control.hpp"
#include <iostream>

#include "Gui.hpp"
#include "Widget.hpp"
#include "Effect.hpp"

#include <algorithm>
#include "managers/Images.hpp"
#include <cstring>
#include "Border.hpp"
// #include "RapidXML/rapidxml.hpp"

namespace ng {
static int control_cnt = 0;
Control::Control() : id( "____" ), engine(0), widget(0), 
z_index(0), type("control"), isWidget(false),
interactible(true), visible(true) 
{
	render_enabled=true;
	id = "unnamed"+std::to_string((control_cnt++));
	m_rect.x = m_rect.y = 0;
	m_rect.w = m_rect.h = 50;
	m_style.border_color = 0xff333376;
	m_style.hoverborder_color = 0xff1228D1;
	// m_style.border_color = 0;
	// m_style.hoverborder_color = 0;
	m_style.background_color = 0;
	m_style.font = Fonts::GetFont( "default", 13 );
	m_style.image_tex = 0;
	m_style.alpha = 1.0f;
	m_style.draggable = false;
	// std::cout << "creating control: " << type << ", "  << this << "\n";
	SetBorder(new Border());
	layout = Layout::parseRect("0,0");
}

#define WE(x) if(widget) { widget->x; } else if(engine) { engine->x; }

Control::~Control() {
}

void Control::process_prerender_effects() {
	for(Effect* e : effects) {
		e->gui = engine;
		e->delta_time = engine->getDeltaTime();
		e->control = this;
		e->prerender();
	}
}

void Control::process_postrender_effects() {
	for(Effect* e : effects) {
		e->postrender();
	}
}

const Point Control::GetOffset() {
	if(engine) {
		return engine->sel_widget_offset;
	} else {
		return {0,0};
	}
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

void Control::SendToFront() {
	WE(sendToFront(this));
}
void Control::SendToBack() {
	WE(sendToBack(this));
}

void Control::SendUpper() {
	SetZIndex(GetZIndex()+1);
}

void Control::SendLower() {
	SetZIndex(GetZIndex()-1);
}

void Control::SendAfterControl(Control* c) {
	SetZIndex(c->GetZIndex()+1);
}

void Control::SendBeforeControl(Control* c) {
	SetZIndex(c->GetZIndex()-1);
}

void Control::update_cache(CacheUpdateFlag flag) {
	WE(updateCache(this,flag));
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
	// std::cout << "renaming " << this->id << " => " << id << " " << this << "\n";
	this->id = id;
}


void Control::setType( const char* type ) {
	this->type = type;
	update_cache(CacheUpdateFlag::all);
}

void Control::poseEmitEvent( Control* c, std::string event_id, const Argv& args ) {
	c->emitEvent(event_id, args);
}

// emit global event
void Control::emitEvent( std::string event_id, const Argv& argv ) {
	#ifdef USE_EVENT_QUEUE
	if(engine) {
		engine->m_events.push( { event_id, this, argv } );
	}
	#endif
	
	
	// std::cout << "evt " << event_id << "\n";
	for(auto sub : subscribers) {
		auto sub_func = Gui::function_map.find(sub.function_name);
		if(sub_func == Gui::function_map.end()) continue;
		
		if(sub.event_id == event_id) {
			Args args;
			args.event_args = argv;
			args.control = this;
			args.cmd_args = sub.args;
			
			
			sub_func->second(args);
			// std::cout << "emitting " << event_id << " " << args.event_args.size() << "\n";
		}
	}
	
}

double Control::getDeltaTime() {
	if(engine) {
		return engine->getDeltaTime();
	}
	return 0;
}

void Control::Focus() {
	if(engine) {
		engine->Focus(this);
	}
}


void Control::parseXml(rapidxml::xml_node<char>* node) {
	
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
void Control::onEvent( std::string event_type, EventCallback callback ) {
	std::string anon_id = std::string("_anon") + std::to_string(last_anon_id++);
	subscribers.emplace_back(event_type, anon_id);
	Gui::function_map[ anon_id ] = callback;
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
	if(p == controls.end()) {
		p = std::find_if(controls.begin(), f, [](Control* c) { return c->interactible; });
	}
	
	if(p != f) {
		(*p)->Activate();
	}
}

std::vector<Control*> empty_vector;

const std::vector<Control*>& Control::getParentControls() {
	if(widget) {
		return widget->controls;
	} else if(engine) {
		return engine->GetControls();
	} else {
		return empty_vector;
	}
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

void Control::SetPosition( int x, int y ) { 
	SetRect(x, y, m_rect.w, m_rect.h); 
}

void Control::SetSize( int w, int h ) {
	SetRect(m_rect.x, m_rect.y, w, h);
}

std::string Control::GetType() {
	return type;
}

std::string Control::GetId() {
	return id;
}

int Control::GetZIndex() {
	return z_index;
}

std::string Control::GetTooltip() {
	return m_style.tooltip;
}

bool Control::CheckCollision(int x, int y) {
	// std::cout << "Control [" << GetId() << "] " << this << " checking collision\n";
	bool check = m_border->CheckCollision(x,y);
	// std::cout << "Result [" << GetId() << "] checking collision: " << check << "\n";
	return check;
}

void Control::SetRect( int x, int y, int w, int h ) {

	if(m_rect.x == x && m_rect.y == y && m_rect.w == w && m_rect.h == h) {
		return;
	}
	if(isWidget) {
		static_cast<Widget*>(this)->setRect(x,y,w,h);
	}
	
	m_rect.x = x;
	m_rect.y = y;
	m_rect.w = w;
	m_rect.h = h;

	Widget* widget = getWidget();
	Gui* engine = getEngine();
	WE(updateCache(this, CacheUpdateFlag::position));
	
	onRectChange();
}

void Control::SetTooltip(std::string tooltip) {
	this->m_style.tooltip = tooltip;
}

void Control::SetRect( Rect r ) { 
	SetRect(r.x, r.y, r.w, r.h);
}

void Control::SetLayout( float W, float w, float x, float H, float h, float y ) {
	this->layout = Layout( layout.coord, x, y, W, w, H, h );
}

void Control::SetLayout( Rect r ) {
	this->layout = Layout();
	this->layout.x = r.x;
	this->layout.y = r.y;
	this->layout.w_min[0] = r.w;
	this->layout.h_min[0] = r.h;
}

void Control::SetLayout( const Layout& layout ) {
	this->layout = layout;
}
Layout& Control::GetLayout() {
	return layout;
}

void Control::AddEffect(Effect* effect) {
	effect->control = this;
	if(engine) {
		effect->gui = engine;
	}
	effect->Init();
	effects.push_back(effect);
}

void Control::RemoveEffect(Effect* effect) {
	auto it = std::remove(effects.begin(), effects.end(), effect);
	effects.resize(std::distance(effects.begin(), it));
}

Effect* Control::GetEffect(std::string effect_name) {
	auto it = std::find_if(effects.begin(), effects.end(), [&](Effect* e) {
		return e->name == effect_name;
	});
	if(it == effects.end()) {
		return 0;
	} else {
		return *it;
	}
}

void Control::RemoveEffect(std::string effect_name) {
	auto it = std::remove_if(effects.begin(), effects.end(), [&](Effect* e) {
		return e->name == effect_name;
	});
	effects.resize(std::distance(effects.begin(), it));
}

void Control::SetRenderEnabled(bool enable) {
	render_enabled = enable;
}


void Control::render(Point position, bool isSelected) {
	process_prerender_effects();
	if(render_enabled) {
		Render(position, isSelected);
	}
	process_postrender_effects();
}

// overridable
void Control::Render( Point pos, bool selected ) {
	pos = m_rect.Offset(pos);
	if(m_style.background_color != 0) {
		Drawing().FillRect( pos.x, pos.y, m_rect.w, m_rect.h, m_style.background_color );
	}
	
	if(m_style.image_tex) {
		Drawing().TexRect(pos.x, pos.y, m_rect.w, m_rect.h, m_style.image_tex, m_style.image_repeat, 
			m_style.image_size.w, m_style.image_size.h);
	}
	#ifdef SELECTION_MARK
		if(selected) {
			if(m_style.hoverborder_color != 0) {
				Drawing().Rect(pos.x, pos.y, m_rect.w, m_rect.h, m_style.hoverborder_color );
			}
		} else {
			if(m_style.border_color != 0) {
				Drawing().Rect(pos.x, pos.y, m_rect.w, m_rect.h, m_style.border_color );
			}
		}
	#else
		Drawing().Rect(pos.x, pos.y, m_rect.w, m_rect.h, m_bordercolor );
	#endif
}

void Control::SetFont( std::string name, int size ) { 
	Font* f = Fonts::GetFont(name, size); 
	if(f) {
		m_style.font = f; 
	}
}

// --- empty overridables ---
void Control::OnKeyDown( Keycode sym, Keymod mod ) {}
void Control::OnKeyUp( Keycode sym, Keymod mod ) {}
void Control::OnLostFocus() {}
void Control::onRectChange() {}
void Control::onFontChange() {}

void Control::OnGetFocus() {}
void Control::OnLostControl() {}
void Control::OnText( std::string s ) {}

void Control::OnMouseMove( int mX, int mY, bool mouseState ) {}
void Control::OnMouseDown( int mX, int mY, MouseButton button ) {}
void Control::OnMouseUp( int mX, int mY, MouseButton button ) {}
void Control::OnMWheel( int updown ) {}
// --------------------------

void Control::Unselect() {
	if(engine && (engine->GetSelectedControl() == this)) {
		engine->UnselectControl();
		engine->unselectWidget();
	}
}

void Control::Unattach() {
	if(widget) {
		widget->RemoveControl(this);
		widget = 0;
	} else if(engine) {
		engine->RemoveControl(this);
	}
	engine = 0;
}

Control* Control::Clone() {
	Control* c = new Control;
	copyStyle(c);
	return c;
}

Gui* Control::GetEngine() {
	return engine;
}

void Control::SetFont(Font* fnt) {
	if(fnt) {
		m_style.font = fnt;
	}
}

Font* Control::GetFont() {
	return m_style.font;
}

void Control::shareEngineBackend(Control* c) {
	c->engine = engine;
}

void Control::removeEngine(Control* c) {
	c->engine = 0;
}

bool Control::IsBeingDragged() {
	return engine->dragging && engine->selected_control == this;
}

void Control::cloneBase(Control* clone_to) {
	clone_to->subscribers.insert(clone_to->subscribers.end(), this->subscribers.begin(), this->subscribers.end());
}

void Control::CopyStyle(Control* copy_to) {
	copyStyle(copy_to);
}

void Control::copyStyle(Control* copy_to) {
	copy_to->m_style = this->m_style;
	cloneBase(copy_to);
}

void Control::SetAlpha(float alpha) {
	m_style.alpha = alpha;
}

void Control::SetBorder(Border* border) {
	m_border = border;
	m_border->m_rect = &m_rect;
	// m_border->control = this;
}

static void split_string(const std::string& str, std::vector<std::string>& values, char delimiter) {
	size_t prevpos = 0;
	size_t pos = str.find(delimiter, prevpos);
	while(pos != str.npos) {
		int len = pos-prevpos;
		if(len != 0) {
			values.push_back( str.substr(prevpos, len) );
		}
		prevpos = pos + 1;
		pos = str.find(delimiter, prevpos);
	}
	if(prevpos < str.size()) {
		values.push_back( str.substr(prevpos) );
	}
}


static void monomial_expr_parse(const char* str, const char *names, float *result) {
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

Layout::Layout( Point coord, float x, float y, float W, float w, float H, float h ) {
	this->coord = coord;
	this->x = x;
	this->y = y;
	this->W = W;
	this->w = w;
	this->H = H;
	this->h = h;
	this->w_func = SizeFunction::none;
	this->h_func = SizeFunction::none;
}

Layout::Layout() {
	coord = {0,0};
	x=0;y=0;
	W=0;w=0;H=0;h=0;
	absolute_coordinates=false;
	w_func = SizeFunction::none;
	h_func = SizeFunction::none;
	w_min[0]=w_min[1]=0;
	w_max[0]=w_max[1]=0;
	h_min[0]=h_min[1]=0;
	h_max[0]=h_max[1]=0;
}

void Layout::SetCoord( Point coord ) {
	this->coord = coord;
}

void Layout::SetPosition( float x, float y, float w, float W, float h, float H, bool absolute_coordinates ) {
	this->x = x;
	this->y = y;
	this->w = w;
	this->W = W;
	this->h = h;
	this->H = H;
	this->absolute_coordinates = absolute_coordinates;
}

void Layout::SetSizeRange( float min_w, float min_W, float min_h, float min_H, float max_w, float max_W, float max_h, float max_H, SizeFunction w_func, SizeFunction h_func ) {
	w_min[0]=min_w;
	w_max[0]=max_w;
	w_min[1]=min_W;
	w_max[1]=max_W;
	
	h_min[0]=min_h;
	h_max[0]=max_h;
	h_min[1]=min_H;
	h_max[1]=max_H;
	this->w_func = w_func;
	this->h_func = h_func;
}

void Layout::SetSize( float w, float W, float h, float H ) {
	w_min[0]=w_max[0]=w;
	w_min[1]=w_max[1]=W;
	h_min[0]=h_max[0]=h;
	h_min[1]=h_max[1]=H;
	w_func = SizeFunction::none;
	h_func = SizeFunction::none;
}

Layout::Layout(std::string s) {
	*this = parseRect(s);
}

Layout Layout::parseRect(std::string s) {
	Layout a;
	std::vector<std::string> parts;
	split_string(s, parts, ',');
	const char* names[] = { "wW", "hH", "W", "H" };
	float res[4];
	int p = 0;
	if(s[0] == 'a') {
		// std::cout << "ABS\n";
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
			// std::cout << "parsing rect(" << p << "):" << cur << " => " << res[0] << ", " << res[1] << ", " << res[2] << std::endl;
			size_t pos;
			if(p == 0) {
				if((pos=cur.find_first_of("LRM")) != std::string::npos) {
					if(cur[pos] == 'L') {
						a.absolute_coordinates = true;
					} else if(cur[pos] == 'R') {
						a.W += 1;
						a.w += -1;
						a.absolute_coordinates = true;
					} else if(cur[pos] == 'M') {
						a.W += 0.5;
						a.w += -0.5;
						a.absolute_coordinates = true;
					}
				}
				a.x += res[0];
				a.w += res[1];
				a.W += res[2];
			} else if(p == 1) {
				if((pos=cur.find_first_of("UDBM")) != std::string::npos) {
					if(cur[pos] == 'U') {
						a.absolute_coordinates = true;
					} else if(cur[pos] == 'M') {
						a.H += 0.5;
						a.h += -0.5;
					} else if(cur.find_first_of("DB",pos) != std::string::npos) {
						a.H += 1;
						a.h += -1;
						a.absolute_coordinates = true;
					}
				}
				a.y += res[0];
				a.h += res[1];
				a.H += res[2];
			}
		} else if(p >= 2) {
			size_t pos = 0;
			
			auto set_func = [&](Layout::SizeFunction f){
				// std::cout << "setting " << (const char*[]){"none","keep","fit","expand"}[f] << "\n";
				if(p == 2) {
					a.w_func = f;
				} else if(p == 3) {
					a.h_func = f;
				}
			};
			
			if(parts[i].find("keep", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::keep);
			} else if(parts[i].find("fit", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::fit);
			} else if(parts[i].find("expand", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::expand);
			}
			
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
				// std::cout << "parsing rect(" << p << "): " << cur << " => " << res[0] << ", " << res[1] << std::endl;
				if(p == 2) {
					a.w_min[0] += res[0];
					a.w_min[1] += res[1];
					a.w_max[0] = a.w_min[0];
					a.w_max[1] = a.w_min[1];
					a.w_func = Layout::SizeFunction::none;
				} else if(p == 3) {
					a.h_min[0] += res[0];
					a.h_min[1] += res[1];
					a.h_max[0] = a.h_min[0];
					a.h_max[1] = a.h_min[1];
					a.h_func = Layout::SizeFunction::none;
				}
			}
			
		}
		p++;
	}
	return a;
}

Rect Control::GetParentWidgetRegion() {
	if(widget) {
		return widget->GetRect();
	} else {
		int w,h;
		Drawing().GetResolution(w,h);
		return Rect(0,0,w,h);
	}
}

void Control::SetImage(std::string image, bool repeat) {
	Image* img = Images::LoadImage(image);
	if(!img) {
		return;
	}
	Size s = img->GetImageSize();
	m_style.image_tex = img;
	m_style.image_size.w = s.w;
	m_style.image_size.h = s.h;
	m_style.image_repeat = repeat;
}


void Control::SetStyle(std::string style, std::string value) {
	const Rect& r = GetRect();
	STYLE_SWITCH {
		_case("rect"): {
			Layout a = Layout::parseRect(value);
			SetLayout(a);
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
		_case("color"):
			m_style.color = Color(value).GetUint32();
		_case("bordercolor"):
			m_style.border_color = Color(value).GetUint32();
		_case("backgroundcolor"):
			m_style.background_color = Color(value).GetUint32();
		_case("hoverbordercolor"):
			if(value == "nochange" || value == "bordercolor") {
				m_style.hoverborder_color = m_style.border_color;
			} else {
				m_style.hoverborder_color = Color(value).GetUint32();
			}
		_case("hovercolor"):
			m_style.hovercolor = Color(value).GetUint32();
		_case("hoverbackground_color"):
			m_style.hoverbackground_color = Color(value).GetUint32();
		_case("border"):
		_case("noborder"):
			if(value == "true") {
				m_style.border_color = 0;
				m_style.hoverborder_color = 0;
			}
		_case("font"): {
			Font* f = Fonts::GetParsedFont(value);
			if(f) {
				std::cout << GetId() << " loading font: " << value << "\n";
				m_style.font = f;
				onFontChange();
			} else {
				std::cout << GetId() << " FAIL loading font: " << value << "\n";
			}
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

Layout& Layout::operator+=(const Layout& b) {
	W += b.W;
	w += b.w;
	x += b.x;
	
	H += b.H;
	h += b.h;
	y += b.y;
	
	absolute_coordinates |= b.absolute_coordinates;
	
	return *this;
}

std::ostream& operator<< (std::ostream& stream, const Layout& a) {
	return stream << a.W << ", " << a.w << ", " << a.x << " ; " <<
		a.H << ", " << a.h << ", " << a.y << " ; " <<
		a.w_min[1] << "+" << a.w_min[0] << ", " << a.h_min[1] << "+" << a.h_min[0] << " ; (" << 
		a.coord.x << ", " << a.coord.y << " a: " << a.absolute_coordinates << std::endl;
}


// ---- backend accessor helpers ----
Screen& Control::Drawing() {
	if(engine) {
		return *engine->backend.screen;
	} else {
		return *default_backend.screen;
	}
}

Speaker& Control::Sound() {
	if(engine) {
		return *engine->backend.speaker;
	} else {
		return *default_backend.speaker;
	}
}

System& Control::GetSystem() {
	if(engine) {
		return *engine->backend.system;
	} else {
		return *default_backend.system;
	}
}

}


