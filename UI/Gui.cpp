

#include <stack>
#include <cassert>
#include <iostream>
#include <ratio>
#include <chrono>

#include "Gui.hpp"
#include "Control.hpp"
#include "controls/Label.hpp" // only for tooltip
#include "managers/Sounds.hpp"
#include "managers/ResourceManager.hpp"
#include "TiledFont.hpp"

namespace ng {

Backend default_backend(new Screen(), new Speaker(), new System());

void Gui::cmd_play_sound( Args& args ) {
	if(args.cmd_args.empty()) {
		return;
	}
	Control* c = args.control;
	// TODO: cleanup
	// static std::chrono::high_resolution_clock::time_point tp_c = std::chrono::high_resolution_clock::now();
	// if(std::chrono::high_resolution_clock::now() - tp_c < std::chrono::milliseconds(200)) {
		// tp_c = std::chrono::high_resolution_clock::now();
		// return;
	// }
	// tp_c = std::chrono::high_resolution_clock::now();
	if(c->getEngine()) {
		// for(auto s : a)
		// std::cout << "[" << c->GetId() << "] play sound: " << args.cmd_args.back() << "\n";
		c->Sound().PlaySound( Sounds::GetSound( args.cmd_args.back() ) );
	}
}

void Gui::cmd_style( Args& args ) {
	if(args.cmd_args.size() == 3) {
		Control* c = rootWidget.Get(args.cmd_args[0]);
		if(c) {
			c->SetStyle(args.cmd_args[1], args.cmd_args[2]);
		}
	}
	if(args.control) {
		args.control->SetStyle(args.cmd_args[0], args.cmd_args[1]);
	}
}

Gui::Gui() {
	m_delta_accum = 0;
	m_mouse_down = false;
	m_focus = false;
	m_keyboard_lock = false;
	m_focus_lock = false;
	m_lock_once = false;
	m_block_all_events = false;
	
	m_time = 0;
	dragging = false;
	m_frames = 0;
	drag_offset = {0,0};
	depth = 0;
	sel_control = 0;
	active_control = 0;
	sel_parent = 0;
	m_lock_target = 0;
	
	hasIntercepted = false;
	sel_intercept = 0;
	sel_intercept_vector.resize(15);
	
	selection_margin = 1;
	
	m_tooltip_shown = false;
	m_tooltip_delay = 2;
	m_tooltip = 0;
	mutex = new std::mutex();
	
	rootWidget.set_engine(this);
	rootWidget.SetId("RootWidget");
	rootWidget.SetLayout(Layout("0,0,1W,1H"));
	
	SetBackend(default_backend);
	AddFunction( "playsound", cmd_play_sound );
	AddFunction( "style", std::bind( &Gui::cmd_style, this, std::placeholders::_1 ) );
	
	ResourceManager::RegisterResourceLoader("tiledfont", TiledFont::GetFont);
}

void Gui::HideOSCursor() {
	backend.system->SetCursorVisibility(false);
}

Gui::Gui(int xsize, int ysize) : Gui() {
	SetSize(xsize, ysize);
}

Gui& Gui::operator=(Gui && engine) {
	*this = engine;
	rootWidget.set_engine(this);
	AddFunction( "style", std::bind( &Gui::cmd_style, this, std::placeholders::_1 ) );
	return *this;
}

Gui::~Gui() {
	Clear();
}

void Gui::Clear() {
	rootWidget.RemoveControls();
}

void Gui::SetDefaultFont(std::string font, int size) {
	if(!Fonts::LoadFont(font, "default", size)) {
		std::cout << "NOT LOADED\n";
		exit(-2);
	}
}

void Gui::SetSize(int w, int h) {
	if(backend.screen) {
		backend.screen->Init();
		backend.screen->SetResolution(w, h);
		resolution = Size(w,h);
	}
	
	rootWidget.SetLayout(Layout(Point(0,0), 0,0,0,w,0,h));
	rootWidget.SetRect(0,0,w,h);
	rootWidget.min = Size(w,h);
	rootWidget.max = Size(w,h);
	ProcessLayout();
}

Size Gui::GetSize() {
	int x,y;
	backend.screen->GetResolution(x,y);
	resolution = Size(x,y);
	return resolution;
}

void Gui::LockWidget(Control* c) {
	if(!c or c->engine != this) return;
	
	Focus(c);
	m_focus_lock = true;
}

void Gui::UnlockWidget() {
	m_focus_lock = false;
}

void Gui::processControlEvent(int event_type, Control* target) {
	if(target) {
		m_lock_target = target;
		Focus(target);
	} else if(event_type == GUI_FOCUS_LOCK || event_type == GUI_LOCK_ONCE) {
		if(!sel_control) {
			return;
		}
		if(sel_control != m_lock_target) {
			m_lock_target = sel_control;
			Focus(m_lock_target);
		}
	}
	
	switch(event_type) {
		case GUI_KEYBOARD_LOCK:
			m_keyboard_lock = true;
			break;
			
		case GUI_FOCUS_LOCK:
			m_focus_lock = true;
			break;
			
		case GUI_UNLOCK:
			m_focus_lock = false;
			m_keyboard_lock = false;
			m_lock_once = false;
			m_lock_target = 0;
			break;
			
		case GUI_LOCK_ONCE:
			m_focus_lock = true;
			m_lock_once = true;
			break;
			
		case GUI_UNLOCK_BY_MOUSEUP:
			m_unblock_all_events_on_mouse_up = true;
			break;
		
		case GUI_UNSELECT:
			unselectControl();
			return;
			
		case GUI_UNSELECT_WIDGET:
			unselectControls();
			break;
	}
}

/*
	TODO: remove all events which are still in queue for this control which is being removed
		  if its widget, then remove all children events from event queue
*/
void Gui::RemoveControl( Control* control ) {
	if(!control->parent || control->engine != this) {
		return;
	}
	std::cout << "RemoveControl called\n";

	
	if(sel_control == control) {
		unselectControl();
	}
	
	std::string id = control->id;
	
	// if its widget, make sure nothing breaks
	Control* widget = control;
	
	// if any of selected controls or widgets are inside this widget, we must break selection
	if(sel_control) {
		Control* w = sel_control->parent;
		while(w && w != widget) {
			w = w->parent;
		}
		if(w == widget) {
			unselectControl();
		}
	}
	
	if(sel_parent) {
		Control* w = sel_parent;
		while(w && w != widget) {
			w = w->parent;
		}
	}
	// }
	
	control->parent->removeControlFromCache(control);
	control->engine = 0;
	control->parent = 0;
}

void Gui::SetStyle(std::string control, std::string style, std::string value) {
	Control* c = rootWidget.Get<Control>(control);
	if(c) {
		c->SetStyle(style, value);
	}
}

/*
void Gui::intercept_hook(Control::imask onaction, std::function<void()> action, std::function<void()> intercept_action, std::function<void()> no_intercept_control_action) {
	Control* last_sel_control = sel_control;
	if((sel_intercept & Control::imask::action_enum) != 0) {
		for(int i=0; !hasIntercepted; i++) {
			if(sel_intercept_vector.size() <= i) break;					
			interceptInfo &v = sel_intercept_vector[i];             	
			if(!v.parent_control || v.parent_control == sel_control) break;
			if( (v.intercept_mask & Control::imask::action_enum) != 0) {  
				v.parent_control->m_is_intercepted = true;
				v.parent_control->action;
				if(intercept_action) {
					intercept_action();
				}
				v.parent_control->m_is_intercepted = false;
				// std::cout << "intercepted: " << last_sel_control->GetId()< " by: " << v.parent_control->GetId() << "\n";
			}                                                       	
		}                                                           	
	} 																	
	if(hasIntercepted) {												
		hasIntercepted = false; 										
	} else if(last_sel_control) {										
		last_sel_control->action;										
	}}
}
*/

/* TODO: what if gets deleted in one of the actions */
#define INTERCEPT_HOOK(action_enum,action,intercept_action,no_intercept_action) {\
	Control* last_sel_control = sel_control;								\
	if((sel_intercept & Control::imask::action_enum) != 0) {  				\
		for(int i=0; !hasIntercepted; i++) {                        		\
			if(sel_intercept_vector.size() <= i) break;						\
			interceptInfo &v = sel_intercept_vector[i];             		\
			if(!v.parent_control || v.parent_control == sel_control) break; \
			if( (v.intercept_mask & Control::imask::action_enum) != 0) {  	\
				v.parent_control->m_is_intercepted = true;					\
				v.parent_control->action;                                   \
				intercept_action											\
				v.parent_control->m_is_intercepted = false;					\
				/*std::cout << "intercepted: " << last_sel_control->GetId() << " by: " << v.parent_control->GetId() << "\n";*/ \
			}                                                       		\
		}                                                           		\
	} 																		\
	if(hasIntercepted) {													\
		hasIntercepted = false; 											\
	} else if(last_sel_control) {											\
		last_sel_control->action;											\
		no_intercept_action\
	}}
	
	
