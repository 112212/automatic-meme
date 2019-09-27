#ifndef NG_DIALOG
#define NG_DIALOG

#include "Label.hpp"
#include "../Control.hpp"

namespace ng {
class Dialog : public Control {
	
	private:
		Control* wgt;
		
		int m_header_height;
		int m_header_x_width;
		Label* m_caption;
		
		void onRectChange();
		void onFontChange();
		void OnSetStyle(std::string& style, std::string& value);
		virtual void OnKeyDown( Keycode sym, Keymod mod );
		void setOptions(std::string);
	public:
		Dialog();
		~Dialog();
		Control* Clone();
		virtual void AddControl( Control* control, bool processlayout=true );
		void Render( Point pos, bool isSelected );
};
}
#endif
