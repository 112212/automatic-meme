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
		
		void onPositionChange();
		void tbox_enter(Control *c);
		void execute_command(std::string cmd);
	public:
	
		Terminal();
		~Terminal();
		
		enum event {
			command,
			max_events
		};
		
		void Render( Point position, bool isSelected );
		Terminal* Clone();
		const std::string& GetText();
		void WriteLog(const std::string& s);
		
		void OnMouseDown( int x, int y );
		void OnMouseUp( int x, int y );
		
	private:
		std::string m_command;
		Point m_last_pos;
};
}
#endif
