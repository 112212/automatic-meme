#ifndef NG_CONTROL_HPP
#define NG_CONTROL_HPP

#ifdef USE_SFML
	#include <SFML/Graphics.hpp>
	#include <SFML/Window/Event.hpp>
#elif USE_SDL
	#include <SDL2/SDL.h>
#endif

// for std::function
#include <functional>

#include "common/Colors.hpp"
#include "common/Fonts.hpp"
#include "common/common.hpp"

#define SELECTION_MARK

#include <vector>
#include <ostream>

namespace ng {


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

enum event {
	click,
	hover,
	enter,
	change,
	submit,
	drag,
	max_default_events
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
struct Event;

class GuiEngine;

class Control {
	private:
		bool isWidget;
		const char* type;
		GuiEngine* engine;
		Widget* widget;
		friend class Widget;
		friend class GuiEngine;
		friend class ControlManager;
		bool visible;
		bool interactible;
		std::string id;
		std::vector< std::pair<event, std::function<void(Control*)> > > subscribers;
		Anchor anchor;
		Rect m_rect;
		
		enum CacheUpdateFlag {
			all = 0,
			position = 1,
			attributes = 2
		};
		
		// compiler screams ambiguous for this, so had to add _
		void update_cache(CacheUpdateFlag flag);
	protected:
		int z_index;
		
		struct ControlStyle {			
			int border_color;
			int hoverborder_color;
			int background_color;
			std::string tooltip;
			
			TTF_Font* font;
			
			int image_tex;
			Size image_size;
			bool image_repeat;
			
			bool draggable;
			float alpha;
		} m_style;
		
		// TODO: to be removed
		static bool custom_check;
		
		// functions used by controls
		void setType(const char* type);
		void emitEvent( int EventID );
		void sendGuiCommand( int eventId );
		bool isSelected();
		bool isActive();
		
		void tabToNextControl();
		void copyStyle(Control* copy_to);
		
		inline Widget* getWidget() { return widget; }
		inline GuiEngine* getEngine() { return engine; }
		
		void setInteractible(bool interactible);
		
		
		const std::vector<Control*>& getParentControls();
		const std::vector<Control*>& getWidgetControls();
		const std::vector<Control*>& getEngineControls();
		
		// internal virtual (use from within control)
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
		
		bool check_collision(int x, int y);
		
#ifdef USE_SDL
		void SetFont( TTF_Font* fnt ) { if(fnt) m_style.font = fnt; }
		void SetFont( std::string name, int size );
		TTF_Font* GetFont() { return m_style.font; }
#endif
		

		inline bool IsWidget() { return isWidget; }
		void SetStyle(std::string style, std::string value);
		void SetVisible(bool visible);
		void SetRenderable(bool visible);
		void SetAlpha(float alpha);
		void SetId( std::string id );
		void SetZIndex( int zindex );
		void SetAnchor( float W, float w, float x, float H, float h, float y );
		void SetAnchor( Rect r );
		void SetAnchor( const Anchor& anchor );
		void SetRect( int x, int y, int w, int h );
		void SetPosition( int x, int y ) { SetRect(x, y, m_rect.w, m_rect.h); }
		void SetRect( Rect r );
		void SetTooltip(std::string tooltip);
		void SetDraggable( bool draggable );
		void SetImage(std::string image, bool repeat = false);
		
		const char* 	GetType() { return type; }
		std::string 	GetId() { return this->id; }
		int 			GetZIndex() { return z_index; }
		const Anchor& 	GetAnchor();
		std::string 	GetTooltip() { return m_style.tooltip; }
		const Rect& 	GetRect() { return m_rect; }
		const Point 	GetOffset();
		const Point 	GetGlobalPosition();
		
		void Focus();
		void Activate();
		void Unselect();
		
		bool IsVisible() { return visible; }
		bool IsSelected();
		bool IsDraggable() { return m_style.draggable; }
		
		void SubscribeEvent( event event_type, std::function<void(Control*)> callback );
		void OnEvent( event event_type, std::function<void(Control*)> callback );
};
}
#endif
