#ifndef NG_CONTROL_HPP
#define NG_CONTROL_HPP

#include <functional>

#include "Color.hpp"
#include "managers/Fonts.hpp"
#include "Font.hpp"
#include "common.hpp"

#define SELECTION_MARK

#include <vector>
#include <ostream>
// #include "backend/Backend.hpp"
#include "backend/Screen.hpp"
#include "backend/Speaker.hpp"
#include "backend/Input.hpp"
#include "backend/System.hpp"

#include <iostream>

namespace rapidxml {
	template<typename ch> class xml_node;
}
namespace ng {

//
#define STYLE_FUNC(value) OnSetStyle(std::string& style, std::string& value)
#define STYLE_SWITCH switch(hash(style.c_str()))
#define _case(a) break; case hash(a)
//

struct Layout {
	Point coord;
	float x,y;
	float W,w,H,h;
	
	bool absolute_coordinates;
	
	// only for widgets
	enum SizeFunction {
		none,
		keep,
		fit,
		expand
	};
	SizeFunction w_func;
	SizeFunction h_func;
	float w_min[2];
	float w_max[2];
	float h_min[2];
	float h_max[2];
	
	Layout();
	Layout( Point coord, float x, float y, float W, float w, float H, float h );
	Layout(std::string s);
	void SetCoord( Point coord );
	void SetPosition( float x, float y, float w=0, float W=0, float h=0, float H=0, bool absolute_coordinates = false );
	void SetSize( float w, float W, float h, float H );
	void SetSizeRange( float min_w, float min_W, float min_h, float min_H, float max_w, float max_W, float max_h, float max_H, SizeFunction w_func=fit, SizeFunction h_func=fit );
	Layout& operator+=(const Layout& b);
	friend std::ostream& operator<< (std::ostream& stream, const Layout& anchor);
	static Layout parseRect(std::string s);
};

// used from within controls
enum Gui_message {
	GUI_UNSELECT = 0,
	GUI_UNSELECT_WIDGET,
	GUI_UNSELECT_WIDGETS,
	GUI_UNLOCK,
	GUI_FOCUS_LOCK,
	GUI_KEYBOARD_LOCK,
	GUI_WIDGET_UNLOCK,
	GUI_LOCK_ONCE,
	GUI_MAX_EVENTS
};

class Widget;
class Control;
class Gui;
class Border;


struct Arg {
	Arg() {}
	Arg(int i) { this->i = i; }
	Arg(float f) { this->f = f; }
	Arg(double d) { this->d = d; }
	Arg(char c) { this->c = c; }
	Arg(std::string str) { s=str; }
	~Arg() { }
	union {
		int i;
		float f;
		double d;
		char c;
	};
	std::string s;
};

typedef std::vector<Arg> Argv;

struct Args {
	Control* control;
	Argv event_args;
	std::vector<std::string> cmd_args;
};

typedef std::function<void(Args& args)> EventCallback;
class Effect;
class Control {
	private:
		struct Event {
			Event(std::string& event_id, std::string function_name) {
				this->event_id = event_id;
				this->function_name = function_name;
			}
			std::string event_id;
			std::vector<std::string> args;
			
			Control* control;
			std::string function_name;
		};

		bool isWidget;
		const char* type;
		Gui* engine;
		Widget* widget;
		friend class Widget;
		friend class Gui;
		friend class ControlManager;
		friend class Effect;
		bool visible;
		bool interactible;
		bool render_enabled;
		std::string id;
		std::vector<Effect*> effects;
		
		// vector of pair (event_type, function)
		std::vector<Event> subscribers;
		Layout layout;
		Rect m_rect;
		Border* m_border;
		
		void update_cache(CacheUpdateFlag flag);
		void process_prerender_effects();
		void process_postrender_effects();
		void render(Point position, bool isSelected);
	protected:
		int z_index;
		
		struct ControlStyle {
			int border_color;
			int hoverborder_color;
			int background_color;
			int color;
			int hovercolor;
			int hoverbackground_color;
			std::string tooltip;
			
			Font* font;
			
			Image* image_tex;
			Size image_size;
			bool image_repeat;
			
			bool draggable;
			float alpha;
		} m_style;
		