#define INTERCEPT_HOOK_KEYBOARD(action_enum,action) {					\
	if(active_control) {												\
		std::stack<Control*> wgts;                                       \
		Control* w = active_control->parent;							\
		while(w) {                                 						\
			wgts.push(w);                          						\
			w = w->parent;                          					\
		}                                                               \
		while(!wgts.empty()) {                                          \
			Control* w = wgts.top();                                     \
			if((w->intercept_mask & Control::imask::action_enum) != 0) {        \
				w->m_is_intercepted = true;								\
				w->action;                                              \
				w->m_is_intercepted = false;							\
				if(hasIntercepted) {                                    \
					break;												\
				}														\
			}															\
			wgts.pop();													\
		}                                               				\
		if(hasIntercepted) {											\
			hasIntercepted = false; 									\
		} else {														\
			active_control->action;										\
		}																\
	}}
	

bool Gui::check_for_drag() {
	if(sel_control && sel_control->IsDraggable()) {
		dragging = true;
		Point p = cursor.GetCursor();
		Point gpos = sel_control->GetGlobalPosition();
		drag_start_diff = p - gpos;
		drag_offset = p - drag_start_diff;
		sel_control->emitEvent( "drag_start" );
	}
	return dragging;
}

bool Gui::check_for_lock() {
	if(m_focus_lock) {
		if(! m_lock_target->CheckCollisionA(cursor.GetCursor())) {
			
			// lose focus
			m_lock_target->emitEvent("lostfocus");
			m_lock_target->OnLostFocus();
			
			if(m_lock_once) {
				m_focus_lock = false;
				m_lock_once = false;
				if(m_unblock_all_events_on_mouse_up) {
					m_block_all_events = true;
				}
				// m_lock_target->emitEvent("lostcontrol");
				// m_lock_target->OnLostControl();
				unselectControls();
				Activate(0);
				// OnMouseMove( p.x, p.y );
				// std::cout << "lock once unlocked: sel_control now is: " << (sel_control ? sel_control->GetId() : "none") << "\n";
			}
			return true;
		}
	}
	return false;
}

