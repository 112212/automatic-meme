#ifndef NG_TERMINAL_HPP
#define NG_TERMINAL_HPP

#include <unordered_map>

#include "../Control.hpp"
#include "TextBox.hpp"
#include "../managers/Effects.hpp"

namespace ng {

class Terminal : public Control {
	private:
		TextBox *m_log;
		Effects::AutoFade* m_log_fade_effect;
		TextBox *m_terminal;
		int m_history_counter;
		std::vector<std::string> m_history;
		std::string m_command;
		std::string m_log_msg;

		bool m_log_immediate;
		void onRectChange();
		void tbox_enter(Control *c);
		void execute_command(std::string cmd);
		void onFontChange();
		void OnSetStyle(std::string& style, std::string& value);
		void OnKeyDown( Keycode &sym, Keymod mod );
		
	public:
	
		Terminal();
		~Terminal();
		
		void Render( Point position, bool isSelected );
		Terminal* Clone();
		const std::string& GetLastCommand();
		
		const std::string GetText();
		void SetText(const std::string& str);
		void ClearLog();
		void WriteLog(const std::string& s);
		void AppendLog(const std::string& s);
		void Focus();
		
		void OnMouseDown( int x, int y, MouseButton button );
		void OnMouseUp( int x, int y, MouseButton button );
		void OnText( std::string s );
};
}
#endif
