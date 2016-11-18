#ifndef NG_CONTROL_HPP
#define NG_CONTROL_HPP

#ifdef USE_SFML
	#include <SFML/Graphics.hpp>
	#include <SFML/Window/Event.hpp>
#elif USE_SDL
	#include <SDL2/SDL.h>
#endif

#include <functional>
#include "common/Colors.hpp"
#include "common/Fonts.hpp"
#include "common/common.hpp"

#define SELECTION_MARK

#include <vector>
#include <ostream>

namespace ng {
struct Point {
	union{
		int x,min;
	};
	union {
		int y,max;
	};
	Point() {}
	Point(int _x, int _y) : x(_x),y(_y) {}
	bool operator< (const Point& b) const { return x < b.x || (x == b.x && y < b.y); }
	Point Offset(const Point& r) const { return Point(x+r.x, y+r.y); }
};

struct Rect : Point {
	int w;
	int h;
	Rect() {}
	Rect(int _x, int _y, int _w, int _h) : Point(_x,_y),w(_w),h(_h) {}
	
};

struct Size {
	int w,h;
};


struct Anchor {
	Point coord;
	float x,y;
	float W,w,H,h;
	float sW,sH;
	float sx,sy;
	bool isrelative;
	int ax,ay;
	Anchor& operator+=(const Anchor& b);
	friend std::ostream& operator<< (std::ostream& stream, const Anchor& anchor);
	static Anchor parseRect(std::string s);
};

enum controlType {
	TYPE_CONTROL = 0,
	TYPE_WIDGET,
	TYPE_SCROLLBAR,
	TYPE_TRACKBAR,
	TYPE_BUTTON,
	TYPE_RADIOBUTTON,
	TYPE_CHECKBOX,
	TYPE_LABEL,
	TYPE_CANVAS,
	TYPE_TEXTBOX,
	TYPE_COMBOBOX,
	TYPE_LISTBOX,
	TYPE_GRID_CONTAINER,
	TYPE_DIALOG,
	MAX_CONTROL_TYPES
};

enum {
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
struct Event;

class GuiEngine;

class Control {
	private:
		bool isWidget;
		controlType type;
		GuiEngine* engine;
		Widget* widget;
		friend class Widget;
		friend class GuiEngine;
		friend class ControlManager;
		bool visible;
		bool interactible;
		std::string id;
		std::vector< std::vector< std::function<void(Control*)> > > subscribers;
		Anchor anchor;
		Rect m_rect;
		
		enum CacheUpdateFlag {
			all = 0,
			position = 1,
			attributes = 2
		};
		// compiler screams ambiguous for this, so had to add _
		void _updateCache(CacheUpdateFlag flag);
	protected:
		char minor_type;
		int z_index;
		
		// image
		int m_image_tex;
		bool m_image_repeat;
		Size m_image_size;
		
		float m_alpha;
		
		struct {			
			int border_color;
			int hoverborder_color;
			int background_color;
			TTF_Font* font;
		} m_style;
		
		static bool custom_check;
		
		// functions used by controls
		void setType(controlType type);
		void initEventVector(int max_events) { subscribers.resize(max_events); }
		void emitEvent( int EventID );
		void sendGuiCommand( int eventId );
		bool isSelected();
		bool isActive();
		
		
		void tabToNextControl();
		void copyStyle(Control* copy_to);
		
		inline Widget* getWidget() { return widget; }
		inline GuiEngine* getEngine() { return engine; }
		bool check_collision(int x, int y);
		void setInteractible(bool interactible);
		const Point getOffset();
		
		const std::vector<Control*>& getParentControls();
		const std::vector<Control*>& getWidgetControls();
		const std::vector<Control*>& getEngineControls();
		
		// internal virtual
		virtual void onPositionChange();
		virtual void onFontChange();
		virtual bool customBoundary( int x, int y );
		
		// called by gui engine, controls can override these functions
		#ifdef USE_SFML
			virtual void Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected );
			virtual void OnKeyDown( sf::Event::KeyEvent &sym );
			virtual void OnKeyUp( sf::Event::KeyEvent &sym );
		#elif USE_SDL
			virtual void Render( Point position, bool isSelected );
			virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
			virtual void OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod );
		#endif
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnMouseDown( int mX, int mY );
		virtual void OnMouseUp( int mX, int mY );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnLostControl();
		virtual void OnMWheel( int updown );
		virtual void STYLE_FUNC(value);
		
	public:
		Control();
		~Control();
		virtual Control* Clone();
		void SetRect( Rect r );
		const Rect& GetRect( ) { return m_rect; } 
		void SetRect( int x, int y, int w, int h );
		void SetPosition( int x, int y ) { SetRect(x, y, m_rect.w, m_rect.h); }
		
		void SetAnchor( float W, float w, float x, float H, float h, float y );
		void SetAnchor( Rect r );
		void SetAnchor( const Anchor& anchor );
		void Focus();
		void Activate();
#ifdef USE_SDL
		void SetFont( TTF_Font* fnt ) { if(fnt) m_style.font = fnt; }
		void SetFont( std::string name, int size );
		TTF_Font* GetFont() { return m_style.font; }
#endif
		const Anchor& GetAnchor();
		
		controlType GetType() { return type; }
		
		bool IsWidget() { return isWidget; }
		void SetStyle(std::string style, std::string value);
		void SetVisible(bool visible);
		void SetRenderable(bool visible);
		void Unselect();
		bool IsSelected();
		void SetAlpha(float alpha);
		
		void SetImage(std::string image, bool repeat = false);
		
		bool IsVisible() { return visible; }
		void SetId( std::string id );
		std::string GetId() { return this->id; }
		void SetZIndex( int zindex );
		int GetZIndex() { return z_index; }
		void SubscribeEvent( int event_type, std::function<void(Control*)> callback );
};
}
#endif
