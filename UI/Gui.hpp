#ifndef _H_GUI_
#define _H_GUI_

#include <vector>
#include <queue>

// #include "Control.hpp"
#include "common/ControlManager.hpp"

#ifdef USE_SFML
	#include <SFML/Window.hpp>
	#include <SFML/Graphics.hpp>
	#include <SFML/OpenGL.hpp>
#endif

#ifdef USE_SDL
	#include <SDL2/SDL_opengl.h>
#endif

#include "common/Cursor.hpp"

// only for tooltip
#include "controls/Label.hpp"

// ---- GUI configuration ---
#define SELECTED_CONTROL_ON_TOP
#define OVERLAPPING_CHECK
#define USE_EVENT_QUEUE
// --------------------------


namespace ng {
#ifdef USE_EVENT_QUEUE
struct Event {
	std::string id;
	int event_type;
	Control* control;
};
#endif

class GuiEngine : public ControlManager
#ifdef USE_SFML
	, public sf::Drawable 
#endif
{
	private:
		std::map<std::string, Control*> map_id_control;
		
		// -- selection info --
		Control* selected_control;
		Widget* sel_first_depth_widget; // first widget, or widget lock
		Point sel_widget_offset; // for widget lock
		Widget* last_selected_widget;
		unsigned int sel_intercept;
		bool hasIntercepted;
		struct interceptInfo {
			unsigned int intercept_mask;
			Widget* widget;
		};
		std::vector<interceptInfo> sel_intercept_vector;
		Control* active_control;
		// ------------------
		
		Cursor cursor;
		
		// -- dragging info --
		Point drag_start_diff;
		Point drag_offset;
		bool dragging;
		// --------------------
		
		// -- tooltip --
		Label* m_tooltip;
		bool m_tooltip_shown;
		double m_tooltip_delay;
		double m_last_cursor_update_time;
		// -------------
		
		bool m_mouse_down;
		bool m_keyboard_lock;
		bool m_widget_lock;
		bool m_focus;
		bool m_focus_lock;
		bool m_lock_once;
		
		double m_time;
		double m_delta_time;
		
		
		void check_for_new_collision( int mX, int mY );
		int depth;
		bool check_control_collision( Control* control, int mX, int mY );
		
		#ifdef USE_EVENT_QUEUE
			std::queue<Event> m_events;
		#endif
		
		double getTime() { return m_time; }
		double getDeltaTime() { return m_delta_time; }
		void processControlEvent(int event_type);
		void unselectControl();
		void unselectWidgets();
		void unselectWidget();
		void recursiveProcessWidgetControls(Widget* wgt, bool add_or_remove);
		
		friend class Widget;
		friend class Control;
		
	public:
		GuiEngine();
		GuiEngine(int xsize, int ysize);
		~GuiEngine();
		GuiEngine& operator=(GuiEngine const& o) = default;
		GuiEngine& operator=(GuiEngine && o);
		
		#ifdef USE_SFML
			void draw(sf::RenderTarget& target, sf::RenderStates states) const;
			void OnKeyDown( sf::Event::KeyEvent &sym );
			void OnKeyUp( sf::Event::KeyEvent &sym );
			void OnEvent(sf::Event &event);
		#elif USE_SDL
			void Render();
			void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
			void OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod );
			void OnEvent(SDL_Event &event);
		#endif
		
		void UnselectControl() { unselectControl(); }
		void AddControl( Control* cntrl );
		void RemoveControl( Control* control );
		void Clear();
		
		void LockWidget(Widget* w);
		void UnlockWidget();
		
		
		void SetDefaultFont(std::string font, int size=13);
		void SetSize(int w, int h);
		void SetTooltipDelay(double seconds);
		void SetRelativeMode(bool relative_mode);
		
		void Focus(Control* control);
		void Activate(Control* control);
		void ShowTooltip(Control* control);
		void HideTooltip();
		
		Control* GetSelectedControl() { return selected_control; }
		Widget* GetSelectedWidget() { return last_selected_widget; }
		Control* GetActiveControl() { return active_control; }
		Control* GetControlById(std::string id);
		
		#ifdef USE_EVENT_QUEUE
			bool HasEvents( );
			Event PopEvent();
		#endif
		
		void SubscribeEvent( std::string id, event event_type, std::function<void(Control*)> callback );
		void OnEvent( std::string id, event event_type, std::function<void(Control*)> callback );
		
		// events
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnMouseMove( int mX, int mY );
		void OnMWheel( int updown );
};

}
#endif
