#include "WidgetResizer.hpp"

namespace ng {
WidgetResizer::WidgetResizer() {
}

void WidgetResizer::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState && getWidget()) {
		Widget* wgt = getWidget();
		Rect r = wgt->GetRect();
		Rect mr = wgt->GetParentWidgetRegion();
		int w = last_rect.w + (mX - last_pos.x);
		w = clip(w, 5, mr.w - r.x);
		int h = last_rect.h + (mY - last_pos.y);
		h = clip(h, 5, mr.h - r.y);
		wgt->SetRect( r.x, r.y, w, h );
		wgt->ProcessLayout();
		poseEmitEvent(wgt, "resized");
	}
}
void WidgetResizer::OnMouseDown( int mX, int mY, MouseButton button ) {
	if(!getWidget()) return;
	last_rect = getWidget()->GetRect();
	last_pos = {mX,mY};
}
void WidgetResizer::OnMouseUp( int mX, int mY, MouseButton button ) {
	
}

void WidgetResizer::Render( Point pos, bool isSelected ) {
	Control::Render(pos,isSelected);
}
}
