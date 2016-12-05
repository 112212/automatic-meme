#ifndef WGT_MOVER
#define WGT_MOVER
#include "../../Control.hpp"
#include "../../Widget.hpp"
namespace ng {
class WidgetMover : public Control {
	private:
		Point last_pos;

	public:
		WidgetMover();
		
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY );
		void OnMouseUp( int mX, int mY );
		void Render( Point pos, bool isSelected );
};
}
#endif
