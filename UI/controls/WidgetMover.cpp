#include <iostream>

#include "WidgetMover.hpp"

namespace ng {
WidgetMover::WidgetMover() {
}

void WidgetMover::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState) {
		Widget* w = getWidget();
		const Rect &r = w->GetRect();
		
		Rect mr = w->GetParentWidgetRegion();
		
		// mX, mY - parent widget local coords
		// r.x, r.y - widgets local to its parent coords
		int dx = mX - last_pos.x;
		int dy = mY - last_pos.y;
		int x = clip(r.x + dx, 0, mr.w-r.w);
		int y = clip(r.y + dy, 0, mr.h-r.h);
		w->SetPosition(x, y);
		poseEmitEvent(w, "moved");
		w->SendToFront();
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
