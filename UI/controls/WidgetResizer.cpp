#include "WidgetResizer.hpp"

namespace ng {
WidgetResizer::WidgetResizer() {
}

void WidgetResizer::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState && getParent()) {
		Control* wgt = getParent();
		Rect r = wgt->GetRect();
		Rect mr = wgt->GetParentRect();
		
		auto a = getAbsoluteOffset();
		Point p = Point(mX,mY)+a;
		int w = last_rect.w + (p.x - last_pos.x);
		w = clip(w, 5, mr.w - r.x);
		// w = clip(w, wgt->min.w, wgt->max.w);
		int h = last_rect.h + (p.y - last_pos.y);
		h = clip(h, 5, mr.h - r.y);
		// h = clip(h, 5, mr.h);
		wgt->SetSize( w, h );
		wgt->ProcessLayout(true);
		poseEmitEvent(wgt, "resized");
	}
}

void WidgetResizer::OnMouseDown( int mX, int mY, MouseButton button ) {
	if(!getParent()) return;
	last_rect = getParent()->GetRect();
	auto a = getAbsoluteOffset();
	last_pos = Point(mX,mY)+a;
}

void WidgetResizer::OnMouseUp( int mX, int mY, MouseButton button ) {
	
}

void WidgetResizer::Render( Point pos, bool isSelected ) {
	Control::Render(pos,isSelected);
}

}
