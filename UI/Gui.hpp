#ifndef NG_GUI_HPP
#define NG_GUI_HPP

#include <vector>
#include <queue>
#include <mutex>

#include "managers/ControlManager.hpp"

#include "Cursor.hpp"
#include "backend/Backend.hpp"
#include "backend/Input.hpp"
#include "Control.hpp"

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

typedef std::function<void(Keycode,Keymod)>  KeyFunc;

class Gui
{
	private:
		Size resolution;
		std::map<std::string, Control*> map_id_control;
		Backend backend;
		int selection_margin;
		std::vector<Control*> invalidates;
		
		std::mutex* mutex;
		
		// -- selection info --
		Control* sel_control;
		Control* last_sel_control;
		Control* active_control;
		
		Point sel_control_parent_window_position; // for widget lock
		Control* sel_parent;
		
		unsigned int sel_intercept;
		struct interceptInfo {
			unsigned int intercept_mask;
			Control* parent_control;
			Control* last_control; // for hover
		};
		bool hasIntercepted;
		std::vector<interceptInfo> sel_intercept_vector;
		// void intercept_hook(Control::imask onaction, std::function<void()> action, std::function<void()> intercept_action={}, std::function<void()> no_intercept_control_action={});
		// ------------------
		int locked;
		
		Control rootWidget;
		
		// -- fps measurement --
		double m_delta_accum;
		uint32_t m_frames;
		uint32_t m_fps;
		std::function<void(uint32_t)> m_on_fps_change;
		std::function<void()> m_on_render;
		std::queue<std::function<void()>> m_on_render_ops;
		// ---------------------
		
		Cursor cursor;
		
		// -- dragging info --
		Point drag_start_diff;
		Point drag_offset;
		Point drag_snap;
		bool snapped;
		bool dragging;
		// --------------------
		
		// -- tooltip --
		Label* m_tooltip;
		bool m_tooltip_shown;
		double m_tooltip_delay;
		double m_last_cursor_update_time;
		// -------------
		
		bool m_mouse_down;
		
		// ---- selection locks ----
		bool m_keyboard_lock;
		bool m_focus;
		bool m_focus_lock;
		bool m_lock_once;
		bool m_block_all_events;
		bool m_unblock_all_events_on_mouse_up;
		Control* m_lock_target;
		// -------------------------
		
		double m_time;
		double m_delta_time;
		
		
		void check_for_new_collision( Point pt, bool start_from_top=false );
		int depth;
		
		std::vector<KeyFunc> m_keyup_cb;
		std::vector<KeyFunc> m_keydown_cb;
		
		#ifdef USE_EVENT_QUEUE
			std::queue<Event> m_events;
		#endif
		
		bool check_for_lock();
		bool check_for_drag();
		inline double getTime() { return m_time; }
		inline double getDeltaTime() { return m_delta_time; }
		void processControlEvent(int event_type, Control* target);
		void unselectControl();
		void unselectControls();

		static std::map<std::string, EventCallback> function_map;
		
		friend class Control;
		friend class ControlManager;
		
		static void cmd_play_sound( Args& );
		void cmd_style( Args& );
		Control* get(const std::string& id);
		
		void renderInvalidate(Control* c);
		
	public:
		Gui();
		Gui(int xsize, int ysize);
		~Gui();
		Gui& operator=(Gui const& o) = default;
		Gui& operator=(Gui && o);
		
		void Render();
		
		void UnselectControl() { unselectControl(); }
		void AddControl( Control* cntrl, bool processlayout=true );
		void RemoveControl( Control* control );
		void Clear();
		
		void LockWidget(Control* w);
		void UnlockWidget();
		
		bool IsDragging();
		void DragSnapTo(ng::Point pt, bool enable=true);
		void SetStyle(std::string control, std::string style, std::string value);
		void SetDefaultFont(std::string font, int size=13);
		void SetSize(int w, int h);
		void SetTooltipDelay(double seconds);
		void SetRelativeMode(bool relative_mode);
		void OnFpsChange(std::function<void(uint32_t)> f) { m_on_fps_change = f; }
		void OnRender(std::function<void()> f) { m_on_render = f; }
		void DoOnRender(std::function<void()> f) { m_on_render_ops.push(f); }
		
		void Focus(Control* control);
		void Activate(Control* control);
		void ShowTooltip(Control* control);
		void HideTooltip();
		void HideOSCursor();
		void MtLock();
		void MtUnlock();
		
		Control* GetSelectedControl();
		Control* GetSelectedWidget();
		Control* GetActiveControl();
		bool IsKeyboardLocked();
		
		Size GetSize();
		uint32_t GetFps();
		
		#ifdef USE_EVENT_QUEUE
			bool HasEvents( );
			Event PopEvent();
		#endif
		
		static bool AddFunction( std::string function_name, EventCallback callback );
		
		template <typename T>
		static bool AddFunction( std::string function_name, void (T::*callback)(Args& args), T* _this ) {
			return AddFunction(function_name, std::bind(callback, _this, std::placeholders::_1));
		}
		
		void OnEvent( std::string id, std::string event_type, EventCallback callback );
		
		template <typename T>
		bool OnEvent( std::string id, std::string event_type, void (T::*callback)(Args& args), T* _this ) {
			OnEvent(id, event_type, std::bind(callback, _this, std::placeholders::_1));
			return true;
		}
		
		void CallFunc( std::string function_name, const Argv& a={} );
		
		// keyboard events
		void OnKeyDown( Keycode sym, Keymod mod );
		void OnKeyUp( Keycode sym, Keymod mod );
		void OnText( std::string text );
		
		void OnKeyDown( KeyFunc f);
		void OnKeyUp( KeyFunc f);
		
		// mouse events
		void OnMouseDown( unsigned int button );
		void OnMouseUp( unsigned int button );
		
		void OnMouseMove(int mX, int mY);
		void OnMWheel(int updown);
		
		void 			SetBackend(Backend backend);
		const Backend&  GetBackend();
		
		Point GetCursorPosition();
		Cursor& GetCursor();
		
		// forward to root widget
		void LoadXml(std::string xml_filename);
		void LoadXml(std::istream& stream);
		void RemoveControls();
		void BreakRow();
		
		template <typename T>
		T* CreateControl(std::string type, std::string id="") {
			return (T*)CreateControl(type,id);
		}
		Control* CreateControl(std::string type, std::string id="");
		const std::vector<Control*>& GetControls();
		
		void EnableStyleGroup(std::string name, bool enable=true);
		void DisableAllStyles();
		void ForEachControl(std::function<void(Control* c)> func);
		void ProcessLayout(bool asRoot=false);
		
		template<typename C=Control>
		C* Get(const std::string id) {
			return static_cast<C*>(get(id));
		}
		
		Control* DbgGet(const char* id);
};

}
#endif
