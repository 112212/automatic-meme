#ifndef _H_GUI_
#define _H_GUI_

#include <vector>
#include <queue>

#include "Control.hpp"
// #include "Widget.hpp"
#include "common/cache.hpp"

#ifdef USE_SFML
	#include <SFML/Window.hpp>
	#include <SFML/Graphics.hpp>
#endif

// ---- GUI configuration ---
// #define SELECTED_CONTROL_ON_TOP
#define OVERLAPPING_CHECK
// #define USE_EVENT_QUEUE
// --------------------------

#include <utility>


namespace ng {
#ifdef USE_EVENT_QUEUE
struct Event {
	unsigned int id;
	int event_type;
	Control* control;
};
#endif

class GuiEngine : public Cache
#ifdef USE_SFML
	, public sf::Drawable 
#endif
{
	private:
		static bool firstInit;
		std::map<std::string, Control*> map_name_control;
		std::map<unsigned int, Control*> map_id_control;
		
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
		// --------------------
		
		bool m_mouse_down;
		
		bool m_keyboard_lock;
		bool m_widget_lock;
		bool m_focus;
		bool m_focus_lock;
		bool m_lock_once;
		
		void check_for_new_collision( int mX, int mY );
		int depth;
		bool checkngControl_collision( Control* control, int mX, int mY );
		
		#ifdef USE_EVENT_QUEUE
			std::queue<Event> m_events;
		#endif
		
		void processControlEvent(int event_type);
		void unselectControl();
		void unselectWidgets();
		void unselectWidget();
		void recursiveProcessWidgetControls(Widget* wgt, bool add_or_remove);
		
		friend class Widget;
		friend class Control;
		
	public:
		GuiEngine();
	
		#ifdef USE_SFML
			void draw(sf::RenderTarget& target, sf::RenderStates states) const;
			void OnKeyDown( sf::Event::KeyEvent &sym );
			void OnKeyUp(  sf::Event::KeyEvent &sym );
			void OnEvent(sf::Event &event);
		#elif USE_SDL
			void Render( SDL_Renderer* ren );
			void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
			void OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod );
			void OnEvent(SDL_Event &event);
		#endif
		
		
		
		void AddControl( Control* cntrl );
		void RemoveControl( Control* control );
		
		void LockWidget(Widget* w);
		void UnlockWidget();
		Control* GetControlByName(std::string name);
		Control* GetControlById(unsigned int id);
		
		Control* GetSelectedControl() { return selected_control; }
		Widget* GetSelectedWidget() { return last_selected_widget; }
		
		#ifdef USE_EVENT_QUEUE
			bool HasEvents( );
			Event PopEvent();
		#endif
		void SubscribeEvent( int id, int event_type, std::function<void(Control*)> callback );
		
		// events
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void OnMouseMove( int mX, int mY );
		void OnMWheel( int updown );
		
		// cleanup
		void OnCleanup();
};

}
#endif
