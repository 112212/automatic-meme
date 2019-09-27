#include <RapidXML/rapidxml.hpp>

#include "Dialog.hpp"
#include "WidgetMover.hpp"
#include "WidgetResizer.hpp"
#include "Button.hpp"

namespace ng {


Dialog::Dialog() {
	setType( "dialog" );
	
	m_header_height = 25;
	m_header_x_width = 20;
	
	Layout l;
	l.SetPosition(0,m_header_height,0,0,0,0,true);
	// l.SetSizeRange(0,1,-m_header_height,1, 9999, 0, 9999, 0);
	Size min,max;
	getRange(min,max);
	l.SetSizeRange(0,1,-m_header_height, 1, 0, 1, 0, 1);
	// l.SetSizeRange(0,1,-m_header_height, 1, min.w, 0, min.h, 0);
	l.SetFuncs(Layout::SizeFunction::fit, Layout::SizeFunction::fit);
	// l.SetFuncs(Layout::SizeFunction::none, Layout::SizeFunction::none);
	wgt = createControl<Control>("control", "widget");
	wgt->SetLayout(l);
	Control::AddControl(wgt);
	
	m_caption = createControl<Label>("label", "caption");
	l.SetSize(-m_header_x_width,1,m_header_height,0);
	l.SetPosition(0,0,0,0,0,0);
	l.SetCoord({0,0});
	m_caption->SetStyle("noborder","true");
	m_caption->SetLayout(l);
	m_caption->SetAlignment(Alignment::center);
	Control::AddControl(m_caption);
	

	setOptions("_mMX");
	
	WidgetResizer* ws = createControl<WidgetResizer>("widgetresizer", "widgetresizer");
	l = Layout("R,B,10,10");
	ws->SetLayout(l);
	// ws->SetStyle("noborder", "true");
	Control::AddControl(ws);
	
	m_caption->SendToFront();
	
	setInterceptMask(imask::mouse_move | imask::key_down);
	
}

void Dialog::OnKeyDown( Keycode sym, Keymod mod ) {
	if(sym == Keycode::KEY_RETURN || sym == Keycode::KEY_KP_ENTER) {
		emitEvent("enter");
	}
}

void Dialog::onRectChange() {
	// ProcessLayout();
}

void Dialog::onFontChange() {
	m_caption->SetFont(m_style.font);
}

void Dialog::setOptions(std::string options) {
	Button* btn_close = Get<Button>("X");
	Button* btn_minimize = Get<Button>("_");
	Button* btn_maximize = Get<Button>("M");
	int opts_size = 0;
	
	if(options.find('X') != std::string::npos) {
		if(!btn_close) {
			btn_close = createControl<Button>("button","X");
			Layout l;
			l.SetSize(m_header_x_width, 0, m_header_height, 0);
			l.SetPosition(0,0,0,0,0,0);
			l.SetCoord(Point(1,0));
			btn_close->SetText("X");
			btn_close->SetLayout(l);
			btn_close->OnEvent("click", [&](Args& args) {
				emitEvent("close");
			});
			Control::AddControl(btn_close);
			opts_size += m_header_x_width;
		}
	} else if(btn_close) {
		RemoveControl(btn_close);
		delete btn_close;
	}
	
	if(options.find('m') != std::string::npos) {
		WidgetMover* wm = Get<WidgetMover>("widgetmover");
		Layout l;
		if(!wm) {
			wm = createControl<WidgetMover>("widgetmover", "widgetmover");
			l.SetSize(-m_header_x_width,1,m_header_height,0);
			l.SetPosition(0,0,0,0,0,0, true);
			l.SetCoord(Point(0,0));
			wm->SetLayout(l);
			Control::AddControl(wm);
		}
		
		wm = Get<WidgetMover>("widgetmover");
		l = wm->GetLayout();
		l.SetSize(-opts_size,1,m_header_height,0);
		wm->SetLayout(l);
	} else {
		WidgetMover* wm = Get<WidgetMover>("widgetmover");
		if(wm) {
			// remove it
			RemoveControl(wm);
		}
	}
	
	Label* caption = Get<Label>("caption");
	if(caption) {
		Layout l;
		l = caption->GetLayout();
		l.SetSize(-opts_size,1,m_header_height,0);
		caption->SetLayout(l);
		ProcessLayout();
	}
	
	if(options.find('_') != std::string::npos) {
		
	}
	
}

void Dialog::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("caption"):
			if(m_caption) {
				m_caption->SetText(value);
			}
		_case("options"):
			setOptions(value);
	}
}

Control* Dialog::Clone() {
	Dialog* l = new Dialog();
	copyStyle(l);
	return l;
}

Dialog::~Dialog() {
	delete wgt;
	if(m_caption) {
		delete m_caption;
	}
}

void Dialog::AddControl( Control* control, bool processlayout ) {
	wgt->AddControl(control,processlayout);
	// Layout l = GetLayout();
	// l.SetSize(
	// wgt->GetRect();
	// ProcessLayout();
	// std::cout << "new rect: " << GetRect() << "\n";
}

void Dialog::Render( Point pos, bool isSelected ) {
	// RenderWidget(pos, isSelected);
	Control::Render(pos,isSelected);
}

}
