#include <RapidXML/rapidxml.hpp>
#include "ContextMenu.hpp"
#include "../Gui.hpp"
#include <algorithm>

namespace ng {
	
class MenuBorder : public Border {
	public:
		std::vector<Control*> *menus;
		MenuBorder(std::vector<Control*> *c) : menus(c) {}
		bool CheckCollision(const Point& p) {
			for(Control* c : *menus) {
				if(c->CheckCollision(p)) {
					return true;
				}
			}
			return false;
		}
};

MenuItem::MenuItem(std::string str, std::vector<MenuItem> items) {
	this->str = str;
	if(!items.empty()) {
		this->items.insert(this->items.end(), items.begin(), items.end());
	}
	lbox=0;
}


ContextMenu::ContextMenu() {
	GetLayout().SetSize(0,1,0,1);
	GetLayout().SetAbsoluteCoords(1,1);
}

ContextMenu::ContextMenu(std::vector<MenuItem> items) {
	root.lbox = 0;
	root.items = items;
	// SetLayout(1,0,0, 1,0,0);
	
	GetLayout().SetSize(0,1,0,1); // W=1, H=1
	GetLayout().SetAbsoluteCoords(1,1);
	
	SetBorder(new MenuBorder(&menus));
	// m_style.SetTransparentBackground();
	// m_style.SetTransparentBorder();
	process(root);
}


void ContextMenu::process(MenuItem& item) {
	auto &lb = item.lbox;
	lb = createControl<ListBox>("listbox", "listbox_menu");
	lb->SetFont("hehe2");
	lb->SetStyle("hoverselectionmode", "t");
	lb->SetVisible(false);
	
	if(!item.items.empty()) {
		// std::cout << "CLK: " << item.items.size() << "\n";
		// item.lbox->OnEvent("click", &ContextMenu::onShow, this);
		item.lbox->OnEvent("click", [&](Args& a) {
			onShow(a, item);
		});
		item.lbox->OnEvent("lostcontrol", [&](Args& a) {
			// std::cout << "lost control " << a.control->GetId() << "\n";
			
			a.control->SetVisible(false);
			hideListbox(item);
			lb->SetVisible(false);
		});
		
		item.lbox->OnEvent("activate", [&](Args& a) {
			
		});
	}
	
	// std::cout << "processing items\n";
	for(auto &i : item.items) {
		// std::cout << "adding item: " << i.str << "\n";
		item.lbox->AddItem(i.str);
		if(!i.items.empty()) {
			process(i);
		}
	}
	// std::cout << "end processing\n";
	// lb->SetRect(0,0,0,0);
	AddControl(lb, false);
}

void ContextMenu::Show(Control* caller) {
	if(!getEngine() || !root.lbox) {
		return;
	}
	
	sendGuiCommand( GUI_LOCK_ONCE, this );
	sendGuiCommand( GUI_UNLOCK_BY_MOUSEUP );
	// root.lbox->SetLayout("100,100,100,100");
	lx = 0;
	
	OnEvent("lostcontrol", [&](Args& a) {
		// std::cout << "lost control " << a.control->GetId() << "\n";
		// a.control->SetVisible(false);
		getEngine()->RemoveControl(a.control);
		// hideListbox(item);
	});
	
	int w,h;
	Drawing().GetResolution(w,h);
	SetRect(0,0,w,h);
	showListbox(root);
	SendToFront();
	// std::cout << "rect: " << GetRect() << "\n";
	// getEngine()->AddControl(this);
	// SetPosition(pt.x, pt.y);
}

void ContextMenu::onShow(Args& a, MenuItem& item) {
	auto *lb = static_cast<ListBox*>(a.control);
	int idx = lb->GetSelectedIndex();
	MenuItem& itm = item.items[idx];
	int openedmenu = findOpenedMenu(itm.lbox);
	
	if(!itm.lbox) {
		// std::cout << "do action: " << itm.str << "\n";
		emitEvent("action", {itm.str});
		hideAll();
	} else if(openedmenu == -1) {
		fallbackTo(item);
		showListbox(itm);
	}
}


int ContextMenu::findOpenedMenu(Control* c) {
	auto it = std::find(menus.begin(), menus.end(), c);
	return it != menus.end() ? std::distance(menus.begin(), it) : -1;
}

void ContextMenu::showListbox(MenuItem& item) {
	auto* lbox = item.lbox;
	if(lbox) {
		
		// std::cout << "lbox: " << item.str << "\n";
		lbox->SetRect(0, 0, 150, 100);
		AddControl(lbox);
		lbox->SetVisible(true);
		
		Point pt = getCursor();
		if(lx == 0) {
			lbox->SetPosition(pt);
			lx += lbox->GetRect().w;
		} else {
			Rect r = menus.back()->GetRect();
			int x2 = r.x + r.w;
			Rect lbox_rect = lbox->GetRect();
			// std::cout << "menu: " << menus.back()->GetId() << " : " << x2 << " : " << GetRect().w << "\n";
			if(x2 + lbox_rect.w > GetRect().w) {
				r = menus.front()->GetRect();
				lbox->SetPosition({r.x-100,pt.y});
			} else {
				lbox->SetPosition({x2,pt.y});
			}
		}
		// SetRect(pt.x, pt.y, 200,200);
		SetVisible(true);
		menus.push_back(item.lbox);
		lbox->Activate();
	}
}

void ContextMenu::fallbackTo(MenuItem& item) {
	if(menus.empty()) return;
	
	for(int i = menus.size()-1; i >= 0; i--) {
		Control *it = menus[i];
		if(it != item.lbox) {
			it->SetVisible(false);
			menus.erase(menus.begin()+i);
		} else {
			break;
		}
	}
}

void ContextMenu::hideListbox(MenuItem& item) {
	auto* lbox = item.lbox;
	if(lbox) {
		// std::cout << "lbox: " << item.str << "\n";
		lbox->SetRect(0,0,100,100);
		lbox->SetVisible(false);
	}
}

void ContextMenu::hideAll() {
	for(auto& m : menus) {
		m->SetVisible(false);
	}
	menus.clear();
	Unselect();
}


// XML

void ContextMenu::parseXmlRecursive(MenuItem& item_parent, rapidxml::xml_node<char>* node) {
	for(;node;node=node->next_sibling()) {
		if(std::string(node->name()) == "item") {
			MenuItem mi(node->first_attribute("name")->value());
			parseXmlRecursive(mi, node->first_node());
			item_parent.items.push_back(mi);
		}
	}
}

void ContextMenu::parseXml(rapidxml::xml_node<char>* node) {
	parseXmlRecursive(root, node);
	
	root.lbox = 0;
	// SetLayout(1,0,0, 1,0,0);
	GetLayout().SetSize(0,1,0,1);
	GetLayout().SetAbsoluteCoords(1,1);
	SetBorder(new MenuBorder(&menus));
	m_style.SetTransparentBackground();
	m_style.SetTransparentBorder();
	process(root);
}

}
