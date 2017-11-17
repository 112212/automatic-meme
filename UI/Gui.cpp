#include "Gui.hpp"
#include "Widget.hpp"
#include <stack>
#include <cassert>
#include <iostream>
#include <ratio>
#include <chrono>
#include "Control.hpp"

#include "controls/Label.hpp" // only for tooltip
#include "managers/Sounds.hpp"

namespace ng {

Backend default_backend(new Screen(), new Speaker());

void Gui::play_sound( Args& args ) {
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



Gui::Gui() : ControlManager(this) {
	m_mouse_down = false;
	m_focus = false;
	m_keyboard_lock = false;
	m_focus_lock = false;
	m_widget_lock = false;
	m_lock_once = false;
	m_time = 0;
	dragging = false;
	m_frames = 0;
	drag_offset = {0,0};
	depth = 0;
	selected_control = 0;
	active_control = 0;
	sel_first_depth_widget = 0;
	last_selected_widget = 0;
	
	hasIntercepted = false;
	sel_intercept = 0;
	sel_intercept_vector.resize(15);
	
	selection_margin = 1;
	selection_margin_control_min_size = Size(10,10);
	
	m_tooltip_shown = false;
	m_tooltip_delay = 2;
	m_tooltip = 0;
	SetBackend(default_backend);
	AddFunction( "play_sound", play_sound );
}

void Gui::HideOSCursor() {
	backend.system->SetCursorVisibility(false);
}


Gui::Gui(int xsize, int ysize) : Gui() {
	SetSize(xsize, ysize);
}

Gui& Gui::operator=(Gui && engine) {
	*this = engine;
	this_engine = this;
	return *this;
}

Gui::~Gui() {
	Clear();
}

void Gui::Clear() {
	for(Control* c : controls) {
		RemoveControl(c);
	}
}

void Gui::SetDefaultFont(std::string font, int size) {
	Fonts::LoadFont(font, "default", size);
}

void Gui::SetSize(int w, int h) {
	if(backend.screen) {
		backend.screen->Init();
		backend.screen->SetResolution(w, h);
		resolution = Size(w,h);
	}
	ProcessLayout();
}

Size Gui::GetSize() {
	int x,y;
	backend.screen->GetResolution(x,y);
	resolution = Size(x,y);
	return resolution;
}

void Gui::LockWidget(Widget* widget) {
	if(!widget or widget->engine != this) return;
	sel_first_depth_widget = widget;
	Point ofs = {0,0};
	Widget *w;
	if(selected_control) {
		w = selected_control->getWidget();
		while(w && w != widget) {
			w = w->getWidget();
		}
		if(w != widget) {
			unselectControl();
			unselectWidgets();
		} else {
			int d = depth;
			w = last_selected_widget;
			while(w != widget) {
				d--;
				w = w->widget;
			}
			int dff = depth - d;
			for(int i=0; i < dff; i++) {
				sel_intercept_vector[i] = sel_intercept_vector[i+d-1];
			}
			sel_intercept_vector[dff].widget = 0;
			depth = dff;
		}
	} else {
		unselectWidgets();
		last_selected_widget = widget;
		
		sel_intercept_vector[0].intercept_mask = widget->intercept_mask;
		sel_intercept_vector[0].widget = widget;
		sel_intercept_vector[1].widget = 0;
		sel_intercept = widget->intercept_mask;
		depth = 1;
	}
	
	w = sel_first_depth_widget;
	while(w) {
		ofs = {ofs.x + w->m_rect.x, ofs.y + w->m_rect.y};
		w = w->widget;
	}
	sel_widget_offset = ofs;
	m_widget_lock = true;
}

void Gui::UnlockWidget() {
	if(!m_widget_lock) return;
	
	// get real depth
	int d = -1;
	Widget *w = sel_first_depth_widget;
	while(w) {
		w = w->widget;
		d++;
	}
	
	// fix intercept vector
	// shift intercept vector
	for(int i=depth; i >= 0; i--) {
		sel_intercept_vector[i+d] = sel_intercept_vector[i];
	}
	// add intercepts from engine to widget
	w = sel_first_depth_widget;
	w = w->widget;
	depth += d;
	while(w) {
		d--;
		sel_intercept_vector[d].widget = w;
		sel_intercept_vector[d].intercept_mask = w->intercept_mask;
		sel_intercept |= w->intercept_mask;
		if(d == 0) {
			sel_first_depth_widget = w;
		}
		w = w->widget;
	}

	assert(d == 0);

	m_widget_lock = false;
}

Control* Gui::GetControlById(std::string id) {
	auto it = map_id_control.find(id);
	if(it != map_id_control.end())
		return it->second;
	else
		return 0;
}

void Gui::processControlEvent(int event_type) {
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
			break;
		case GUI_LOCK_ONCE:
			m_lock_once = true;
			break;
		case GUI_UNSELECT:
			unselectControl();
			return;
		case GUI_WIDGET_UNLOCK: {
				m_widget_lock = false;
				if(selected_control) {
					Widget* w = static_cast<Widget*>(selected_control);
					while(w && w->widget) {
						w = w->widget;
					}
					if(w) sel_first_depth_widget = w;
				}
			}
		case GUI_UNSELECT_WIDGET:
			unselectWidget();
			break;
		case GUI_UNSELECT_WIDGETS:
			unselectWidgets();
			break;
	}
}
	  
void Gui::AddControl( Control* control ) {
	if(control->engine) return;
	
	if(control->isWidget) {
		Widget* w = static_cast<Widget*>(control);

		recursiveProcessWidgetControls(w, true);
		
		w->set_engine(this);
	} else {
		control->engine = this;
	}
	
	addControlToCache(control);
	
	map_id_control[control->id] = control;
}

/*
	TODO: remove all events which are still in queue for this control which is being removed
		  if its widget, then remove all children events from event queue
*/
void Gui::RemoveControl( Control* control ) {
	if(control->engine != this) {
		return;
	}
	
	if(selected_control == control) {
		unselectControl();
	}
	
	std::string id = control->id;
	
	map_id_control.erase(id);
	
	// if its widget, make sure nothing breaks
	if(control->isWidget) {
		Widget* widget = (Widget*)control;
		
		recursiveProcessWidgetControls(widget, false);
		
		if(sel_first_depth_widget == widget) {
			unselectWidgets();
			sel_first_depth_widget = 0;
			m_widget_lock = false;
		}
		// if any of selected controls or widgets are inside this widget, we must break selection
		if(selected_control) {
			Widget* w = selected_control->widget;
			while(w && w != widget) {
				w = w->widget;
			}
			if(w == widget) {
				unselectControl();
			}
		}
		
		if(last_selected_widget) {
			Widget* w = last_selected_widget;
			while(w && w != widget) {
				w = w->widget;
			}
			if(w == widget) {
				unselectWidgets();
			}
		}
	}
	
	if(!control->widget) {
		removeControlFromCache(control);
	} else {
		control->widget->removeControlFromCache(control);
	}
	control->engine = 0;
	control->widget = 0;
}

#define INTERCEPT_HOOK(action_enum,action) {								\
	/* TODO: what if gets deleted in one of the actions */					\
	Control* last_selected_control = selected_control;						\
	if((sel_intercept & Widget::imask::action_enum) != 0) {  				\
		for(int i=0; !hasIntercepted; i++) {                        		\
			if(sel_intercept_vector.size() <= i) break;						\
			interceptInfo &v = sel_intercept_vector[i];             		\
			if(!v.widget) break;                                    		\
			if( (v.intercept_mask & Widget::imask::action_enum) != 0) {  	\
				v.widget->action;                                   		\
			}                                                       		\
		}                                                           		\
	} 																		\
	if(hasIntercepted) {													\
		hasIntercepted = false; 											\
	} else if(last_selected_control) {										\
		last_selected_control->action;										\
	}}
	