		// functions used by controls
		void setType(const char* type);
		void emitEvent( std::string event_id, const Argv& args = {} );
		void poseEmitEvent( Control* c, std::string event_id, const Argv& args = {} );
		void sendGuiCommand( int eventId );
		bool isSelected();
		bool isActive();
		
		void tabToNextControl();
		void copyStyle(Control* copy_to);
		void cloneBase(Control* copy_to);
		
		inline Widget* getWidget() { return widget; }
		inline Gui* getEngine() { return engine; }
		double getDeltaTime();
		void shareEngineBackend(Control* c);
		void removeEngine(Control* c);
		void setInteractible(bool interactible);
		
		const std::vector<Control*>& getParentControls();
		const std::vector<Control*>& getWidgetControls();
		const std::vector<Control*>& getEngineControls();
		
		// internal virtual (use from within control)
		virtual void onRectChange();
		virtual void onFontChange();
		
		// called by gui engine, controls can override these functions
		virtual void Render( Point position, bool isSelected );
		virtual void OnKeyDown( Keycode sym, Keymod mod );
		virtual void OnKeyUp( Keycode sym, Keymod mod );
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnMouseDown( int mX, int mY, MouseButton which_button );
		virtual void OnMouseUp( int mX, int mY, MouseButton which_button );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnLostControl();
		virtual void OnMWheel( int updown );
		virtual void OnText( std::string s );
		virtual void STYLE_FUNC(value);
		
		// ------- backend functions
		Screen& Drawing();
		Speaker& Sound();
		System& GetSystem();
		void onEvent( std::string event_type, EventCallback callback );
		virtual void parseXml(rapidxml::xml_node<char>* node);
	public:
		Control();
		~Control();
		virtual Control* Clone();
		
		
		bool CheckCollision(int x, int y);
		
		void CopyStyle(Control* copy_to);
		
		void SetFont( Font* fnt );
		void SetFont( std::string name, int size );
		void SetStyle(std::string style, std::string value);
		void SetVisible(bool visible);
		void SetRenderable(bool visible);
		void SetAlpha(float alpha);
		void SetId( std::string id );
		void SetZIndex( int zindex );
		
		void SendToFront();
		void SendToBack();
		void SendUpper();
		void SendLower();
		void SendAfterControl(Control* c);
		void SendBeforeControl(Control* c);
		
		void SetLayout( float W, float w, float x, float H, float h, float y );
		void SetLayout( Rect r );
		void SetLayout( const Layout& anchor );
		void SetRect( int x, int y, int w, int h );
		void SetPosition( int x, int y );
		void SetSize( int w, int h );
		void SetRect( Rect r );
		void SetTooltip(std::string tooltip);
		void SetDraggable( bool draggable );
		void SetImage(std::string image, bool repeat = false);
		void SetBorder(Border* border);
		void SetRenderEnabled(bool enable);
		
		
		Rect 				GetParentWidgetRegion();
		std::string 		GetType();
		std::string 		GetId();
		int 				GetZIndex();
		Layout& 			GetLayout();
		std::string 		GetTooltip();
		inline const Rect&  GetRect() { return m_rect; }
		const Point 		GetOffset();
		const Point 		GetGlobalPosition();
		Font* 				GetFont();
		Gui* 				GetEngine();
		
		void				AddEffect(Effect* effect);
		void				RemoveEffect(Effect* effect);
		void				RemoveEffect(std::string effect_name);
		Effect*				GetEffect(std::string effect_name);
		
		void Focus();
		void Activate();
		void Unselect();
		void Unattach();
		
		inline bool IsWidget() { return isWidget; }
		inline bool IsVisible() { return visible; }
		inline bool IsDraggable() { return m_style.draggable; }
		bool IsBeingDragged();
		bool IsSelected();
		
		template <typename T>
		void OnEvent( std::string event_type, T callback) {
			
			// TODO: implement optional function arguments passing
			// void OnEvent( std::string event_type, T callback) {
			// using P = typename function_argument_type<T,0>::type;
			// onEvent( event_type, [=](Control* c, Argv& args) {
				// callback((P)c, args);
			// });
			onEvent(event_type, callback);
		}
};
}
#endif
