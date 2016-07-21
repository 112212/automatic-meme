#ifndef GUI_XML_LOADER_HPP
#define GUI_XML_LOADER_HPP
#include <string>
#include <istream>
#include <functional>
namespace ng {
	class GuiEngine;
	class Control;
	namespace XmlLoader {
		void LoadXml(GuiEngine &engine, std::string filename);
		void LoadXml(GuiEngine &engine, std::istream& input_xml);
		void RegisterControl(std::string tag, std::function<Control*()> control_constructor);
	}
}
#endif