#define INTERCEPT_HOOK_KEYBOARD(action_enum,action) {					\
	if(active_control) {												\
		std::stack<Widget*> wgts;                                       \
		Widget* w = active_control->widget;								\
		while(w) {                                 						\
			wgts.push(w);                          						\
			w = w->widget;                          					\
		}                                                               \
		while(!wgts.empty()) {                                          \
			Widget* w = wgts.top();                                     \
			if(w->intercept_mask & Widget::imask::action_enum) {        \
				w->action;                                              \
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
	
#define WIDGET_HOOK(action_enum,action) if(last_selected_widget) { 		\
	if((sel_intercept & Widget::imask::action_enum) != 0) {				\
		/*sel_first_depth_widget->action;*/								\
		for(int i=0; !hasIntercepted; i++) {                        	\
			interceptInfo &v = sel_intercept_vector[i];             	\
			if(!v.widget) break;                                    	\
			if((v.intercept_mask                                    	\
				& Widget::imask::action_enum) != 0) {          			\
				v.widget->action;                                   	\
			}                                                       	\
		}    															\
	} 																	\
	if(hasIntercepted)													\
		hasIntercepted = false; 										\
}

bool Gui::check_for_drag() {
	if(selected_control && selected_control->IsDraggable()) {
		dragging = true;
		Point p = cursor.GetCursor();
		Point gpos = selected_control->GetGlobalPosition();
		drag_start_diff = Point(p.x - gpos.x, p.y - gpos.y);
		drag_offset = Point(p.x - drag_start_diff.x, p.y - drag_start_diff.y);
		selected_control->emitEvent( "drag_start" );
	}
	return dragging;
}

void Gui::OnMouseDown( unsigned int button ) {
	m_mouse_down = true;
	
	Point p = cursor.GetCursor();
	
#ifndef OVERLAPPING_CHECK
	if(!selected_control) {
		check_for_new_collision( p.x, p.y, true );
	}
#else
	if(!m_focus_lock) {
		check_for_new_collision( p.x, p.y, true );
	}
#endif
	Point control_coords{p.x-sel_widget_offset.x, p.y-sel_widget_offset.y};
	active_control = selected_control;
	if( selected_control ) {
		
		if(m_focus_lock) {
			INTERCEPT_HOOK(mouse_down, OnMouseDown( control_coords.x, control_coords.y, (MouseButton)button ));
			
			// if still locked
			if(m_focus_lock) {
				return;
			}
			
			if(m_lock_once) {
				m_lock_once = false;
				return;
			}
		}

		if( selected_control->CheckCollision(control_coords.x, control_coords.y) ) {
			
			if(!check_for_drag()) {
				selected_control->emitEvent( "mousedown", {control_coords.x-selected_control->m_rect.x, control_coords.y-selected_control->m_rect.y} );
				INTERCEPT_HOOK(mouse_down, OnMouseDown( control_coords.x, control_coords.y, (MouseButton)button ));
			}
			
		} else {
			unselectControl();
			check_for_new_collision( p.x, p.y );
			
			if(!check_for_drag() && selected_control && !dragging) {
				selected_control->OnGetFocus();
				INTERCEPT_HOOK(mouse_down, OnMouseDown( p.x-sel_widget_offset.x, p.y-sel_widget_offset.y, (MouseButton)button ));
			}
		}
		
	} else {
		WIDGET_HOOK(mouse_down, OnMouseDown( p.x, p.y, (MouseButton)button )); 
	}
	
	if(selected_control) {
		active_control = selected_control;
	}
}

void Gui::SetTooltipDelay(double seconds) {
	m_tooltip_delay = seconds;
}

void Gui::OnMouseUp( unsigned int button ) {
	m_mouse_down = false;
	
	int mX, mY;
	Point p = cursor.GetCursor();
	mX = p.x;
	mY = p.y;
	
	// ------- dragging -----
	if(dragging) {
		Control* dragging_control = selected_control;
		UnselectControl();
		check_for_new_collision(mX, mY);
		if(last_selected_widget) {
			dragging_control->emitEvent( "drag" );
		}
		dragging = false;
		return;
	}
	// -----------------------
		
	Point widget_coords{mX-sel_widget_offset.x, mY-sel_widget_offset.y};
	if( selected_control ) {
		INTERCEPT_HOOK(mouse_up, OnMouseUp( widget_coords.x, widget_coords.y, (MouseButton)button ));
		if(selected_control->CheckCollision(widget_coords.x, widget_coords.y)) {
			Rect r = selected_control->GetRect();
			Point control_coords{widget_coords.x-r.x, widget_coords.y-r.y};
			// std::cout << "button: " << button << "\n";
			selected_control->emitEvent( "click", {control_coords.x, control_coords.y} );
		}
	} else {
		WIDGET_HOOK(mouse_up, OnMouseUp( mX, mY, (MouseButton)button ));
	}
}

void Gui::ShowTooltip(Control* control) {
	m_last_cursor_update_time = m_time;
	if(!control) return;
	if(control->GetTooltip().empty()) return;
	Point p = cursor.GetCursor();
	Point g = control->GetOffset();
	if(!control->CheckCollision(p.x-g.x, p.y-g.y)) return;
	if(!m_tooltip) {
		m_tooltip = (Label*)CreateControl("tooltip", "tooltip");
	}
	int distance = 25;
	// TODO: tooltip: fix this rect
	m_tooltip->SetRect(p.x+distance, p.y+distance, 200, 100);
	m_tooltip->SetText( selected_control->GetTooltip() );
	m_tooltip->SetAlignment( Alignment::center );
	m_tooltip_shown = true;
}

void Gui::HideTooltip() {
	if(m_tooltip_shown) {
		m_tooltip_shown = false;
	}
}

void Gui::OnMouseMove( int mX, int mY ) {
	
	cursor.MoveCursor(mX, mY);
	Point p = cursor.GetCursor();
	mX = p.x;
	mY = p.y;
	m_last_cursor_update_time = m_time;
	// return;
	
	Point control_coords{mX-sel_widget_offset.x, mY-sel_widget_offset.y};
	HideTooltip();
	if(selected_control) {
		if( m_mouse_down || m_focus_lock ) {
			if(m_mouse_down && selected_control->IsDraggable()) {
				Point pt = selected_control->GetGlobalPosition();
				drag_offset = Point(mX - drag_start_diff.x, mY - drag_start_diff.y);
			}
			
			if(!dragging) {
				INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
			}
			return;
		}
		
		#ifdef OVERLAPPING_CHECK
		if(!m_keyboard_lock) {
			Control *last_control = selected_control;
			#ifdef OVERLAPPING_CHECK
				check_for_new_collision( mX, mY, true );
			#else
				check_for_new_collision( mX, mY );
			#endif
			if(selected_control) {
				if(last_control != selected_control) {
					last_control->OnLostFocus();
					INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
					return;
				}
			} else {
				last_control->OnLostFocus();
				WIDGET_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
				return;
			}
		}
		#endif
		
		if(selected_control->CheckCollision(control_coords.x, control_coords.y))
		{
			INTERCEPT_HOOK(mouse_move, OnMouseMove( control_coords.x, control_coords.y, m_mouse_down ));
			if(!m_focus) {
				selected_control->OnGetFocus();
				m_focus = true;
			}
		} else {
			selected_control->OnLostFocus();
			m_focus = false;
			if(!(m_focus_lock || m_keyboard_lock)) {
				unselectControl();
			}
		}
	} else {
		WIDGET_HOOK(mouse_move, OnMouseMove( mX, mY, m_mouse_down ));
		#ifdef OVERLAPPING_CHECK
			check_for_new_collision( mX, mY, true );
		#else
			check_for_new_collision( mX, mY );
		#endif
	}
}

void Gui::OnMWheel( int updown ) {
	if(selected_control) {
		INTERCEPT_HOOK(mwheel, OnMWheel( updown ));
	} else {
		WIDGET_HOOK(mwheel, OnMWheel( updown ));
	}
}

void Gui::check_for_new_collision( int x, int y, bool start_from_top ) {
	Control* last_control = selected_control;
	Point offset{0,0};
	Point &o = offset;
	Widget *p = 0;
	
	if(!start_from_top) {
		if(last_selected_widget || m_widget_lock) {
			offset = sel_widget_offset;
			if(m_widget_lock) {
				p = sel_first_depth_widget;
			}
		}
		
		if(last_selected_widget) {
			p = last_selected_widget;
			if(m_widget_lock) {
				Rect r = p->m_rect;
				while(p != sel_first_depth_widget and !(x >= o.x && x <= o.x + r.w && 
					y >= o.y && y <= o.y + r.h)
					) {
					p->selected_control = 0;
					const Point &o2 = p->m_offset;
					o = {o.x - r.x - o2.x, o.y - r.y - o2.y};
					p = p->widget;
					r = p->m_rect;
					depth--;
				}
				if(p == sel_first_depth_widget and !(x >= o.x && x <= o.x + r.w && 
					y >= o.y && y <= o.y + r.h)) {
					// mouse not in locked widget
					return;
				}
			} else {
				Rect r = p->m_rect;
				while(p && !(x >= o.x && x <= o.x + r.w && 
					y >= o.y && y <= o.y + r.h)) {
					p->selected_control = 0;
					const Point &o2 = p->m_offset;
					o = {o.x - r.x - o2.x, o.y - r.y - o2.y};
					p = p->widget;
					if(p) {
						r = p->m_rect;
					}
					depth--;
				}
			}
		}
	
		if(p != last_selected_widget) {
			unselectControl();
		}
	} else {
		
		unselectWidgets();
	}
	
	std::vector<cache_entry>::reverse_iterator it, it_end;
	
	if(p) {
		it = p->cache.rbegin();
		it_end = p->cache.rend();
	} else {
		it = cache.rbegin();
		it_end = cache.rend();
		depth = 0;
		o = {0,0};
	}
	
	Widget* last_widget = p;
	last_selected_widget = p;
	sel_widget_offset = o;
	
	// --- descending tree ---
	while(it != it_end) {
		if(!it->interactible) { it++; continue; }
		
		/*
		bool in = false;
		{
			Rect& r = it->rect;
			int x1 = r.x + o.x;
			int x2 = x1 + r.w;
			int y1 = r.y + o.y;
			int y2 = y1 + r.h;
			
			if(r.w > selection_margin_control_min_size.w) {
				x1 += selection_margin;
				x2 -= selection_margin;
			}
			if(r.h > selection_margin_control_min_size.h) {
				y1 += selection_margin;
				y2 -= selection_margin;
			}
			
			if(x >= x1 && x <= x2 &&
			   y >= y1 && y <= y2) {
			   in = true;
			}
		}
		*/
		
		// std::cout << "checking collision: " << it->control->GetId() << "\n";
		if(it->control->CheckCollision(x-o.x,y-o.y)) {
			auto c = it->control;
			Rect r = c->GetRect();
			// std::cout << "[" << x << ", " << y << "]" << "found collision: " << (x-o.x) << ", " << (y-o.y) << " " << it->control->GetId() << " : " << r.x << ", " << r.y << ", " << "\n";
			if(it->isWidget) {
				Widget* w = static_cast<Widget*>(it->control);
				if(depth == 0 && !m_widget_lock) {
					sel_first_depth_widget = w;
				} else if(last_widget) {
					last_widget->selected_control = w;
				}
				last_widget = w;
				
				// per widget
				sel_intercept_vector[depth].intercept_mask = w->intercept_mask;
				sel_intercept_vector[depth].widget = w;
				
				w->cached_absolute_offset.x = offset.x + it->rect.x;
				w->cached_absolute_offset.y = offset.y + it->rect.y;
				
				offset.x += it->rect.x + w->m_offset.x;
				offset.y += it->rect.y + w->m_offset.y;
				
				it = w->cache.rbegin();
				it_end = w->cache.rend();
			} else {
				selected_control = it->control;
				if(last_widget) {
					last_widget->selected_control = selected_control;
				}
				break;
			}
			depth++;
		} else {
			it++;
		}
	}
	
	if(last_widget) {
		last_selected_widget = last_widget;
		sel_widget_offset = offset;
	}
		
	sel_intercept_vector[depth].widget = 0;
	
	// global intercept flag
	sel_intercept = 0;
	for(int i=0; i < depth; i++) {
		sel_intercept |= sel_intercept_vector[i].intercept_mask;
	}
		
	if(last_control != selected_control) {
		if(last_control) {
			last_control->emitEvent("leave");
			last_control->OnLostControl();
		}
		if(selected_control) {
			
			selected_control->emitEvent("hover");
			selected_control->OnGetFocus();
			m_focus = true;
			m_keyboard_lock = false;
		}
	}
}

void Gui::Focus(Control* control) {
	if(!control || !control->engine || selected_control == control) return;
	
	if(control->widget) {
		if(selected_control && selected_control->widget == control->widget) {
			selected_control = control;
			active_control = control;
			control->widget->selected_control = control;
		} else {
			Widget* w = control->widget;
			Widget* wgt = w;
			last_selected_widget = w;
			Point ofs{0,0};
			while(w->widget) {
				ofs.x += w->m_offset.x;
				ofs.y += w->m_offset.y;
				w = w->widget;
			}
			sel_first_depth_widget = w;
			w = wgt;
			Point o = ofs;
			while(w->widget) {
				o.x -= w->m_offset.x;
				o.y -= w->m_offset.y;
				w = w->widget;
				w->cached_absolute_offset = o;
			}
			selected_control = control;
			active_control = control;
			wgt->selected_control = selected_control;
		}
	} else {
		active_control = control;
		selected_control = control;
	}
}

void Gui::OnText( std::string text ) {
	if(active_control) {
		INTERCEPT_HOOK_KEYBOARD(key_text, OnText( text ));
	}
}

void Gui::Activate(Control* control) {
	if(control && control->engine == this && control->interactible) {
		active_control = control;
		active_control->OnGetFocus();
	} else if(!control) {
		active_control = 0;
	}
}

void Gui::recursiveProcessWidgetControls(Widget* wgt, bool add_or_remove) {
	Widget* w = wgt;
	for(auto it = w->cache.begin(); it != w->cache.end(); it++) {
		if(it->isWidget) {
			if(add_or_remove) {
				map_id_control[it->control->id] = it->control;
			} else {
				map_id_control.erase(it->control->id);
			}
			recursiveProcessWidgetControls((Widget*)it->control, add_or_remove);
		} else {
			if(add_or_remove) {
				map_id_control[it->control->id] = it->control;
			} else {
				map_id_control.erase(it->control->id);
			}
		}
	}
}

void Gui::unselectWidget() {
	if(last_selected_widget) {
		if(selected_control) {
			unselectControl();
		}
		Widget* lsw = last_selected_widget;
		lsw = lsw->widget;
		if(lsw) {
			lsw->selected_control = 0;
		}
		last_selected_widget = lsw;
		depth--;
	}
}

void Gui::unselectWidgets() {
	if(selected_control) {
		unselectControl();
	}
	if(last_selected_widget) {
		Widget* w = last_selected_widget;
		while(w) {
			w->selected_control = 0;
			w = w->widget;
		}
		last_selected_widget = 0;
		depth = 0;
	}
	// active_control = 0;
}

void Gui::unselectControl() {
	if(!selected_control) return;
	selected_control->OnLostControl();
	selected_control->emitEvent("leave");
	m_focus_lock = false;
	m_keyboard_lock = false;
	if(selected_control->widget) {
		selected_control->widget->selected_control = 0;
	}
	selected_control = 0;
}

#ifdef USE_EVENT_QUEUE
	bool Gui::HasEvents( ) {
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
	auto it = map_id_control.find(id);
	if(it != map_id_control.end()) {
		it->second->OnEvent(event_type, callback);
	}
}

std::map<std::string, EventCallback> Gui::function_map  __attribute__ ((init_priority (200))) ;

bool Gui::AddFunction( std::string function_name, EventCallback callback ) {
	function_map[function_name] = callback;
	return true;
}

// ------------------------------------------------------------------------------


	void Gui::OnKeyDown( Keycode sym, Keymod mod) {
		if(active_control) {
			INTERCEPT_HOOK_KEYBOARD(key_down, OnKeyDown( sym, mod ));
		}
	}

	void Gui::OnKeyUp( Keycode sym, Keymod mod ) {
		if(active_control) {
			INTERCEPT_HOOK_KEYBOARD(key_up, OnKeyUp( sym, mod ));
		}
	}
	
	uint32_t Gui::GetFps() {
		if(m_delta_accum > 0.2) {
			return m_frames / m_delta_accum;
		} else {
			return m_fps;
		}
	}
	
	void Gui::Render() {
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
		
		if(!m_tooltip_shown && selected_control && 
			m_time - m_last_cursor_update_time > m_tooltip_delay)
		{
			ShowTooltip(selected_control);
		}
		
		bool dragging_widget_workaround = false;
		if(dragging && selected_control->widget) {
			dragging_widget_workaround = true;
			selected_control->visible = false;
			selected_control->update_cache(CacheUpdateFlag::attributes);
		}
		
		bool has_selected_control = false;
		Point pos(0,0);
		for(auto &ca : cache) {
			if(ca.visible) {
				Control* const &c = ca.control;
				
				if(c != selected_control) {
					c->render(pos, c == sel_first_depth_widget);
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
			selected_control->visible = true;
			selected_control->update_cache(CacheUpdateFlag::attributes);
		}
		
		if(dragging) {
			const Rect& r = selected_control->GetRect();
			selected_control->render(pos.Offset(Point(-r.x, -r.y)).Offset(drag_offset), true);
		} else {
			if(has_selected_control) {
				selected_control->render(pos,true);
			}
		}
				
		if(m_tooltip_shown) {
			m_tooltip->engine = this;
			m_tooltip->interactible = false;
			static_cast<Control*>(m_tooltip)->render( Point(0,0), false );
			m_tooltip->engine = 0;
		}
		
		cursor.Render(backend.screen);
		
	}
	
Cursor& Gui::GetCursor() {
	return cursor;
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


}