void Gui::OnMouseDown( unsigned int button ) {
	std::unique_lock<std::mutex> lock(*mutex);
	if(m_block_all_events) {
		return;
	}
	m_mouse_down = true;
	
	Point p = cursor.GetCursor();
	
	Control* last_sel_control = sel_control;

	
#ifndef OVERLAPPING_CHECK
	if(!sel_control) {
		check_for_new_collision( p, true );
	}
#else
	check_for_new_collision( p, true );
#endif
	
	if(!m_focus_lock) {
		Activate(sel_control);
	}
	
	/*
	if(active_control) {
		std::cout << "active: " << active_control->GetId() << "\n";
	}
	*/
	
	Point parent_control_coords = p - sel_control_parent_window_position;
	bool was_locked = m_focus_lock;
	if( sel_control ) {
		
		Point control_coords = parent_control_coords - sel_control->GetRect();

		

		// mouse down event
		if( sel_control->CheckCollision(parent_control_coords) ) {
			
			// if not dragging, then pass event
			if(!check_for_drag()) {
				INTERCEPT_HOOK(mouse_down, OnMouseDown( control_coords.x, control_coords.y, (MouseButton)button ), 
					v.parent_control->emitEvent("mousedown", {control_coords.x, control_coords.y});
					,
					sel_control->emitEvent( "mousedown", {control_coords.x, control_coords.y} );
				);
			}
			
		} else {

			if(check_for_lock()) {
				return;
			}
			
			if(!check_for_drag() && sel_control && !dragging) {
				INTERCEPT_HOOK(mouse_down, OnMouseDown( control_coords.x, control_coords.y, (MouseButton)button ),
					,
					sel_control->OnGetFocus();
					sel_control->emitEvent( "mousedown", {control_coords.x, control_coords.y} );
				);
			}
		} 
		
	} else {
		if(sel_control != last_sel_control) {
			last_sel_control->emitEvent("lostfocus");
		}
	}
	
	if(sel_control) {
		Activate(sel_control);
		/*
		Point control_coords = parent_control_coords - sel_control->GetRect();
		INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, (MouseButton)button ),,);
		*/
	}
}

