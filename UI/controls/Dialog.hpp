#ifndef NG_DIALOG
#define NG_DIALOG

#include "Label.hpp"
#include "../Widget.hpp"

namespace ng {
class Dialog : public Widget {
	
	private:
		Widget* wgt;
		int m_header_height;
		int m_header_x_width;
		Label* m_caption;
		
		void onRectChange();
		void onFontChange();
		void OnSetStyle(std::string& style, std::string& value);
	public:
		Dialog();
		~Dialog();
		Control* Clone();
		virtual void AddControl( Control* control );
		void Render( Point pos, bool isSelected );
};
}
#endif
