#include "WidgetResizer.hpp"
#include "../../common/SDL/Drawing.hpp"

namespace ng {
WidgetResizer::WidgetResizer() {
}

void WidgetResizer::OnMouseMove( int mX, int mY, bool mouseState ) {
	if(mouseState && getWidget()) {
		Widget* wgt = getWidget();
		const Rect &r = wgt->GetRect();
		wgt->SetRect( r.x, r.y, last_rect.w + (mX - last_pos.x), last_rect.h + (mY - last_pos.y) );
	}
}
void WidgetResizer::OnMouseDown( int mX, int mY ) {
	if(!getWidget()) return;
	last_rect = getWidget()->GetRect();
	last_pos = {mX,mY};
}
void WidgetResizer::OnMouseUp( int mX, int mY ) {
	
}

void WidgetResizer::Render( Point pos, bool isSelected ) {
	Control::Render(pos,isSelected);
}
}