void Gui::SetTooltipDelay(double seconds) {
	m_tooltip_delay = seconds;
}

void Gui::OnMouseUp( unsigned int button ) {
	std::unique_lock<std::mutex> lock(*mutex);
	
	m_mouse_down = false;
	
	if(m_block_all_events) {
		// std::cout << "block all evts\n";
		if(m_unblock_all_events_on_mouse_up) {
			m_block_all_events = false;
			m_unblock_all_events_on_mouse_up = false;
		}
		return;
	}
	
	// if(check_for_lock()) {
		// return;
	// }
	
	Point p = cursor.GetCursor();

	// ------- dragging -----
	if(dragging) {
		Control* dragging_control = sel_control;
		unselectControl();
		check_for_new_collision(p);
		if(sel_parent) {
			dragging_control->emitEvent( "drag" );
		}
		dragging = false;
		return;
	}
	// -----------------------
		
	Point widget_coords = p-sel_control_parent_window_position;
	if( sel_control ) {
		const Rect &r = sel_control->GetRect();
		Point control_coords = widget_coords - r;
		INTERCEPT_HOOK(mouse_up, OnMouseUp( control_coords.x, control_coords.y, (MouseButton)button ),,
			if(sel_control->CheckCollision(widget_coords)) {
				sel_control->emitEvent( "click", {control_coords.x, control_coords.y} );
				if( button == MouseButton::BUTTON_RIGHT ) {
					sel_control->emitEvent( "rclick", {control_coords.x, control_coords.y} );
				}
				sel_control->OnClick(p.x, p.y, (MouseButton)button);
			}
			// std::cout << "MOUSE UP " << widget_coords << " " << sel_control->GetId() << "\n";
			sel_control->emitEvent( "mouseup", {control_coords.x, control_coords.y} );
		);
	}
	lock.unlock();
	OnMouseMove(p.x, p.y);
}

void Gui::ShowTooltip(Control* control) {
	m_last_cursor_update_time = m_time;
	if(!control) return;
	if(control->GetTooltip().empty()) return;
	std::cout << "should show tooltip\n";
	Point p = cursor.GetCursor();
	
	Point g = control->GetOffset();
	if(sel_control != control) return;
	
	if(!m_tooltip) {
		// std::cout << "show tooltip\n";
		m_tooltip = CreateControl<Label>("tooltip", "tooltip");
	}
	
	int distance = 25;
	m_tooltip->engine = this;
	
	// TODO: tooltip: fix this rect
	m_tooltip->SetRect(p.x+distance, p.y+distance, 9999, 9999);
	m_tooltip->SetText( sel_control->GetTooltip() );
	Rect r = m_tooltip->GetContentRect();
	m_tooltip->SetRect(p.x+distance, p.y+distance, r.w+10, r.h+10);
	m_tooltip->SetAlignment( Alignment::center );
	m_tooltip_shown = true;
}

void Gui::HideTooltip() {
	if(m_tooltip_shown) {
		m_tooltip_shown = false;
	}
}

