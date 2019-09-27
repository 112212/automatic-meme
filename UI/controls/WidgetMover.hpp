#ifndef NG_WGT_MOVER_HPP
#define NG_WGT_MOVER_HPP
#include "../Control.hpp"
#include "../Control.hpp"
namespace ng {
class WidgetMover : public Control {
	private:
		Point last_pos;

	public:
		WidgetMover();
		
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY, MouseButton button );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void Render( Point pos, bool isSelected );
};
}
#endif
