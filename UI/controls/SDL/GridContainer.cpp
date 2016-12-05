#include "GridContainer.hpp"
#include "../../common/SDL/Drawing.hpp"

namespace ng {

GridContainer::GridContainer() {
	setType( "gridcontainer" );
	m_is_mouseDown = false;
	m_num_controls = 0;
}


int GridContainer::getPointedControl( int x, int y ) {
	const Rect& r = GetRect();
	int sel = ((x-r.x) / (r.w / m_grid_w)) % m_grid_w +
			 ((y-r.y) / (r.h / m_grid_h)) * m_grid_w;
	if( m_num_controls > sel )
		return sel;
	else
		return -1;
}


void GridContainer::Render( Point pos, bool isSelected ) {
	const Rect& r = GetRect();
	Drawing::Rect(r.x+pos.x, r.y+pos.y, r.w, r.h, 0xffffffff);
	RenderWidget(pos,isSelected);
}
void GridContainer::onPositionChange() {}

void GridContainer::AddItem( Control* control ) {
	if( m_num_controls >= m_grid_w * m_grid_h )
		return;
	
	const Rect& r = GetRect();
	int max_w = r.w / m_grid_w;
	int max_h = r.h / m_grid_h;
	
	int x = m_num_controls % m_grid_w;
	int y = m_num_controls / m_grid_w;
	
	Rect rect = control->GetRect();
	control->SetRect( x * max_w, y * max_h, std::min( rect.w, max_w ), std::min( rect.h, max_h ) );

	this->AddControl( control );
	m_num_controls++;
}

void GridContainer::SetGrid( int x, int y ) {
	m_grid_w = x;
	m_grid_h = y;
}

}