void Gui::OnMouseMove( int mX, int mY ) {
	std::unique_lock<std::mutex> lock(*mutex);
	if(m_block_all_events) {
		return;
	}
	cursor.MoveCursor(mX, mY);
	Point p = cursor.GetCursor();
	m_last_cursor_update_time = m_time;
	Point widget_coords = p - sel_control_parent_window_position;
	HideTooltip();
	
	if(sel_control) {
		// if(sel_control->parent) std::cout << "m_focus_lock: " << m_focus_lock << m_lock_once << " " << sel_control->parent->sel_control->GetId() << ", " << sel_control->GetId()  << "\n";
		
		Rect r = sel_control->GetRect();
		Point control_coords = widget_coords - r;
		if( m_mouse_down || m_focus_lock ) {
			
			// drag
			if(m_mouse_down && sel_control->IsDraggable()) {
				drag_offset = p - drag_start_diff;
			}
			
			if(!dragging) {
				if(!m_mouse_down) {
					check_for_new_collision( p );
					r = sel_control->GetRect();
					widget_coords = p - sel_control_parent_window_position;
					control_coords = widget_coords - r;
				}
				
				// std::cout << "MMOVE: " << sel_control->GetId() << " " << control_coords << "\n";
				INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ),,);
			}
			return;
		}
		
		#ifdef OVERLAPPING_CHECK
		if(!m_keyboard_lock && !m_focus_lock) {
			
			Control *last_control = sel_control;
			#ifdef OVERLAPPING_CHECK
				check_for_new_collision( p, true );
			#else
				check_for_new_collision( p );
			#endif
			
			if(sel_control) {
				widget_coords = p - sel_control_parent_window_position;
				r = sel_control->GetRect();
				control_coords = widget_coords - r;
				
				if(last_control != sel_control) {
					last_control->OnLostFocus();
					last_control->emitEvent("lostfocus");
					INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ),,);
					
					return;
				}
			} else {
				last_control->emitEvent("lostfocus");
				last_control->OnLostFocus();
				return;
			}
		}
		#endif
		
		// if mouse over sel_control then pass event, else unselect and lose focus if not focus locked
		if(sel_control->CheckCollision(widget_coords)) {
			
			INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down),
				if(v.parent_control != v.last_control) {
					v.last_control = v.parent_control;
					// std::cout << "HOVER\n";
					v.last_control->emitEvent("hover");
				}
				,
			);
			
			if(!m_focus) {
				sel_control->OnGetFocus();
				m_focus = true;
			}
		} else if(!m_focus_lock) {
			sel_control->OnLostFocus();
			m_focus = false;
			if(!m_keyboard_lock) {
				unselectControl();
			}
		}
	} else {
		#ifdef OVERLAPPING_CHECK
			check_for_new_collision(p, true);
		#else
			check_for_new_collision(p);
		#endif
	}
}

bool Gui::IsKeyboardLocked() {
	return m_keyboard_lock;
}

void Gui::OnMWheel( int updown ) {
	std::unique_lock<std::mutex> lock(*mutex);
	if(sel_control) {
		INTERCEPT_HOOK(mwheel, OnMWheel( updown ),,);
	}
}

