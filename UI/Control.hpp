#ifndef NG_CONTROL_HPP
#define NG_CONTROL_HPP

#include "managers/ControlManager.hpp"

#include <functional>

#include "Color.hpp"
#include "managers/Fonts.hpp"
#include "Font.hpp"
#include "common.hpp"
#include "Layout.hpp"
#include "Cursor.hpp"

#define SELECTION_MARK

#include <vector>
#include <ostream>
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
#define _casef(a) case hash(a)
//

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
	GUI_UNLOCK_BY_MOUSEUP,
	GUI_MAX_EVENTS
};

class Gui;
class Control;
class Border;
class Effect;

// ----------- Event --------------

// Event arguments
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

// --------------------------

class Control : public ControlManager  {
	private:
		friend class Gui;
		friend class ControlManager;
		friend class Effect;
		
		// ----- <ControlBase> -----
		
		struct Event {
			Event(std::string& event_id, std::string function_name) {
				this->event_id = event_id;
				this->function_name = function_name;
			}
			int tag;
			std::string event_id;
			std::vector<std::string> args;
			
			Control* control;
			std::string function_name;
		};
		const char* type;
		Gui* engine;
		Control* parent;
		
		bool visible;
		int interactible;
		bool render_enabled;
		bool nostyling;
		Layout layout;
		Rect m_rect;
		Border* m_border;
		
		std::string id;
		std::vector<Effect*> effects;
		
		// vector of pair (event_type, function)
		std::vector<Event> subscribers;
		
		void update_cache(CacheUpdateFlag flag);
		void process_prerender_effects();
		void process_postrender_effects();
		void render(Point position, bool isSelected);
		// ----- </ControlBase> ------
		
		bool m_is_intercepted;
		unsigned int intercept_mask;
		Control* sel_control;
		Point cached_absolute_offset;
		Size min, max;
		
		// offset for easy scrolling controls (it adds to everys child control position)
		Point m_offset;
		
		static int current_tag;
		
		void backtrackStylingCreationPairs();
		
		void set_engine(Gui* engine);
	protected:
		// ------- [ControlBase] --------
		int z_index;
		bool use_clipping;
		
		struct ControlStyle {
			uint32_t border_color;
			uint32_t hoverborder_color;
			uint32_t background_color;
			uint32_t color;
			uint32_t hovercolor;
			uint32_t hoverbackground_color;
			int border_thickness;
			std::string tooltip;
			
			Font* font;
			
			Image* hoverimg;
			Image* image_tex;
			Size image_size;
			bool image_repeat;
			
			bool draggable;
			float alpha;
			
			void SetTransparentBackground();
			void SetTransparentBorder();
		} m_style;
		
		// functions used by controls
		
		bool emitEvent( std::string event_id, const Argv& args = {} );
		bool poseEmitEvent( Control* c, std::string event_id, const Argv& args = {} );
		void sendGuiCommand( int eventId, Control* target=0 );
		bool isActive();
		void getRange(Size& min, Size& max);
		void tabToNextControl();
		void copyStyle(Control* copy_to);
		void cloneBase(Control* copy_to);
		
		inline Control* getParent() { return parent; }
		inline Gui* getEngine() { return engine; }
		double getDeltaTime();
		void shareEngineBackend(Control* c);
		void removeEngine(Control* c);
		void setInteractible(int interactible);
		Point getAbsCursor();
		Point getCursor();
		
		const std::vector<Control*>& getParentControls();
		
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
		virtual void OnClick(int x, int y, MouseButton btn);
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnActivate();
		virtual void OnLostControl();
		virtual void OnMWheel( int updown );
		virtual void OnText( std::string s );
		virtual void OnSetStyle(std::string& style, std::string& value);
		
		// ------- backend functions
		Screen& Drawing();
		Speaker& Sound();
		System& GetSystem();
		void onEvent( std::string event_type, EventCallback callback );
		
		// -------- [/ControlBase] ------------------
	
		// ----- intercept events -----
		enum imask {
			mouse_up = 0x01,
			mouse_down = 0x02,
			mouse_move = 0x04,
			mwheel = 0x08,
			key_down = 0x10,
			key_up = 0x20,
			key_text = 0x40,
		};
		void setInterceptMask(unsigned int mask);
		void intercept();
		// ----------------------------
		
		bool isSelected();
		bool inSelectedBranch();
		
		bool isIntercepted() { return m_is_intercepted; }
		unsigned int getInterceptMask() { return intercept_mask; }
		inline Control* getSelectedControl() { return sel_control; }
		const Point getAbsoluteOffset();
		virtual void parseXml(rapidxml::xml_node<char>* node);
		Point layoutProcessControls(std::vector<Control*>& controls_copy);
		Control* get(const std::string& id);
		void setCursor(CursorType type);
		
