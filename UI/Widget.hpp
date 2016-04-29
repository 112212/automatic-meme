#ifndef WIDGET
#define WIDGET 

#include "Control.hpp"
#include "common/cache.hpp"

namespace ng {
class GuiEngine;

class Widget : public Cache, public Control {
	
	private:
		unsigned int intercept_mask;
		friend class GuiEngine;
		friend class Control;
		friend class Cache;
		void set_engine(GuiEngine* engine);
		
		#ifdef USE_SFML
			sf::RectangleShape rect;
		#endif
		Control* selected_control;
		Point cached_absolute_offset;
	protected:
		enum imask {
			mouse_up = 0x01,
			mouse_down = 0x02,
			mouse_move = 0x04,
			key_down = 0x08,
			key_up = 0x10,
			mwheel = 0x20
		};
		void intercept();
		void setInterceptMask(unsigned int mask);
		unsigned int getInterceptMask() { return intercept_mask; }
		bool isThisWidgetSelected();
		bool isThisWidgetInSelectedBranch();
		Control* getSelectedControl() { return selected_control; }
		const Point& getAbsoluteOffset() { return cached_absolute_offset; };
		Point offset;
		
	public:
		Widget();
		~Widget();
		
		void OnMouseMove( int mX, int mY, bool mouseState ){}
		void OnMouseDown( int mX, int mY ){}
		void OnMouseUp( int mX, int mY ){}
		void OnLostFocus(){}
		void OnGetFocus(){}
		void OnLostControl(){}
		void OnMWheel( int updown ){}
		
		#ifdef USE_SFML
			virtual void Render( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected );
		#elif USE_SDL
			virtual void Render(  SDL_Rect position, bool isSelected );
		#endif
		
		
		virtual void AddControl( Control* control );
		void RemoveControl( Control* control );
		void LockWidget(bool lock);
		#ifdef USE_SFML
			void RenderWidget( sf::RenderTarget &ren, sf::RenderStates state, bool isSelected);
		#elif USE_SDL
			void RenderWidget( SDL_Rect position, bool isSelected );
		#endif
		void SetOffset(int x, int y);
		const Point& GetOffset() { return offset; }
};

}
#endif