void Gui::check_for_new_collision( Point pt, bool start_from_top ) {
	Control* last_control = sel_control;
	Point o{0,0};
	Control *p = 0;
	
	
	if(m_focus_lock) {
		depth = 0;
		p = m_lock_target;
		if(p != sel_control) {
			unselectControl();
		}
		o = p->getAbsoluteOffset();
	} else {
		unselectControl();
	}
	
	std::vector<cache_entry>::reverse_iterator it, it_end;
	
	if(p) {
		it = p->cache.rbegin();
		it_end = p->cache.rend();
	} else {
		it = rootWidget.cache.rbegin();
		it_end = rootWidget.cache.rend();
		depth = 0;
		o = {0,0};
	}
	
	Control* last_widget = p;
	if(!last_widget) {
		last_widget = &rootWidget;
	}
	sel_parent = p;
	Point last_offset;
	
	
	// --- descending tree ---
	while(it != it_end) {
		if(it->interactible == 0) { it++; continue; }
		
		// std::cout << "checking collision: " << it->control->GetId() << " " << pt - o << " " << y-o.y<< "\n";
		if(it->control->checkCollision(pt - o, true)) {
			auto c = it->control;
			Rect r = c->GetRect();
			Control* w = it->control;

			last_widget->sel_control = w;
			last_widget = w;
			
			// per widget
			sel_intercept_vector[depth].intercept_mask = w->intercept_mask;
			sel_intercept_vector[depth].parent_control = w;
			
			w->cached_absolute_offset = o + it->rect;
			
			last_offset = o;
			o += it->rect + w->m_offset;
			
			it = w->cache.rbegin();
			it_end = w->cache.rend();
			sel_control = w;
			
			depth++;
		} else {
			it++;
		}
	}
	
	if(last_widget) {
		// std::cout << "last_widget: " << last_widget->GetId() << "\n";
		sel_control = last_widget;
		sel_parent = last_widget->parent;
		if(sel_parent) {
			sel_parent->sel_control = sel_control;
		}
		sel_control_parent_window_position = last_offset;
	}

	sel_intercept_vector[depth].parent_control = 0;
	sel_intercept_vector[depth].last_control = 0;
	
	// global intercept flag
	sel_intercept = 0;
	for(int i=0; i < depth; i++) {
		sel_intercept |= sel_intercept_vector[i].intercept_mask;
	}
		
	if(last_control != sel_control) {
		// std::cout << "new sel: " << sel_control->GetId() << "\n";
		if(last_control) {
			last_control->emitEvent("leave");
			// last_control->OnLostControl();
		}
		if(sel_control) {
			
			sel_control->emitEvent("hover");
			sel_control->OnGetFocus();
			m_focus = true;
			m_keyboard_lock = false;
		} 
	}
}

void Gui::Focus(Control* control) {
	if(!control || !control->engine || sel_control == control || !control->parent) return;

	if(sel_control && sel_control->parent == control->parent) {
		sel_control = control;
		Activate(active_control);
		control->parent->sel_control = control;
	} else {
		
		unselectControls();
		
		Control* w = control->parent;
		Control* p = control;
		// std::cout << "focus called to: " << p->GetId() << "\n";
		Control* wgt = control->parent;
		sel_parent = wgt;
		Point ofs{0,0};
		depth = 1;

		while(w->parent) {
			ofs += w->m_offset + w->m_rect;
			depth++;
			w->sel_control = p;
			p = w;
			w = w->parent;
		}
		// w->sel_control = p;
		rootWidget.sel_control = p;
		w = wgt;
		Point o = ofs;
		control->cached_absolute_offset = o + control->m_rect;
		int d = depth-1;
		sel_intercept = 0;
		
		while(w->parent) {
			o -= w->m_offset + w->m_rect;
			sel_intercept_vector[d].intercept_mask = w->intercept_mask;
			sel_intercept_vector[d].parent_control = w->parent;
			sel_intercept |= w->intercept_mask;
			d--;
			w->cached_absolute_offset = o;
			w = w->parent;
		}
		
		sel_intercept_vector[depth].parent_control = 0;
		sel_control = control;
		sel_control_parent_window_position = wgt->cached_absolute_offset;

		Activate(control);
	}
}

void Gui::OnText( std::string text ) {
	if(active_control) {
		INTERCEPT_HOOK_KEYBOARD(key_text, OnText( text ));
	}
}

void Gui::Activate(Control* control) {
	if(active_control == control) return;
	Control* old_active_control = active_control;
	
	if(control && control->engine == this && control->interactible) {
		active_control = control;
		active_control->emitEvent("activate");
		active_control->OnActivate();
	} else if(!control) {
		active_control = 0;
	}
	
	if(old_active_control) {
		Control* p = old_active_control;
		while(p && !p->inSelectedBranch()) {
			p->OnLostControl();
			p->emitEvent("lostcontrol");
			p = p->parent;
		}
	}
}

void Gui::unselectControls() {
	// unselectControl();
	
	// nullify all control->sel_control
	if(sel_parent) {
		for(Control* w = sel_parent; w; w = w->parent) {
			w->sel_control = 0;
		}
		sel_parent = 0;
	}
	sel_control = 0;
	depth = 0;
}


Control* Gui::GetSelectedControl() {
	return sel_control != &rootWidget ? sel_control : 0;
}

Control* Gui::GetSelectedWidget() {
	if(sel_control) { return sel_control->parent; } else return 0;
}

