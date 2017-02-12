#ifndef WGT_RESIZER
#define WGT_RESIZER
#include "../../Control.hpp"
#include "../../Widget.hpp"
namespace ng {
class WidgetResizer : public Control {
	private:
		Point last_pos;
		Rect last_rect;
	public:
		WidgetResizer();
		
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseDown( int mX, int mY, MouseButton button );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void Render( Point pos, bool isSelected );
};
}
#endif
