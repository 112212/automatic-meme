#include "Widget.hpp"
#include "Gui.hpp"

namespace ng {
Widget::Widget() : ControlManager(this), 
	selected_control(0), intercept_mask(0) {
	setType(TYPE_WIDGET);
	isWidget = true;
	offset = {0,0};
}

Widget::~Widget() {
}

void Widget::set_engine(GuiEngine* engine) {
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
	if(engine)
		engine->hasIntercepted = true;
}

void Widget::setInterceptMask(unsigned int mask) {
	intercept_mask = mask;
}

void Widget::AddControl( Control* control ) {
	if(control->widget or control->engine) return;
	control->widget = this;
	if(engine) {
		if(control->isWidget) {
			Widget* w = static_cast<Widget*>(control);
			w->set_engine(engine);
			engine->recursiveProcessWidgetControls(w, true);
		} else {
			engine->map_id_control[control->id] = control;
		}
	}
	
	addControlToCache(control);
}

bool Widget::isSelected() {
	if(engine)
		return engine->last_selected_widget == this;
	else
		return false;
}

bool Widget::inSelectedBranch() {
	return selected_control != 0 or isSelected();
}

void Widget::LockWidget(bool lock) {
	if(!engine) {
		cout << "\n[GUI] widget cannot be locked if not bound to an engine\n";
		return;
	}
	if(lock)
		engine->LockWidget(this);
	else
		sendGuiCommand( GUI_WIDGET_UNLOCK );
}

void Widget::RemoveControl( Control* control ) {
	if(!engine)
		removeControlFromCache(control);
}

void Widget::setRect( int x, int y, int w, int h ) {
	if(inSelectedBranch()) {
		engine->sel_widget_offset.x += x-m_rect.x;
		engine->sel_widget_offset.y += y-m_rect.y;
	}
}

void Widget::SetOffset(int x, int y) {
	if(inSelectedBranch()) {
		engine->sel_widget_offset.x += x-offset.x;
		engine->sel_widget_offset.y += y-offset.y;
	}
	offset.x = x;
	offset.y = y;
}

#ifdef USE_SFML
void Widget::Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected) {
	RenderWidget(ren,state,isSelected);
}
void Widget::RenderWidget( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected) {
	
	state.transform.translate( m_rect.x + offset.x, m_rect.y + offset.y );

	for(auto &ca : cache) {
		if(ca.visible) {
			auto &c = ca.control;
			#ifdef SELECTED_CONTROL_ON_TOP
				if(c != selected_control) {
					c->Render(ren, state, false);
				}
			#else
				c->Render(ren, state, c == selected_control);
			#endif
		}
	}
	
	#ifdef SELECTED_CONTROL_ON_TOP
	if(selected_control) {
		selected_control->Render(ren,state,true);
	}
	#endif
}
#elif USE_SDL
void Widget::Render( Point position, bool isSelected ) {
	RenderWidget(position,isSelected);
}
void Widget::RenderWidget( Point position, bool isSelected ) {
	
	position = position.Offset(m_rect).Offset(offset);
	
	for(auto &ca : cache) {
		if(ca.visible) {
			auto &c = ca.control;
			#ifdef SELECTED_CONTROL_ON_TOP
				if(c != selected_control) {
					c->Render( position, false );
				}
			#else
				c->Render( position, c == selected_control);
			#endif
		}
	}
	
	#ifdef SELECTED_CONTROL_ON_TOP
	if(selected_control && selected_control->visible) {
		selected_control->Render(position,true);
	}
	#endif
}
#endif
}
