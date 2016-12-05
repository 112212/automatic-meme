#include "WidgetMover.hpp"
#include "../common/SDL/Drawing.hpp"

namespace ng {
WidgetMover::WidgetMover() {
}

void WidgetMover::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState) {
		const Rect &r = getWidget()->GetRect();
		getWidget()->SetPosition(mX + r.x - last_pos.x, mY + r.y - last_pos.y);
	}
}
void WidgetMover::OnMouseDown( int mX, int mY ) {
	if(!getWidget()) return;
	
	last_pos = {mX,mY,0,0};
}
void WidgetMover::OnMouseUp( int mX, int mY ) {
	
}

void WidgetMover::Render( Point pos, bool isSelected ) {
	Control::Render(pos,isSelected);
}
}
