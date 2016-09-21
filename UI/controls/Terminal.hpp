#ifndef NG_TERMINAL_HPP
#define NG_TERMINAL_HPP

#include "../Widget.hpp"
#include "SDL/TextBox.hpp"
#include <unordered_map>
namespace ng {

class Terminal : public Widget {
	private:
		TextBox *m_log;
		TextBox *m_terminal;
		int m_history_counter;
		std::vector<std::string> m_history;
		std::string m_command;
		std::string m_log_msg;
		bool m_log_immediate;
		void onPositionChange();
		void tbox_enter(Control *c);
		void execute_command(std::string cmd);
		void onFontChange();
		void STYLE_FUNC(value);
		void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
		
	public:
	
		Terminal();
		~Terminal();
		
		enum event {
			command,
			max_events
		};
		
		void Render( Point position, bool isSelected );
		Terminal* Clone();
		const std::string& GetLastCommand();
		
		const std::string GetText();
		void SetText(const std::string& str);
		void ClearLog();
		void WriteLog(const std::string& s);
		void AppendLog(const std::string& s);
		void Focus();
		
		void OnMouseDown( int x, int y );
		void OnMouseUp( int x, int y );
};
}
#endif
