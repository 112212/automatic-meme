#include "common/cache.hpp"
#include "Control.hpp"

// za interface eventa sa GUI einom
#include "Gui.hpp"
#include "Widget.hpp"

namespace ng {
Control::Control() : id("0"), engine(0), widget(0), z_index(0), type(TYPE_CONTROL), isWidget(false),
interactible(true), visible(true) {
	m_rect.x = m_rect.y = m_rect.w = m_rect.h = 0;
}

Control::~Control() {
}

const Point Control::getOffset() {
	if(engine)
		return engine->sel_widget_offset;
	else
		return {0,0};
}

void Control::SetZIndex( int zindex ) {
	if(zindex == z_index) return;
	if(widget) {
		widget->setZIndex(this, zindex);
	} else if(engine) {
		engine->setZIndex(this, zindex);
	}
}

void Control::_updateCache(CacheUpdateFlag flag) {
	if(widget) {
		widget->updateCache(this,flag);
	} else if(engine) {
		engine->updateCache(this,flag);
	}
}

void Control::SetVisible(bool visible) {
	if(this->visible != visible) {
		this->visible = visible;
		_updateCache(CacheUpdateFlag::attributes);
	}
}

void Control::setInteractible(bool interactible) {
	if(this->interactible != interactible) {
		this->interactible = interactible;
		
		_updateCache(CacheUpdateFlag::attributes);
	}
}

void Control::SetId(std::string id) {
	this->id = id;
}


void Control::setType( controlType type ) {
	this->type = type;
	_updateCache(CacheUpdateFlag::all);
}

// emit global event
void Control::emitEvent( int EventID ) {
	#ifdef USE_EVENT_QUEUE
	if(engine) {
		engine->m_events.push( { id, EventID, this } );
	}
	#endif
	
	if(EventID < subscribers.size()) {
		auto &lst = subscribers[EventID];
		for(auto i = lst.begin(); i != lst.end(); i++) {
			(*i)(this);
		}
	}
}

void Control::SubscribeEvent( int event_type, std::function<void(Control*)> callback ) {
	if(event_type < subscribers.size())
		subscribers[event_type].push_back(callback);
}

void Control::sendGuiCommand( int eventId ) {
	if(engine) {
		engine->processControlEvent(eventId);
	}
}

const std::vector<Control*> Control::getWidgetControls() {
	if(widget)
		return widget->controls;
	else
		return std::vector<Control*>();
}
const std::vector<Control*> Control::getEngineControls() {
	if(engine)
		return engine->GetControls();
	else
		return std::vector<Control*>();
}

bool Control::check_collision(int x, int y) {
	Rect& r = m_rect;
	return (x >= r.x && x <= r.x+r.w && y >= r.y && y <= r.y+r.h);
}

void Control::SetRect( int x, int y, int w, int h ) {
	int old_x = m_rect.x;
	int old_y = m_rect.y;
	
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

void Control::SetRect( Rect r ) { 
	SetRect(r.x, r.y, r.w, r.h);
}

// make these virtuals optional
void Control::OnMouseMove( int mX, int mY, bool mouseState ) {}
void Control::OnMouseDown( int mX, int mY ) {}
void Control::OnMouseUp( int mX, int mY ) {}
#ifdef USE_SFML
	void Control::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {}
	void Control::OnKeyDown( sf::Event::KeyEvent &sym ) {}
	void Control::OnKeyUp( sf::Event::KeyEvent &sym ) {}
#elif USE_SDL
	void Control::Render( SDL_Rect position, bool isSelected ) {}
	void Control::OnKeyDown( SDL_Keycode &sym, SDL_Keymod &mod ) {}
	void Control::OnKeyUp(  SDL_Keycode &sym, SDL_Keymod &mod ) {}
#endif
void Control::OnLostFocus() {}
void Control::onPositionChange() {}

void Control::OnGetFocus() {}
void Control::OnLostControl() {}
void Control::OnMWheel( int updown ) {}
bool Control::customBoundary( int x, int y ) {}


void Control::SetStyle(std::string& style, std::string& value) {
	const Rect& r = GetRect();
	
	STYLE_SWITCH {
		_case("x"):
			SetRect(std::stoi(value), r.y, r.w, r.h);
		_case("y"):
			SetRect(r.x, std::stoi(value), r.w, r.h);
		_case("w"):
			SetRect(r.x, r.y, std::stoi(value), r.h);
		_case("h"):
			SetRect(r.x, r.y, r.w, std::stoi(value));
		_case("id"):
			SetId(value);
		_case("rect"): {
				int c[4];
				int num=0;
				std::string::size_type tokenOff = 0, sepOff = 0;
				while (sepOff != std::string::npos)
				{
					sepOff = value.find(',', sepOff);
					std::string::size_type tokenLen = (sepOff == std::string::npos) ? sepOff : sepOff++ - tokenOff;
					std::string token = value.substr(tokenOff, tokenLen);
					if (!token.empty()) {
						// cout << "test: " << token << endl;
						c[num++] = std::stoi(token);
					}
					tokenOff = sepOff;
				}
				if(num == 4)
					SetRect(c[0],c[1],c[2],c[3]);
			}
		_case("visible"):
			SetVisible(value=="true");
		default:
			OnSetStyle(style, value);
	}

}

void Control::STYLE_FUNC(value) {}
}
