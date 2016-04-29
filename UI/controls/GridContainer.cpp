#include "GridContainer.hpp"
#ifdef USE_SDL
// #include <SDL2/SDL2_gfxPrimitives.h>
#include "../common/SDL/Drawing.hpp"
#endif

namespace ng {

GridContainer::GridContainer() {
	m_is_mouseDown = false;
	#ifdef USE_SFML
		m_rectShape.setFillColor( sf::Color::Transparent );
		m_rectShape.setOutlineColor( sf::Color::White );
		m_rectShape.setOutlineThickness( 1 );
	#endif
	m_numngControls = 0;
}



int GridContainer::getPointedControl( int x, int y ) {
	int sel = ((x-m_rect.x) / (m_rect.w / m_grid_w)) % m_grid_w +
			 ((y-m_rect.y) / (m_rect.h / m_grid_h)) * m_grid_w;
	if( m_numngControls > sel )
		return sel;
	else
		return -1;
}


#ifdef USE_SFML
	void GridContainer::Render( sf::RenderTarget& ren, sf::RenderStates states, bool isSelected ) {
		
		ren.draw( m_rectShape );
		RenderWidget(ren,states,isSelected);
	}
	void GridContainer::onPositionChange() {
		m_rectShape.setPosition( m_rect.x, m_rect.y );
		m_rectShape.setSize( sf::Vector2f( m_rect.w, m_rect.h ) );
	}
#elif USE_SDL
	void GridContainer::Render( SDL_Renderer* ren, SDL_Rect pos, bool isSelected ) {
		// rectangleColor(ren, m_rect.x+pos.x, m_rect.y+pos.y, m_rect.x+m_rect.w+pos.x, m_rect.y+m_rect.h+pos.y, 0xffffffff);
		Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, 0xffffffff);
		RenderWidget(pos,isSelected);
	}
	void GridContainer::onPositionChange() {}
#endif

void GridContainer::AddItem( Control* control ) {
	if( m_numngControls >= m_grid_w * m_grid_h )
		return;
		
	int max_w = m_rect.w / m_grid_w;
	int max_h = m_rect.h / m_grid_h;
	
	int x = m_numngControls % m_grid_w;
	int y = m_numngControls / m_grid_w;
	
	Rect rect = control->GetRect();
	control->SetRect( x * max_w, y * max_h, std::min( rect.w, max_w ), std::min( rect.h, max_h ) );

	this->AddControl( control );
	m_numngControls++;
}

void GridContainer::SetGrid( int x, int y ) {
	m_grid_w = x;
	m_grid_h = y;
}

}
