#ifndef NG_CONTEXTMENU_HPP
#define NG_CONTEXTMENU_HPP
#include "../Control.hpp"
#include "ListBox.hpp"
#include "../Border.hpp"

namespace ng {

struct MenuItem {
	MenuItem(){}
	MenuItem(std::string str, std::vector<MenuItem> items={});
	std::string str;
	std::vector<MenuItem> items;
	ListBox* lbox;
};


	
class ContextMenu : public Control {
		private:
			MenuItem root;
			void process(MenuItem& item);
			void onShow(Args& a, MenuItem& item);
			int lx;
			Border* border;
			
			std::vector<Control*> menus;
			
			int findOpenedMenu(Control* c);
			void showListbox(MenuItem& item);
			void hideListbox(MenuItem& item);
			void hideAll();
			void fallbackTo(MenuItem& item);
			void parseXmlRecursive(MenuItem& item_parent, rapidxml::xml_node<char>* node);
			void parseXml(rapidxml::xml_node<char>* node);
			
		public:
			
			ContextMenu();
			ContextMenu(std::vector<MenuItem> items);
			void Show(Control* caller);
			
			
};

}


#endif
