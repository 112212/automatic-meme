#ifndef NG_BORDER_HPP
#define NG_BORDER_HPP

#include "common.hpp"
#include <iostream>
// #include "Control.hpp"

namespace ng {

class Border {
	private:
		friend class Control;
	protected:
		Rect* m_rect;
		// Control* control;
	public:
		virtual bool CheckCollision(const Point& p) {
			Rect& r = *m_rect;
			return (p.x >= r.x && p.x <= r.x+r.w && p.y >= r.y && p.y <= r.y+r.h);
		}
};

}

#endif
