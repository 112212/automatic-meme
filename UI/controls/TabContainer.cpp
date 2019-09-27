#include <RapidXML/rapidxml.hpp>

#include "Label.hpp"
#include "Button.hpp"
#include "TabContainer.hpp"

namespace ng {
TabContainer::TabContainer() {
	setType("tabcontainer");
	m_cur_tab = -1;
	setInterceptMask(imask::mouse_down);
	m_tab_height = 40;
}


Control* TabContainer::NewTab(std::string tabname) {
	TabContext tab;
	tab.widget = createControl("control","new-tab-widget");
	tab.tabname = tabname;
	
	int tab_idx = m_tabs.size();
	Layout tl;
	tl.SetCoord( Point( tab_idx, 0 ) );
	tl.SetSize(80,0,30,0);
	// Button* lb = createControl<Button>("button", GetId() + "_"+tabname+"_tab");
	Button* lb = createControl<Button>("button", "tab");
	tab.tab = lb;
	lb->SetText(tabname.c_str());
	// lb->SetAlignment( Alignment::center );
	// lb->SetId(tabname);
	lb->SetLayout(tl);
	AddControl(lb);
	
	Layout l;
	l.SetPosition(0,m_tab_height, 0,0, 0,0, true);
	l.SetSize(0,1,-m_tab_height,1);
	tab.widget->SetLayout(l);
	AddControl(tab.widget);
	
	lb->OnEvent("click", [=](Args& args) {
		m_tabs[m_cur_tab].widget->SetVisible(false);
		m_cur_tab = tab_idx;
		m_tabs[m_cur_tab].widget->SetVisible(true);
	});
	
	if(m_cur_tab == -1) {
		m_cur_tab = 0;
		tab.widget->SetVisible(true);
	} else {
		tab.widget->SetVisible(false);
	}
	m_tabs.push_back(tab);
	ProcessLayout();
	return tab.widget;
}

Control* TabContainer::Clone() {
	TabContainer* l = createControl<TabContainer>("tabcontainer");
	copyStyle(l);
	return l;
}


Control* TabContainer::GetTabWidget(std::string tabname) {
	for(TabContext& t : m_tabs) {
		if(t.tabname == tabname) {
			return t.widget;
		}
	}
	return 0;
}

Control* TabContainer::GetTabWidget(int idx) {
	if(idx < m_tabs.size()) {
		return m_tabs[idx].widget;
	} else {
		return 0;
	}
}

void TabContainer::parseXml(rapidxml::xml_node<>* node) {
	for(;node;node=node->next_sibling()) {
		if(std::string(node->name()) == "tab") {
			rapidxml::xml_attribute<>* attr = node->first_attribute("name");
			if(!attr) {
				continue;
			}
			Control* wgt = NewTab(attr->value());
			Layout l;
			for(rapidxml::xml_node<> *node2=node->first_node(); node2; node2=node2->next_sibling()) {
				wgt->parseAndAddControl(node2, l);
			}
		}
	}
}

void TabContainer::Render( Point pos, bool isSelected ) {
	// std::cout << "rendering tabcontainer\n";
	RenderWidget(pos, isSelected);
	Control::Render(pos, isSelected);
}

const int tab_overlap = 15;
void TabContainer::onRectChange() {
	
}

void TabContainer::OnMouseDown( int mX, int mY ) {
	
}

}
