#include "WidgetMover.hpp"
#include "../../common/SDL/Drawing.hpp"

namespace ng {
WidgetMover::WidgetMover() {
}

void WidgetMover::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState) {
		const Rect &r = getWidget()->GetRect();
		getWidget()->SetPosition(mX + r.x - last_pos.x, mY + r.y - last_pos.y);
	}
}
void WidgetMover::OnMouseDown( int mX, int mY, MouseButton button ) {
	if(!getWidget()) return;
	
	last_pos = {mX,mY};
}
void WidgetMover::OnMouseUp( int mX, int mY, MouseButton button ) {
	
}

void WidgetMover::Render( Point pos, bool isSelected ) {
	Control::Render(pos,isSelected);
}
}
