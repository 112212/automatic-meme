#ifndef NG_LAYOUT_HPP
#define NG_LAYOUT_HPP
#include "common.hpp"
namespace ng {
class Control;
class ControlManager;

class Layout {
	public:
		enum SizeFunction {
			none,
			keep,
			fit,
			expand,
			fill
		};
	private:
		friend class Control;
		friend class ControlManager;
		bool enabled;
		Point coord;
		float x,w;
		float y,h;
		float W,H;
		
		Rect padding;
		
		bool absolute_coordinate_x;
		bool absolute_coordinate_y;
		
		// only for widgets
		
		SizeFunction w_func;
		SizeFunction h_func;
		float w_min[2];
		float w_max[2];
		float h_min[2];
		float h_max[2];
		
	public:
	
	Layout();
	Layout( Point coord, float x, float y, float W, float w, float H, float h );
	Layout(std::string s);
	Rect GetPadding();
	void SetPadding(Rect padding);
	void SetPadding(std::string str);
	void SetCoord( Point coord );
	void SetPosition( float x, float y, float w=0, float W=0, float h=0, float H=0, bool absolute_coordinates = false );
	void SetSize( float w, float W, float h, float H );
	void SetSizeRange( float min_w, float min_W, float min_h, float min_H, float max_w, float max_W, float max_h, float max_H, SizeFunction w_func=fit, SizeFunction h_func=fit );
	void SetFuncs(SizeFunction w_func=fit, SizeFunction h_func=fit);
	void SetAbsoluteCoords(bool abs_x, bool abs_y);
	void SetEnabled(bool enabled);
	bool Enabled();
	Layout& operator+=(const Layout& b);
	friend std::ostream& operator<< (std::ostream& stream, const Layout& anchor);
	static Layout parseRect(std::string s);
};

}
#endif
