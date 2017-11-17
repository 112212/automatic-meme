#ifndef BORDER_HPP
#define BORDER_HPP

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
		virtual bool CheckCollision(int x, int y) {
			Rect& r = *m_rect;
			// std::cout << control->GetId() << " " << control << " Border: " << r.x << ", " << r.y << "\n";
			return (x >= r.x && x <= r.x+r.w && y >= r.y && y <= r.y+r.h);
		}
};

}

#endif
