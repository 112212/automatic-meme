#ifndef GUI_XML_LOADER_HPP
#define GUI_XML_LOADER_HPP
#include <string>
#include <istream>

namespace ng {
class GuiEngine;
namespace XmlLoader {
	void LoadXml(GuiEngine &eine, std::string filename);
	void LoadXml(GuiEngine &eine, std::istream& input_xml);
}
}
#endif
