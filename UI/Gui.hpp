#ifndef _GUI_HPP_
#define _GUI_HPP_

#include <vector>
#include <queue>

#include "managers/ControlManager.hpp"

#include "Cursor.hpp"
#include "Control.hpp"
#include "backend/Backend.hpp"
#include "backend/Input.hpp"

#define NO_SELECTED_CONTROL_ON_TOP

// ---- GUI configuration ---
// make selected control be rendered last (over everything, that is make it TOP MOST)
#ifndef NO_SELECTED_CONTROL_ON_TOP
	#define SELECTED_CONTROL_ON_TOP
#endif

// overlapping
// #define FORCE_ALWAYS_OVERLAPPING_CHECK
// #define NO_OVERLAPPING_CHECK

#ifndef NO_OVERLAPPING_CHECK
	#define OVERLAPPING_CHECK
	// #ifdef FORCE_ALWAYS_OVERLAPPING_CHECK
		// #undef FORCE_ALWAYS_OVERLAPPING_CHECK
		// #define FORCE_ALWAYS_OVERLAPPING_CHECK true
	// #else
		// #define FORCE_ALWAYS_OVERLAPPING_CHECK false
	// #endif
#endif
// #define USE_EVENT_QUEUE
// --------------------------



namespace ng {
	
	
#define GUIFUNC(name) \
			void _guifunc_##name (Args& args); \
			bool _guifunc_bool_##name = Gui::AddFunction( #name, (_guifunc_##name) ); \
			void _guifunc_##name  (Args& args)

class Label;

extern Backend default_backend;

class Gui : public ControlManager
{
	private:
		Size resolution;
		std::map<std::string, Control*> map_id_control;
		
		// -- selection info --
		Control* selected_control;
		Control* last_selected_control;
		Widget* sel_first_depth_widget; // first widget, or widget lock
		Point sel_widget_offset; // for widget lock
		Widget* last_selected_widget;
		Backend backend;
		unsigned int sel_intercept;
		int selection_margin;
		Size selection_margin_control_min_size;
		bool hasIntercepted;
		struct interceptInfo {
			unsigned int intercept_mask;
			Widget* widget;
		};
		std::vector<interceptInfo> sel_intercept_vector;
		Control* active_control;
		// ------------------
		
		// -- fps measurement --
		double m_delta_accum;
		uint32_t m_frames;
		uint32_t m_fps;
		std::function<void(uint32_t)> m_on_fps_change;
		// ---------------------
		
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
		
		
		void check_for_new_collision( int mX, int mY, bool start_from_top=false );
		int depth;
		
		#ifdef USE_EVENT_QUEUE
			std::queue<Event> m_events;
		#endif
		
		bool check_for_drag();
		inline double getTime() { return m_time; }
		inline double getDeltaTime() { return m_delta_time; }
		void processControlEvent(int event_type);
		void unselectControl();
		
		void unselectWidgets();
		void unselectWidget();
		void recursiveProcessWidgetControls(Widget* wgt, bool add_or_remove);

		static std::map<std::string, EventCallback> function_map;
		
		friend class Widget;
		friend class Control;
		friend class ControlManager;
		
		static void play_sound( Args& );
		
	public:
		Gui();
		Gui(int xsize, int ysize);
		~Gui();
		Gui& operator=(Gui const& o) = default;
		Gui& operator=(Gui && o);
		
		void Render();
		
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
		void OnFpsChange(std::function<void(uint32_t)> f) { m_on_fps_change = f; }
		
		void Focus(Control* control);
		void Activate(Control* control);
		void ShowTooltip(Control* control);
		void HideTooltip();
		void HideOSCursor();
		
		Control* GetSelectedControl() { return selected_control; }
		Widget* GetSelectedWidget() { return last_selected_widget; }
		Control* GetActiveControl() { return active_control; }
		Control* GetControlById(std::string id);
		Size GetSize();
		uint32_t GetFps();
		
		#ifdef USE_EVENT_QUEUE
			bool HasEvents( );
			Event PopEvent();
		#endif
		
		static bool AddFunction( std::string function_name, EventCallback callback );
		void OnEvent( std::string id, std::string event_type, EventCallback callback );
		
		// keyboard events
		void OnKeyDown( Keycode sym, Keymod mod );
		void OnKeyUp( Keycode sym, Keymod mod );
		void OnText( std::string text );
		
		// mouse events
		void OnMouseDown( unsigned int button );
		void OnMouseUp( unsigned int button );
		void OnMouseMove(int mX, int mY);
		void OnMWheel(int updown);
		
		void SetBackend(Backend backend);
		const Backend& GetBackend();
		
		Cursor& GetCursor();
};

}
#endif