Control* Gui::GetActiveControl() {
	return active_control != &rootWidget ? active_control : 0;
}

void Gui::unselectControl() {
	if(!sel_control) {
		return;
	}
	
	// remove locks
	m_keyboard_lock = false;
	m_focus_lock = false;
	
	/*
	if(sel_control == &rootWidget) {
		rootWidget.sel_control = 0;
		sel_control = 0;
		return;
	}
	*/
	// TODO: review
	// sel_control->OnLostControl();
	// sel_control->emitEvent("leave");
	
	
	// sel_parent->sel_control = 0
	/*
	if(sel_parent) {
		std::cout << "Sel_control: " << sel_control->GetId() << "\n";
		std::cout << "sel_parent: " << sel_parent->GetId() << " : " << sel_control->parent->GetId() << "\n";
	}
	*/
	if(sel_control->parent) {
		sel_control->parent->sel_control = 0;
	}
	// select parent
	sel_control = sel_control->parent;
	
}

#ifdef USE_EVENT_QUEUE
bool Gui::HasEvents() {
	return !m_events.empty();
}

Event Gui::PopEvent() {
	Event evt = m_events.front();
	m_events.pop();
	return evt;
}
#endif

void Gui::SetRelativeMode(bool relative_mode) {
	cursor.SetRelativeMode(relative_mode);
}

// OnEvent function
void Gui::OnEvent( std::string id, std::string event_type, EventCallback callback ) {
	Control* c = rootWidget.Get<Control>(id);
	if(c) {
		c->OnEvent(event_type, callback);
	}
}

void Gui::MtLock() {
	mutex->lock();
}
void Gui::MtUnlock() {
	mutex->unlock();
}

std::map<std::string, EventCallback> Gui::function_map  __attribute__ ((init_priority (200)));

bool Gui::AddFunction( std::string function_name, EventCallback callback ) {
	function_map[function_name] = callback;
	return true;
}

void Gui::CallFunc( std::string function_name, const Argv& a ) {
	Args args;
	args.event_args = a;
	function_map[function_name](args);
}
// ------------------------------------------------------------------------------

void Gui::OnKeyDown( KeyFunc f) {
	m_keydown_cb.push_back(f);
}
void Gui::OnKeyUp( KeyFunc f) {
	m_keyup_cb.push_back(f);
}

void Gui::OnKeyDown( Keycode sym, Keymod mod) {
	std::unique_lock<std::mutex> lock(*mutex);
	if(active_control) {
		INTERCEPT_HOOK_KEYBOARD(key_down, OnKeyDown( sym, mod ));
	}
	for(auto i : m_keydown_cb) {
		i(sym,mod);
	}
}

void Gui::OnKeyUp( Keycode sym, Keymod mod ) {
	std::unique_lock<std::mutex> lock(*mutex);
	if(active_control) {
		INTERCEPT_HOOK_KEYBOARD(key_up, OnKeyUp( sym, mod ));
	}
	for(auto i : m_keyup_cb) {
		i(sym,mod);
	}
}

uint32_t Gui::GetFps() {
	if(m_delta_accum > 0.2) {
		return m_frames / m_delta_accum;
	} else {
		return m_fps;
	}
}

void Gui::renderInvalidate(Control* c) {
	
}

