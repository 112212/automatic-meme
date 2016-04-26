#ifndef WGT_MOVER
#define WGT_MOVER
#include "../Control.hpp"
#include "../Widget.hpp"
namespace ng {
class WidgetMover : public Control {
	private:
		Rect last_pos;

	public:
		WidgetMover();
		
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected );
};
}
#endif
