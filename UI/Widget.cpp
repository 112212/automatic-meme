#include "RapidXML/rapidxml.hpp"
#include "controls/RadioButton.hpp"
#include <iostream>

#include "Widget.hpp"
#include "Gui.hpp"

namespace ng {
Widget::Widget() : Control(), ControlManager(this), 
	selected_control(0), intercept_mask(0) {
	setType( "widget" );
	isWidget = true;
	m_offset = {0,0};
	
	layout.SetSizeRange( 0, 0, 0, 0, 9999, 0, 9999, 0 );
	
	min.w = 0;
	min.h = 0;
	max.w = 0;
	max.h = 0;
}

Widget::~Widget() {
	
}

void Widget::set_engine(Gui* engine) {
	this->engine = engine;
	for(Control* c : controls) {
		if(c->isWidget) {
			Widget* w = static_cast<Widget*>(c);
			w->set_engine(engine);
		} else {
			c->engine = engine;
		}
	}
}

void Widget::intercept() {
	if(engine) {
		engine->hasIntercepted = true;
	}
}

void Widget::setInterceptMask(unsigned int mask) {
	intercept_mask = mask;
}

void Widget::AddControl( Control* control ) {
	if(control->widget or control->engine) {
		return;
	}
	
	control->widget = this;
	
	if(engine) {
		if(control->isWidget) {
			Widget* w = static_cast<Widget*>(control);
			w->set_engine(engine);
			engine->recursiveProcessWidgetControls(w, true);
		} else {
			engine->map_id_control[control->id] = control;
			control->engine = engine;
		}
	}
	
	addControlToCache(control);
	if(engine) {
		ProcessLayout();
	}
}

bool Widget::isSelected() {
	if(engine) {
		return engine->last_selected_widget == this;
	} else {
		return false;
	}
}

bool Widget::inSelectedBranch() {
	return selected_control != 0 or isSelected();
}

void Widget::LockWidget(bool lock) {
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

void Widget::RemoveControl( Control* control ) {
	removeControlFromCache(control);
	control->widget = 0;
	control->engine = 0;
}

void Widget::setRect( int x, int y, int w, int h ) {
	if(inSelectedBranch()) {
		engine->sel_widget_offset.x += x-m_rect.x;
		engine->sel_widget_offset.y += y-m_rect.y;
	}
}

void Widget::SetOffset(int x, int y) {
	if(inSelectedBranch()) {
		engine->sel_widget_offset.x += x-m_offset.x;
		engine->sel_widget_offset.y += y-m_offset.y;
	}
	m_offset.x = x;
	m_offset.y = y;
}

Control* Widget::Clone() {
	Widget *w = new Widget;
	copyStyle(w);
	return w;
}

void Widget::Render( Point position, bool isSelected ) {
	RenderWidget(position,isSelected);
}

void Widget::RenderWidget( Point position, bool isSelected ) {

	bool use_clipping = true;
	
	Rect old_box;
	bool save_clipping;
	
	Point orig_position = position;
	
	Control::Render(orig_position, isSelected);
	
	position = position.Offset(m_rect);
	// ------ clipping ---------
	// must go between these 2 "position.Offset"
	if(use_clipping) {
		save_clipping = Drawing().GetClipRegion(old_box.x, old_box.y, old_box.w, old_box.h);
		int margin = 1;
		old_box.x-=margin;
		old_box.y-=margin;
		old_box.h+=margin;
		old_box.w+=margin;
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
			auto &c = ca.control;
			#ifdef SELECTED_CONTROL_ON_TOP
				if(c != selected_control) {
					c->render( position, false );
				}
			#else
				c->render( position, isSelected && c == selected_control );
			#endif
		}
	}
	
	#ifdef SELECTED_CONTROL_ON_TOP
	if(selected_control && selected_control->visible) {
		selected_control->render(position,true);
	}
	#endif
	
	if(use_clipping) {
		Drawing().PopClipRegion();
	}
	
}

std::string Widget::GetSelectedRadioButton(int group) {
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

void Widget::parseXml(rapidxml::xml_node<>* node) {
	Layout layout;
	for(;node;node=node->next_sibling()) {
		parseAndAddControl(node, layout);
	}
}

}