void Gui::Render() {
	if(m_on_render) {
		m_on_render();
	}
	std::unique_lock<std::mutex> lock(*mutex);
	static std::chrono::high_resolution_clock::time_point time_point = std::chrono::high_resolution_clock::now();
	
	// measure time for animations
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> d = now - time_point;
	double last_time = m_time;
	m_time = d.count();
	m_delta_time = m_time - last_time;
	// --------
	
	// measure fps
	m_frames++;
	m_delta_accum += m_delta_time;
	if(m_delta_accum > 1.0) {
		// m_fps = m_frames / m_delta_accum;
		m_fps = m_frames;
		m_delta_accum = 0;
		m_frames = 0;
		if(m_on_fps_change) {
			m_on_fps_change(m_fps);
		}
	}
	// ---------
	
	if(!m_tooltip_shown && sel_control && 
		m_time - m_last_cursor_update_time > m_tooltip_delay)
	{
		ShowTooltip(sel_control);
	}
	
	bool dragging_widget_workaround = false;
	if(dragging && sel_control->parent) {
		dragging_widget_workaround = true;
		sel_control->visible = false;
		sel_control->update_cache(CacheUpdateFlag::attributes);
	}
	
	bool has_selected_control = false;
	Point pos(0,0);
	
	// rootWidget.render(pos, true);
	
	/*
	if(rootWidget.sel_control) {
		Control* w = &rootWidget;
		std::cout << "SELECTION TREE: ";
		while(w) {
			std::cout << w->GetId() << " => ";
			w = w->sel_control;
		}
		std::cout << "\n";
	}
	*/
	// TODO: review
	// std::cout << "cycle" << "\n";
	for(auto &ca : rootWidget.cache) {
		Control* const &c = ca.control;
		if(ca.visible) {
			// std::cout << c->GetId() << "\n";
			if(c != sel_control) {
				c->render(pos, c == rootWidget.sel_control);
			} else {
				#ifdef SELECTED_CONTROL_ON_TOP
					has_selected_control = true;
				#else
					if(!dragging) {
						c->render(pos, true);
					}
				#endif
			}
		}
	}
	
	
	if(dragging_widget_workaround) {
		sel_control->visible = true;
		sel_control->update_cache(CacheUpdateFlag::attributes);
	}
	
	if(dragging) {
		const Rect& r = sel_control->GetRect();
		sel_control->render(pos - r + drag_offset, true);
	} else {
		if(has_selected_control) {
			sel_control->render(pos,true);
		}
	}
	
	if(m_tooltip_shown) {
		m_tooltip->render( Point(0,0), true );
	}
	
	cursor.Render(backend.screen);
}
	
Cursor& Gui::GetCursor() {
	return cursor;
}

Point Gui::GetCursorPosition() {
	return cursor.GetCursor();
}

void Gui::SetBackend(Backend backend) {
	this->backend = backend;
	
	if(backend.screen) {
		backend.screen->Init();
		backend.screen->SetResolution(resolution.w, resolution.h);
	}
	if(backend.speaker) {
		backend.speaker->Init();
	}
	if(backend.screen) {
		ProcessLayout();
	}
}

const Backend& Gui::GetBackend() {
	return backend;
}

GUIFUNC(debug) {
	if(args.cmd_args.empty()) return;
	std::cout << "dbg control: " << args.control->GetId() << ": " << args.cmd_args[0] << "\n";
}

// ------ forward to rootwidget -----

void Gui::AddControl(Control* control, bool processlayout) {
	rootWidget.AddControl(control, processlayout);
}

void Gui::LoadXml(std::string xml_filename) {
	rootWidget.LoadXml(xml_filename);
}

void Gui::LoadXml(std::istream& stream) {
	rootWidget.LoadXml(stream);
}

void Gui::BreakRow() {
	rootWidget.BreakRow();
}

void Gui::RemoveControls() {
	rootWidget.RemoveControls();
}

void Gui::EnableStyleGroup(std::string name, bool enable) {
	for(auto &s : ControlManager::group_styles) {
		if(s.name == name) {
			s.disabled = !enable;
			break;
		}
	}
}

void Gui::DisableAllStyles() {
	for(auto &s : ControlManager::group_styles) {
		s.disabled = true;
		if(s.tag_id != 0) {
			ForEachControl([&](Control* c) {
				for(auto it = c->subscribers.begin(); it != c->subscribers.end(); ) {
					if(it->tag == s.tag_id) {
						it = c->subscribers.erase(it);
						if(it == c->subscribers.end()) {
							break;
						}
					} else {
						it++;
					}
				}
			});
		}
	}
}

const std::vector<Control*>& Gui::GetControls() {
	return rootWidget.GetControls();
}

Control* Gui::get(const std::string& id) {
	return rootWidget.get(id);
}

Control* Gui::CreateControl(std::string type, std::string id) {
	return ControlManager::CreateControl(type, id);
}

void Gui::ProcessLayout(bool asRoot) {
	rootWidget.ProcessLayout();
}

void Gui::ForEachControl(std::function<void(Control* c)> func) {
	rootWidget.ForEachControl(func);
}



}
