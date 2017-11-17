#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#include "Control.hpp"
#include "managers/ControlManager.hpp"

namespace ng {
class Gui;

class Widget : public Control, public ControlManager  {
	
	private:
		unsigned int intercept_mask;
		friend class Gui;
		friend class Control;
		friend class ControlManager;
		void set_engine(Gui* engine);
		
		Control* selected_control;
		Point cached_absolute_offset;
		Size min, max;
		Point m_offset;
		void setRect( int x, int y, int w, int h );
	protected:
	
		// ----- intercept events -----
		enum imask {
			mouse_up = 0x01,
			mouse_down = 0x02,
			mouse_move = 0x04,
			mwheel = 0x08,
			key_down = 0x10,
			key_up = 0x20,
			key_text = 0x40,
		};
		void setInterceptMask(unsigned int mask);
		void intercept();
		// ----------------------------
		
		bool isSelected();
		bool inSelectedBranch();
		
		unsigned int getInterceptMask() { return intercept_mask; }
		inline Control* getSelectedControl() { return selected_control; }
		inline const Point& getAbsoluteOffset() { return cached_absolute_offset; };
		virtual void parseXml(rapidxml::xml_node<char>* node);
		
	public:
		Widget();
		~Widget();
		
		Control* Clone();
		
		void OnMouseMove( int mX, int mY, bool mouseState ) {}
		void OnMouseDown( int mX, int mY, MouseButton button ) {}
		void OnMouseUp( int mX, int mY, MouseButton button ) {}
		
		void OnLostFocus() {}
		void OnGetFocus() {}
		void OnLostControl() {}
		void OnMWheel( int updown ){}
		
		virtual void Render( Point position, bool isSelected );
		virtual void AddControl( Control* control );
		
		void RemoveControl( Control* control );
		void LockWidget(bool lock);
		void RenderWidget( Point position, bool isSelected );
		void SetOffset(int x, int y);
		const Point& GetOffset() { return m_offset; }
		
		std::string GetSelectedRadioButton(int group=0);
};

}
#endif
