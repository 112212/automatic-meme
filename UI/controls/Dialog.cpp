#include <RapidXML/rapidxml.hpp>

#include "Dialog.hpp"
#include "WidgetMover.hpp"
#include "WidgetResizer.hpp"
#include "Button.hpp"

namespace ng {


Dialog::Dialog() : Widget() {
	setType( "dialog" );
	
	m_header_height = 25;
	m_header_x_width = 20;
	Layout l;
	l.SetPosition(0,m_header_height,0,0,0,0,true);
	l.SetSize(0,1,-m_header_height,1);
	wgt = (Widget*)ControlManager::CreateControl("widget");//new Widget();
	wgt->SetLayout(l);
	Widget::AddControl(wgt);
	
	WidgetMover* wm = (WidgetMover*)ControlManager::CreateControl("widgetmover"); new WidgetMover();
	l.SetSize(-m_header_x_width,1,m_header_height,0);
	l.SetPosition(0,0,0,0,0,0);
	l.SetCoord(Point(0,0));
	wm->SetLayout(l);
	
	Widget::AddControl(wm);
	
	m_caption = (Label*)ControlManager::CreateControl("label");//new Label();
	l.absolute_coordinates = true;
	m_caption->SetLayout(l);
	m_caption->SetZIndex(100);
	m_caption->SetAlignment(Alignment::center);
	Widget::AddControl(m_caption);
	
	Button* btn_x = (Button*)ControlManager::CreateControl("button");//new Button();
	l.SetSize(m_header_x_width, 0, m_header_height, 0);
	l.SetPosition(0,0,0,0,0,0);
	l.SetCoord(Point(1,0));
	btn_x->SetText("X");
	btn_x->SetLayout(l);
	btn_x->OnEvent("click", [&](Args& args) {
		emitEvent("close");
	});
	Widget::AddControl(btn_x);
	
	WidgetResizer* ws = (WidgetResizer*)ControlManager::CreateControl("widgetresizer");//new WidgetResizer();
	l = Layout("R,B,10,10");
	ws->SetLayout(l);
	ws->SetStyle("noborder", "true");
	
	Widget::AddControl(ws);
	
}


void Dialog::onRectChange() {
}

void Dialog::onFontChange() {
	m_caption->SetFont(m_style.font);
}

void Dialog::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("caption"):
			m_caption->SetText(value);
	}
}

Control* Dialog::Clone() {
	Dialog* l = new Dialog();
	copyStyle(l);
	return l;
}

Dialog::~Dialog() {
	delete wgt;
}

void Dialog::AddControl( Control* control ) {
	wgt->AddControl(control);
}

void Dialog::Render( Point pos, bool isSelected ) {
	RenderWidget(pos, isSelected);
	Control::Render(pos,isSelected);
}

}
