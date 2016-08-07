#ifndef NG_SCROLLBAR_HPP
#define NG_SCROLLBAR_HPP

#include "../Widget.hpp"
#include "SDL/TextBox.hpp"
namespace ng {
enum {
	
};

class Terminal : public Widget {
	private:
		TextBox *m_log;
		TextBox *m_terminal;
	
	public:
		Terminal();
		~Terminal();
		
		
		void Render( Point position, bool isSelected );
		
		Terminal* Clone();
		void OnMouseMove( int x, int y, bool mouseState );
		void OnMouseDown( int x, int y );
		void OnMouseUp( int x, int y );
		void OnLostFocus();
		void OnMWheel( int updown );
		void SetValue( int value );
};
}
#endif
