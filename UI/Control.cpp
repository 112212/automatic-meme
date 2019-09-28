#include "RapidXML/rapidxml.hpp"
#include "controls/RadioButton.hpp"
#include <iostream>
#include <algorithm>
#include "Control.hpp"
#include "Gui.hpp"
#include "Effect.hpp"
#include "Border.hpp"
#include "managers/Images.hpp"

#define BIGNUM 9999

namespace ng {
Control::Control() 
	: ControlManager(this)
	, id( "____" )
	, engine(0)
	, parent(0)
	, z_index(0)
	, sel_control(0)
	, type("control")
	, interactible(true)
	, visible(true)
	, use_clipping(true)
	, intercept_mask(0)
	, m_is_intercepted(false)
	, render_enabled(true)
	, nostyling(false)
	, m_rect(0,0,50,50)
	, m_border(0)
	, m_offset(0,0)
	, min(0,0)
	, max(BIGNUM,BIGNUM)
{ 
	static int control_cnt = 0;
	// ------- [ControlBase] ------
	id = "unnamed"+std::to_string((control_cnt++));
	m_style.border_color = 0xff333376;
	m_style.hoverborder_color = 0xff1228D1;
	// m_style.border_color = 0;
	// m_style.hoverborder_color = 0;
	m_style.background_color = 0;
	m_style.hoverbackground_color = 0;
	m_style.font = Fonts::GetFont( "default", 13 );
	m_style.image_tex = 0;
	m_style.alpha = 1.0f;
	m_style.color = 0;
	m_style.draggable = false;
	m_style.hoverimg = 0;
	// std::cout << "creating control: " << type << ", "  << this << "\n";
	layout = Layout("0,0");
	// ------ [/ControlBase] ------
	
	// ----- widget part
	layout.SetSizeRange( 0, 0, 0, 0, BIGNUM, 0, BIGNUM, 0 );
}

void Control::DisableStyling(bool tf) {
	nostyling = tf;
}

Control::~Control() {
	
}

void Control::OnClick(int x, int y, MouseButton btn) {
	
}

void Control::set_engine(Gui* engine) {
	this->engine = engine;
	for(Control* c : controls) {
		c->set_engine(engine);
	}
}

void Control::intercept() {
	if(engine) {
		engine->hasIntercepted = true;
	}
}

void Control::setInterceptMask(unsigned int mask) {
	intercept_mask = mask;
}

void Control::AddControl( Control* control, bool processlayout ) {
	if(control->parent or control->engine) {
		return;
	}
	
	control->parent = this;
	
	if(engine) {
		static_cast<Control*>(control)->set_engine(engine);
	}
	
	addControlToCache(control);
	
	// std::cout << "processing layout\n";
	if(engine && processlayout) {
		ProcessLayout();
	}
	// std::cout << "processed layout\n";
}

bool Control::isSelected() {
	return engine && engine->GetSelectedControl() == this;
}

bool Control::inSelectedBranch() {
	return sel_control != 0 or isSelected();
}

void Control::LockWidget(bool lock) {
	if(!engine) {
		// TODO: use debug system
		std::cout << "\n[GUI] widget cannot be locked if not bound to an engine\n";
		return;
	}
	if(lock) {
		engine->LockWidget(this);
	} else {
		sendGuiCommand( GUI_WIDGET_UNLOCK );
	}
}

void Control::RemoveControl( Control* control ) {
	removeControlFromCache(control);
	control->parent = 0;
	control->engine = 0;
}

void Control::SetOffset(int x, int y) {
	Point p(x,y);
	if(inSelectedBranch() && !isSelected()) {
		engine->sel_control_parent_window_position += p-m_offset;
	}
	m_offset = p;
}

Control* Control::Clone() {
	Control *w = new Control();
	copyStyle(w);
	return w;
}

void Control::Render( Point position, bool isSelected ) {
	Control::RenderBase(position, isSelected);
	RenderWidget(position,isSelected);
}

void Control::RenderWidget( Point position, bool isSelected ) {
	if(cache.empty()) return;
	use_clipping = true;
	
	Rect old_box;
	bool save_clipping;
	
	Point orig_position = position;
	position = position.Offset(m_rect);
	// ------ clipping ---------
	// must go between these 2 "position.Offset"
	if(use_clipping) {
		save_clipping = Drawing().GetClipRegion(old_box.x, old_box.y, old_box.w, old_box.h);
		int margin = 2;
		old_box += Rect(-margin, -margin, margin, margin);
		ng::Rect clipRect = ng::Rect( std::max(0,position.x-margin), std::max(0, position.y-margin), m_rect.w+margin*2, m_rect.h+margin*2 );
		if(save_clipping) {
			clipRect = getIntersectingRectangle(old_box, clipRect);
		}
		Drawing().PushClipRegion( clipRect.x, clipRect.y, clipRect.w, clipRect.h );
	}
	// ---------------------------
	position = position.Offset(m_offset);
	
	for(auto &ca : cache) {
		if(ca.visible) {
			Control* const &c = ca.control;
			// std::cout << "inner control render: " << c->GetId() << "\n";
			#ifdef SELECTED_CONTROL_ON_TOP
				if(c != sel_control) {
					c->render( position, false );
				}
			#else
				c->render( position, isSelected && c == sel_control );
			#endif
		}
	}
	
	#ifdef SELECTED_CONTROL_ON_TOP
	if(sel_control && sel_control->visible) {
		sel_control->render(position,true);
	}
	#endif
	
	if(use_clipping) {
		Drawing().PopClipRegion();
		save_clipping = Drawing().GetClipRegion(old_box.x, old_box.y, old_box.w, old_box.h);
	}
	
}

void Control::setCursor(CursorType type) {
	if(engine) {
		engine->GetCursor().SetCursorType(type);
	}
}

Control* Control::get(const std::string& id) {
	for(auto c : controls) {
		if(c->id == id) {
			return c;
		}
		
		Control* p = c->get(id);
		if(p) {
			return p;
		}
	}
	return 0;
}

std::string Control::GetSelectedRadioButton(int group) {
	for(auto &c : controls) {
		RadioButton* rb = dynamic_cast<RadioButton*>(c);
		if( rb ) {
			if(group == rb->GetGroup() && rb->IsSelected()) {
				return rb->GetId();
			}
		}
	}
	return "";
}



std::string getTabs(int depth) {
	depth--;
	if(depth <= 0) return "";
	std::string s(depth*2, '\t');
	for(int i=0; i < s.size(); i+=2) {
		s[i] = '|';
	}
	return s;
}


void Control::SetTransparentBackground() {
	m_style.SetTransparentBackground();
}


#define dbg(x)

dbg(static int depth = 0;)

// -------------------------------------------
// ------- Processing layout -----------------
// -------------------------------------------
Point Control::layoutProcessControls(std::vector<Control*>& controls_copy) {
	
	dbg(std::cout << getTabs(depth) << "layoutProcessControls " << "\n";)
	bool w_auto = false;
	bool h_auto = false;
	bool any_auto = false;
	
	int lasty = 0;
	int min_x = 0, min_y = 0;
	int max_x = min_x, max_y = min_y;
	const Layout &pa = GetLayout();
	// max_x = a.padding.x;
	// max_y = a.padding.y;
	if(pa.w_func != Layout::SizeFunction::none) {
		dbg(std::cout << getTabs(depth) << "w_func is: " << pa.w_func << "\n";)
		w_auto = true;
	}
	if(pa.h_func != Layout::SizeFunction::none) {
		dbg(std::cout << getTabs(depth) << "h_func is: " << pa.h_func << "\n";)
		h_auto = true;
	}
	any_auto = w_auto || h_auto;
	
	const Rect pr = GetRect();
	// corner points for auto
	Point c2 = Point(0,0);
	for(Control* c : controls_copy) {
		// if(!c->IsVisible()) continue;
		const Layout &a = c->GetLayout();
		if(!a.enabled) continue;
		const Rect &r = c->GetRect();
		const Point p = a.coord;
		
		// dbg(std::cout << "processing control: " << c->GetId() << "\n");
		
		// calculate min, max of this control if its widget
		
		Size _min = Size(pr.w, pr.h);
		Size _max = max;
		if(a.w_func == Layout::SizeFunction::none) {
			_min.w = pr.w;
			_max.w = _min.w;
		}
		
		if(a.h_func == Layout::SizeFunction::none) {
			_min.h = pr.h;
			_max.h = _min.h;
		}
		
		
		c->min.w = a.w_min[0] + a.w_min[1] * _min.w;
		c->max.w = a.w_max[0] + a.w_max[1] * _max.w;
		
		c->min.h = a.h_min[0] + a.h_min[1] * _min.h;
		c->max.h = a.h_max[0] + a.h_max[1] * _max.h;
		
		// c->min.w = a.w_min[0] + a.w_min[1] * pr.w;
		// c->max.w = a.w_max[0] + a.w_max[1] * pr.w;
		
		// c->min.h = a.h_min[0] + a.h_min[1] * pr.h;
		// c->max.h = a.h_max[0] + a.h_max[1] * pr.h;
		
		dbg(std::cout << getTabs(depth) << "inside widget [" << c->GetId()
				  << "] " << c->min.w 
				  << ", " << c->max.w 
				  << ", " << c->min.h 
				  << ", " << c->max.h << " ABS " << a.absolute_coordinate_x << ", " << a.absolute_coordinate_y << "\n");
				  
		dbg(std::cout << getTabs(depth) << "+-> Entering widget [" << c->GetId() << "]\n";)
		c->ProcessLayout();
		// control forced on next line or overflow
		if( !(a.absolute_coordinate_x && a.absolute_coordinate_y) && (p.y != lasty || (max_x + a.x + r.w > max.w)) ) {
			lasty = p.y;
			
			// starts at x beginning (min_x)
			max_x = min_x;
			
			// at new row
			min_y = max_y;
			dbg(std::cout << "next line or overflow\n");
		}
		
		// std::cout << c->GetId() << ": " << r << "\n";
		
		// x + w * width + W * parent_width
		int xc = a.x + (a.w * r.w) + (a.W * pr.w);
		
		// y + h * height + H * parent_height
		int yc = a.y + (a.h * r.h) + (a.H * pr.h);
		
		int x,y;
		
		
		if(a.absolute_coordinate_x && a.absolute_coordinate_y) {
			c->SetPosition(xc+a.padding.x, yc+a.padding.y);
		} else {
			if(a.absolute_coordinate_x) {
				x = xc;
				max_x = 0;
				min_y = max_y;
				lasty = p.y;
				// max_y = std::max<int>(min_y + yc + r.h, max_y);
			} else {
				x = xc + max_x;
				dbg(std::cout << "x = " << x << "\n");
				max_x += xc + r.w;
			}
			
			if(a.absolute_coordinate_y) {
				y = yc;
				// max_x = 0;
				// min_y = max_y;
				// lasty = p.y;
			} else {
				y = yc + min_y;
				max_y = std::max<int>(min_y + yc + r.h, max_y);
			}
			
			c->SetPosition(x+a.padding.x, y+a.padding.y);
		}
		
		if(any_auto) {
			// auto max
			if(w_auto) {
				c2.x = std::max<int>(c2.x, r.x + r.w - a.padding.x);
			}
			
			if(h_auto) {
				c2.y = std::max<int>(c2.y, r.y + r.h - a.padding.y);
			}
		}
		
		c->SetSize(r.w - a.padding.x - a.padding.w, r.h - a.padding.y - a.padding.h);
	}
	return c2;
}


void Control::ProcessLayout(bool asRoot) {
	bool w_auto = false;
	bool h_auto = false;
	int wres, hres;
	int wmax = 0;
	
	dbg(depth++;)

	const Rect &r = GetRect();
	const Layout &a = GetLayout();
	
	dbg(std::cout << getTabs(depth) << "ProcessLayout (Control [" << GetId() << "])\n";)
	
	// resolve wres
	if(a.w_func != Layout::SizeFunction::none) {
		dbg(std::cout << getTabs(depth) << "w_func is: " << a.w_func << "\n";)
		w_auto = true;
	}
	
	// if(asRoot) {
		// wres = r.w;
		// wmax = r.w;
	// } else 
	{
		wres = clip(r.w, min.w, max.w);
		wmax = max.w;
	}
	
	// this_widget->m_rect.w = wres;
	dbg(std::cout << getTabs(depth) << "setting w: " << wres << "\n";)
	
	// resolve hres
	if(a.h_func != Layout::SizeFunction::none) {
		dbg(std::cout << getTabs(depth) << "h_func is: " << a.h_func << "\n";)
		h_auto = true;
	}
	
	// if(asRoot) {
		// hres = r.h;
	// } else 
	{	
		hres = clip(r.h, min.h, max.h);
	}
	// std::cout << GetId() << " " << r << " min: " << min << ", " << max << "\n";
	
	dbg(std::cout << getTabs(depth) << "setting h: " << hres << "\n";)
	
	// SetRect(r.x, r.y, wres, hres);
	SetSize(wres, hres);
	
	dbg(std::cout << getTabs(depth) << "size is set\n";)
	bool any_auto = w_auto || h_auto;
	Point c2;
	std::vector<Control*> controls_copy;
	if(!controls.empty()) {
		controls_copy = controls;
		// sort controls by coords
		std::sort(controls_copy.begin(), controls_copy.end(), [](Control* ca, Control* cb) {
			const Point &p1 = ca->GetLayout().coord;
			const Point &p2 = cb->GetLayout().coord;
			return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
		});
		
		c2 = layoutProcessControls(controls_copy);
	}
	
	// if widget with auto sizes, must calculate it
	if(!asRoot && any_auto) {
		dbg(std::cout << getTabs(depth) << "has any_auto\n";)
		const Rect &r = GetRect();
		const Layout &a = GetLayout();
		Control* p = parent;
		const Rect& pr = p->GetRect();
		
		int w = r.w;
		int h = r.h;
		int x = r.x;
		int y = r.y;
		
		
		Rect cr = GetContentRect();
		if(w_auto) {
			if(a.w_func == Layout::SizeFunction::fit) {
				w = clip(c2.x, std::max(min.w, cr.w), max.w);
			} else if(a.w_func == Layout::SizeFunction::expand) {
				w = std::max<int>(w, c2.x);
			}
			x += a.w*w;
			if(p) {
				x += a.W*pr.w;
			}
		}
		
		if(h_auto) {
			if(a.h_func == Layout::SizeFunction::fit) {
				h = clip(c2.y, std::max(min.h, cr.h), max.h);
			} else if(a.h_func == Layout::SizeFunction::expand) {
				h = std::max<int>(h, c2.y);
			}
			y += a.h*h;
			if(p) {
				x += a.H*pr.h;
			}
		}
		
		SetRect(x, y, w, h);
		
	}
		// layoutProcessControls(controls_copy);
	
	dbg(
		std::cout << getTabs(depth-1) << "<----- Leaving widget [" << GetId() << "]\n";
	)
	dbg(depth--;)
}


void Control::parseXml(rapidxml::xml_node<>* node) {
	Layout layout;
	for(;node;node=node->next_sibling()) {
		// std::cout << "\tControl::parseXml (" << GetId() << ") :" << node->name() << "\n";
		parseAndAddControl(node, layout);
	}
	// std::cout << "\tleaving Control::parseXml (" << GetId() << ")\n";
}


// ----------------------- [ControlBase] ---------------------



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


Rect Control::GetContentRect() {
	return Rect(0,0,0,0);
}

// get control's window position
const Point Control::GetGlobalPosition() {
	return GetOffset().Offset(m_rect);
}

void Control::SetDraggable( bool draggable ) {
	m_style.draggable = draggable;
}

void Control::SetZIndex( int zindex ) {
	if(zindex == z_index) return;
	if(parent) {
		parent->setZIndex((Control*)this, zindex);
	}
}

void Control::SendToFront() {
	if(parent) {
		parent->sendToFront((Control*)this);
	}
}
void Control::SendToBack() {
	if(parent) {
		parent->sendToBack((Control*)this);
	}
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
	if(parent) {
		parent->updateCache((Control*)this,flag);
	}
}

bool Control::isActive() {
	return engine && engine->active_control == this;
}

void Control::SetVisible(bool visible) {
	if(this->visible != visible) {
		this->interactible = visible;
		this->visible = visible;
		if(!visible && engine && engine->active_control == this) {
			// engine->active_control = 0;
			engine->Activate(0);
		}
		update_cache(CacheUpdateFlag::attributes);
		if(!visible && ((Control*)this)->inSelectedBranch() ) {
			// TODO: review
			
		}
		if(visible) {
			emitEvent("show");
		} else {
			emitEvent("hide");
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

bool Control::poseEmitEvent( Control* c, std::string event_id, const Argv& args ) {
	return c->emitEvent(event_id, args);
}

// emit global event
bool Control::emitEvent( std::string event_id, const Argv& argv ) {
	#ifdef USE_EVENT_QUEUE
	if(engine) {
		engine->m_events.push( { event_id, this, argv } );
	}
	#endif
	
	bool emitted = false;
	// std::cout << "evt " << event_id << "\n";
	for(auto sub : subscribers) {
		auto sub_func = Gui::function_map.find(sub.function_name);
		if(sub_func == Gui::function_map.end()) continue;
		
		if(sub.event_id == event_id) {
			Args args;
			args.event_args = argv;
			args.control = (Control*)this;
			args.cmd_args = sub.args;
			emitted=true;
			sub_func->second(args);
			// std::cout << "emitting " << event_id << " " << args.event_args.size() << "\n";
		}
	}
	return emitted;
}

double Control::getDeltaTime() {
	if(engine) {
		return engine->getDeltaTime();
	}
	return 0;
}

void Control::Focus() {
	if(engine) {
		engine->Focus((Control*)this);
	}
}


void Control::Activate() {
	if(engine) {
		engine->Activate((Control*)this);
	}
}

void Control::OnActivate() {
	
}

static int last_anon_id = 0;
void Control::onEvent( std::string event_type, EventCallback callback ) {
	std::string anon_id = std::string("_anon") + std::to_string(last_anon_id++);
	subscribers.emplace_back(event_type, anon_id);
	Gui::function_map[ anon_id ] = callback;
}

void Control::sendGuiCommand( int eventId, Control* target ) {
	if(engine) {
		if(!target) target=this;
		engine->processControlEvent(eventId, target);
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
	if(parent) {
		return parent->controls;
	} else {
		return empty_vector;
	}
}

void Control::SetPosition( Point pt ) {
	SetRect(pt.x, pt.y, m_rect.w, m_rect.h); 
}

void Control::SetPosition( int x, int y ) { 
	SetRect(x, y, m_rect.w, m_rect.h); 
}

void Control::SetSize( Size size ) {
	SetRect(m_rect.x, m_rect.y, size.w, size.h);
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

std::string Control::GetFullId() {
	std::stack<std::string> ids;
	Control* d = this;
	while(d->parent) {
		ids.push(d->GetId());
		d = d->parent;
	}
	
	std::string fid = "";
	bool first=true;
	while(!ids.empty()) {
		if(first) {
			first=false;
		} else {
			fid += "/";
		}
		fid += ids.top();
		ids.pop();
	}
	return fid;
}

int Control::GetZIndex() {
	return z_index;
}

std::string Control::GetTooltip() {
	return m_style.tooltip;
}

bool Control::CheckCollisionA(const Point& p) {
	return CheckCollision(p - GetParentRect());
}
bool Control::CheckCollision(const Point& p) {
	Rect& r = m_rect;
	if(p.x >= r.x && p.x <= r.x+r.w && p.y >= r.y && p.y <= r.y+r.h) {
		if(m_border) {
			return m_border->CheckCollision(p);
		} else {
			return true;
		}
	} else {
		return false;
	}
}

void Control::SetRect( int x, int y, int w, int h ) {
	w = std::max(0,w);
	h = std::max(0,h);
	
	Rect newRect = Rect(x,y,w,h);
	if(m_rect == newRect) {
		return;
	}
	
	if(inSelectedBranch()) {
		Point dp = newRect - m_rect;
		
		if(!isSelected()) {
			engine->sel_control_parent_window_position += dp;
		}
		
		Control* c = this;
		while(c) {
			c->cached_absolute_offset = c->cached_absolute_offset + dp;
			c = c->sel_control;
		}
		m_rect=newRect;
	} else {
		m_rect=newRect;
		cached_absolute_offset = getAbsoluteOffset();
		// std::cout << "abs " << GetId() << " " <<  cached_absolute_offset << "\n";
	}
	
	if(parent) {
		parent->updateCache((Control*)this, CacheUpdateFlag::position);
	}
	
	onRectChange();
}

void Control::SetTooltip(std::string tooltip) {
	this->m_style.tooltip = tooltip;
}

void Control::SetRect( Rect r ) { 
	SetRect(r.x, r.y, r.w, r.h);
}

void Control::SetLayout( float x, float w, float W, float y, float h, float H ) {
	this->layout = Layout( layout.coord, x, y, W, w, H, h );
}

void Control::SetLayout( std::string str_layout ) {
	this->layout = Layout(str_layout);
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

void Control::SetLayoutEnabled(bool enabled) {
	layout.SetEnabled(enabled);
}

void Control::AddEffect(Effect* effect) {
	if(engine) {
		effect->control = this;
		effect->gui = engine;
		if(effect->is_exclusive) {
			RemoveEffect(effect->name);
		}
		effect->Init();
		effects.push_back(effect);
	}
}

void Control::RemoveEffect(Effect* effect) {
	auto it = std::remove(effects.begin(), effects.end(), effect);
	effects.resize(std::distance(effects.begin(), it));
}

void Control::applyStyling(std::vector<ControlCreationPair>& vec, std::string group) {
	if(vec.empty()) return;
	auto &gstyles = ControlManager::group_styles;
	// printCreationVector();
	for(auto &g : gstyles) {
		if(g.disabled) continue;
		if(group.empty()) {
			current_tag = g.tag_id;
			applyStyling(g.styles, vec.begin(), vec.end());
		} else if(g.name == group) {
			current_tag = g.tag_id;
			applyStyling(g.styles, vec.begin(), vec.end());
			break;
		}
	}
	current_tag = 0;
}
bool Control::applyStyling(std::vector<Styling>& stylings, std::vector<ControlCreationPair>::iterator it, std::vector<ControlCreationPair>::iterator vec_end) {
	bool is_leaf = it+1 == vec_end;
	
	// if(is_leaf) {
		// std::cout << "is_leaf: " << "\n";
	// }
	bool match = false;
	for(auto &s : stylings) {
		// std::cout << "Testing styling " << s.style_for << " with " << it->type << " , " << it->id << "\n";
		if(s.style_for == "**") {
			// apply no matter what
			match = true;
			applyStyling(s);
			// if(!is_leaf) {
				for(auto it2 = it; it2 != vec_end; it2++) {
					if(applyStyling(s.child_styles, it2, vec_end)) {
						break;
					}
				}
			// }
		} else if(s.style_for == "*") {
			// apply if leaf
			match = true;
			if(is_leaf) {
				// apply
				applyStyling(s);
			} else {
				applyStyling(s.child_styles, it+1, vec_end);
			}
		} else if((s.style_for[0] == '#' && s.style_for.substr(1) == it->id) || (s.style_for == it->type)) {
			match = true;
			if(is_leaf) {
				applyStyling(s);
			} else {
				applyStyling(s.child_styles, it+1, vec_end);
			}
		}
	}
	return false;
}

int Control::current_tag = 0;

void Control::applyStyling(const Styling& styling) {
	// std::cout << "applying style: " << styling.style_for << " to " << GetId() << "\n";
	for(auto &attr : styling.attributes) {
		SetStyle(attr.first, attr.second);
	}
}

void Control::backtrackStylingCreationPairs() {
	if(!parent) return;
	// std::cout << "backtrack creation: \n";
	std::stack<ControlCreationPair> bt;
	Control* p = this;
	while(p->parent) {
		bt.push({p->GetId(), p->GetType()});
		p=p->parent;
	}
	while(!bt.empty()) {
		creation_vector.push_back(bt.top());
		bt.pop();
	}
	
	// printCreationVector();
}

void Control::ApplyStyle(std::string style_group) {
	if(nostyling) return;
	backtrackStylingCreationPairs();
	applyStyling(creation_vector, style_group);
	creation_vector.clear();
}

void Control::ApplyStyle(const Styling& s) {
	if(nostyling) return;
	applyStyling(s);
}

Control* Control::createControl(std::string type, std::string id) {
	Control* c;
	if(parent) {
		// backtrack to creation_vector
		backtrackStylingCreationPairs();
	}
	c = ControlManager::CreateControl(type, id);
	if(parent) {
		creation_vector.clear();
	}
	return c;
}

void Control::ForEachControl(std::function<void(Control*)> func) {
	for(Control* c : controls) {
		func(c);
		c->ForEachControl(func);
	}
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
	
	// draw border
	Point pos = m_rect.Offset(position);
	#ifdef SELECTION_MARK
		if(isSelected) {
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
	
	process_postrender_effects();
}

// overridable
void Control::RenderBase( Point pos, bool isSelected ) {
	pos = m_rect.Offset(pos);
	
	if(isSelected && m_style.hoverbackground_color != 0) {
		Drawing().FillRect( pos.x, pos.y, m_rect.w, m_rect.h, m_style.hoverbackground_color );
	} else
	if(m_style.background_color != 0) {
		Drawing().FillRect( pos.x, pos.y, m_rect.w, m_rect.h, m_style.background_color );
	}
	
	if(isSelected && m_style.hoverimg) {
		Size s = m_style.hoverimg->GetImageSize();
		Drawing().TexRect(pos.x, pos.y, m_rect.w, m_rect.h, m_style.hoverimg, false, s.w, s.h);
	} else
	if(m_style.image_tex) {
		Drawing().TexRect(pos.x, pos.y, m_rect.w, m_rect.h, m_style.image_tex, m_style.image_repeat, 
			m_style.image_size.w, m_style.image_size.h);
	}
}


void Control::SetFont( std::string name ) { 
	Font* f = Fonts::GetFont(name, 0); 
	m_style.font = f;
}

Point Control::getAbsCursor() {
	return getEngine()->GetCursor().GetCursor();
}

Point Control::getCursor() {
	// control local coords
	return getAbsCursor() - getAbsoluteOffset();
}

const Point Control::getAbsoluteOffset() { 
	if(inSelectedBranch()) {
		return cached_absolute_offset;
	} else {
		Point pt;
		Control* pc = this;
		while(pc->parent) {
			pt += pc->m_rect;
			pc = pc->parent;
		}
		return pt;
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
	}
}

void Control::Unattach() {
	if(parent) {
		parent->RemoveControl(this);
		parent = 0;
	}
	engine = 0;
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
	return engine->dragging && engine->sel_control == this;
}

void Control::cloneBase(Control* clone_to) {
	clone_to->subscribers.insert(clone_to->subscribers.end(), this->subscribers.begin(), this->subscribers.end());
}

void Control::CopyStyle(Control* copy_to) {
	copyStyle((Control*)copy_to);
}

void Control::copyStyle(Control* copy_to) {
	copy_to->m_style = this->m_style;
	cloneBase(copy_to);
}

void Control::SetAlpha(float alpha) {
	m_style.alpha = alpha;
}

void Control::ControlStyle::SetTransparentBackground() {
	background_color = 0;
	hoverimg = 0;
	hovercolor = 0;
	image_tex = 0;
	hoverbackground_color = 0;
}

void Control::ControlStyle::SetTransparentBorder() {
	hoverborder_color = 0;
	border_color = 0;
}

void Control::SetBorder(Border* border) {
	m_border = border;
	m_border->m_rect = &m_rect;
	// m_border->control = this;
}
Border* Control::GetBorder() {
	return m_border;
}

Rect Control::GetParentRect() {
	if(parent) {
		return parent->GetRect();
	} else {
		return {0,0,0,0};
	}
}

void Control::SetImage(std::string image, bool repeat) {
	Image* img = Images::LoadImage(image);
	if(!img) {
		m_style.image_tex = 0;
		return;
	}
	Size s = img->GetImageSize();
	m_style.image_tex = img;
	m_style.image_size = s;
	m_style.image_repeat = repeat;
}

void Control::SetImage(Image* image, bool repeat) {
	Image* img = image;
	if(!img) {
		m_style.image_tex = 0;
		return;
	}
	Size s = img->GetImageSize();
	m_style.image_tex = img;
	m_style.image_size = s;
	m_style.image_repeat = repeat;
}



void Control::SetStyle(std::string style, const char* value) {
	SetStyle(style, std::string(value));
}

void Control::SetStyle(std::string style, bool value) {
	SetStyle(style, std::string(value ? "t" : "f"));
}

void Control::SetStyle(std::string style, std::string value) {
	const Rect& r = GetRect();
	STYLE_SWITCH {
		_case("rect"): {
			Rect padding = layout.padding;
			Layout a = Layout(value);
			a.SetPadding(padding);
			SetLayout(a);
			// SetRect(a.x, a.y, a.w_min[0], a.h_min[0]);
		}
		_case("padding"):
			layout.SetPadding(value);
		_case("zindex"):
			SetZIndex(std::stoi(value));
		_case("id"):
			SetId(value);
		_case("interactible"):
			setInteractible(toBool(value));
		_case("visible"):
			SetVisible(toBool(value));
		_case("color"):
			m_style.color = Color(value).GetUint32();
		_case("bordercolor"):
			m_style.border_color = Color(value).GetUint32();
		_case("bgcolor"):
			m_style.background_color = Color(value).GetUint32();
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
		_case("hoverbg_color"):
			m_style.hoverbackground_color = Color(value).GetUint32();
		_case("border"):
		_case("noborder"):
			if(toBool(value)) {
				m_style.border_color = 0;
				m_style.hoverborder_color = 0;
			}
		_case("font"): {
			Font* f = Fonts::GetParsedFont(value);
			if(f) {
				// std::cout << GetId() << " loading font: " << value << "\n";
				m_style.font = f;
				onFontChange();
			} else {
				std::cout << GetId() << " FAIL loading font: " << value << "\n";
				exit(0);
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
		_case("hoverimage"): {
			m_style.hoverimg = Images::LoadImage(value);
		}
		_case("draggable"): {
			m_style.draggable = toBool(value);
		}
			break;
			
		default:
		
			// events
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
					subscribers.back().tag = current_tag;
				}
			} else {
			
				OnSetStyle(style, value);
			}
	}

}

void Control::OnSetStyle(std::string& style, std::string& value) {}

void Control::getRange(Size& min, Size& max) {
	min = this->min;
	max = this->max;
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


// -------------[/ControlBase]---------------

}