		template<typename T>
		T* createControl(std::string type, std::string id="") {
			return (T*)createControl(type, id);
		}
		
		void applyStyling(const Styling& styling);
		void applyStyling(std::vector<ControlCreationPair>& vec, std::string group="");
		bool applyStyling(std::vector<Styling>& styling, std::vector<ControlCreationPair>::iterator vec_start, std::vector<ControlCreationPair>::iterator vec_end);
		Control* createControl(std::string type, std::string id="");
		bool checkCollision(const Point& p, bool use_interactible=0);
		
	public:
		// TODO: shouldn't be public
		void setType(const char* type);
		// ----------- [ControlBase] --------
		virtual Control* Clone();
		
		bool CheckCollision(const Point& p);
		bool CheckCollisionA(const Point& p);
		
		void CopyStyle(Control* copy_to);
		
		void SendToFront();
		void SendToBack();
		void SendUpper();
		void SendLower();
		void SendAfterControl(Control* c);
		void SendBeforeControl(Control* c);
		
		void SetFont( Font* fnt );
		void SetFont( std::string name );
		void SetStyle(std::string style, std::string value);
		void SetStyle(std::string style, const char* value);
		void SetStyle(std::string style, bool value);
		
		void SetVisible(bool visible);
		void SetRenderable(bool visible);
		void SetAlpha(float alpha);
		void SetId( std::string id );
		void SetZIndex( int zindex );
		
		void SetLayout( float x, float w, float W, float y, float h, float H );
		void SetLayout( Rect r );
		void SetLayout( std::string layout );
		void SetLayout( const Layout& anchor );
		void SetLayoutEnabled(bool enabled);
		
		void SetRect( int x, int y, int w, int h );
		void SetPosition( Point pt );
		void SetPosition( int x, int y );
		void SetSize( int w, int h );
		void SetSize( Size s );
		void SetRect( Rect r );
		
		void SetTooltip(std::string tooltip);
		void SetDraggable( bool draggable );
		void SetImage(std::string image, bool repeat = false);
		void SetImage(Image *image, bool repeat = false);
		void SetBorder(Border* border);
		void SetRenderEnabled(bool enable);
		void SetTransparentBackground();
		void DisableStyling(bool tf = true);
		
		Point 				GetAbsCursor() { return getAbsCursor(); };
		Point 				GetCursor() { return getCursor(); };
		const Point 		GetAbsoluteOffset() { return getAbsoluteOffset(); };
		virtual Rect		GetContentRect();
		Rect 				GetParentRect();
		std::string 		GetType();
		std::string 		GetId();
		std::string 		GetFullId();
		int 				GetZIndex();
		Layout& 			GetLayout();
		std::string 		GetTooltip();
		inline const Rect&  GetRect() { return m_rect; }
		Border* 			GetBorder();
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
		
		inline bool IsVisible() { return visible; }
		inline bool IsDraggable() { return m_style.draggable; }
		bool IsBeingDragged();
		bool IsSelected() { return isSelected(); }
		
		template <typename T>
		void OnEvent(std::string event_type, T callback) {
			
			// TODO: implement optional function arguments passing
			// void OnEvent( std::string event_type, T callback) {
			// using P = typename function_argument_type<T,0>::type;
			// onEvent( event_type, [=](Control* c, Argv& args) {
				// callback((P)c, args);
			// });
			onEvent(event_type, callback);
		}
		
		template <typename T>
		bool OnEvent( std::string event_type, void (T::* callback)(Args& args), T* _this ) {
			OnEvent(event_type, std::bind(callback, _this, std::placeholders::_1));
			// OnEvent(event_type, [=](Args& a){ (_this->*callback)(a);});
			return true;
		}
		
		
		// -------------- [/ControlBase] --------------
	
		Control();
		~Control();

		void RenderBase( Point position, bool isSelected );
		
		virtual void AddControl( Control* control, bool processlayout=true );
		
		void RemoveControl( Control* control );
		void LockWidget(bool lock);
		void RenderWidget( Point position, bool isSelected );
		
		void 			SetOffset(int x, int y);
		const Point& 	GetOffset() { return m_offset; }
		
		void ProcessLayout(bool asRoot=false);
		
		void ForEachControl(std::function<void(Control* c)> func);
		
		std::string GetSelectedRadioButton(int group=0);
		
		void ApplyStyle(std::string style_group="");
		void ApplyStyle(const Styling& s);
		
		template<typename C=Control>
		C* Get(const std::string id) {
			return static_cast<C*>(get(id));
		}
};

}
#endif
