#include "ControlManager.hpp"
#include "../Widget.hpp"
#include <map>
#include <algorithm>
#include <list>
#include "SDL/Drawing.hpp"
namespace ng {
#define CACHE_SPACING 5

#define CONTROL_CACHE(control) \
	{ control->type, \
	  control->z_index, \
	  control->visible, \
	  control->interactible, \
	  control->isWidget, \
	  control->custom_check, \
	  control->m_rect, \
	  control }
	  
ControlManager::ControlManager(Control* c) : next_z_index(0), this_control(c) {
}

ControlManager::~ControlManager() {
	cache.clear();
	int len = controls.size();
	for(int i=0; i < len; i++) {
		delete controls[i];
	}
	controls.clear();
}

int ControlManager::binary_search(int z_index) {
	int size = cache.size();
	if(size == 0) return -1;
	int a=0,b=size-1,m=size>>1;
	// check edges
	if(cache[a].z_index == z_index) return a;
	else if(cache[b].z_index == z_index) return b;
	// a ----- m ----- b
	while(true) {
		if(z_index < cache[m].z_index) {
			b = m;
			m = (a+m) >> 1;
			if(b == m)
				return m;
		} else {
			a = m;
			m = (b+m) >> 1;
			if(a == m)
				return m;
		}
	}
	return m;
}

void ControlManager::setZIndex(Control* control, int new_z_index) {
	
	cache_entry e;
	int old = binary_search(control->z_index);
	int put_at = binary_search(new_z_index);

	
	int new_z;
	if(put_at > 0) {
		if(new_z_index > cache[cache.size()-1].z_index) {
			put_at++;
			new_z = cache[put_at].z_index + CACHE_SPACING;
		} else if(put_at != cache.size()-1) {
			new_z = (cache[put_at].z_index + cache[put_at+1].z_index) >> 1;
		} else {
			put_at--;
			new_z = (cache[put_at].z_index + cache[put_at+1].z_index) >> 1;
		}
	} else if(put_at == 0) {
		new_z = cache[0].z_index >> 1;
	}
	
	e = cache[old];
	e.z_index = new_z;
	control->z_index = new_z;
	
	if(old == put_at) {
		return;
	} else if(old < put_at) {
		// shift left
		for(int i = old; i < put_at; i++) {
			cache[i] = cache[i+1];
		}
		cache[put_at] = e;
	} else { // if (old > put_at)
		// shift right
		for(int i = old; i > put_at; i--) {
			cache[i] = cache[i-1];
		}
		cache[put_at] = e;
	}
	
	if(put_at > 0 && put_at < cache.size()-1) {
		if(new_z == cache[put_at+1].z_index ||
		   new_z == cache[put_at-1].z_index) {
		   rescale_z_indices(CACHE_SPACING);
	   }
	} else if(put_at == 0) {
		if(new_z == cache[put_at+1].z_index) {
			rescale_z_indices(CACHE_SPACING);
		}
	} else {
		if(new_z == cache[put_at-1].z_index) {
			rescale_z_indices(CACHE_SPACING);
		}
	}
	
}

void ControlManager::updateCache(Control* control, CacheUpdateFlag flag) {
	int index = binary_search(control->z_index);
	
	if(flag == CacheUpdateFlag::position) {
		cache[index].rect = control->m_rect;
	} else if(flag == CacheUpdateFlag::all) {
		cache[index] = CONTROL_CACHE(control);
	} else if(flag == CacheUpdateFlag::attributes) {
		cache_entry &e = cache[index];
		e.interactible = control->interactible;
		e.visible = control->visible;
	}
}

Rect getRect( int x, int y, int w, int h ) {
	Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

void ControlManager::rescale_z_indices(int spacing) {
	int size = cache.size();
	int cur = 0;
	for(int i=0; i < size; i++, cur+=spacing) {
		cache[i].z_index = cur;
		cache[i].control->z_index = cur;
	}
	next_z_index = cur;
}


void ControlManager::addControlToCache(Control* control) {
	int z_index = control->z_index;
	if(z_index == 0 or z_index >= next_z_index) {
		control->z_index = next_z_index;
		cache.push_back(CONTROL_CACHE(control));
		next_z_index += CACHE_SPACING;
	} else {
		int put_at = binary_search(z_index);
		if(put_at == cache.size()-1)
			cache.push_back(CONTROL_CACHE(control));
		else
			cache.insert(cache.begin()+put_at, CONTROL_CACHE(control));
	}
	
	controls.push_back(control);
}

void ControlManager::removeControlFromCache(Control* control) {
	int idx = binary_search(control->z_index);
	cache.erase(cache.begin()+idx);
	
	if(control->isWidget) {
		Widget* w = static_cast<Widget*>(control);
		w->set_engine(0);
	}
	
	for(auto j = controls.begin(); j != controls.end(); j++) {
		if(*j == control) {
			controls.erase(j);
			break;
		}
	}
}

void ControlManager::ApplyAnchoring() {
	std::map<Point,std::list<Control*>> grouping;
	int wres, hres;
	if(this_control) {
		const Rect &r = this_control->GetRect();
		wres = r.x;
		hres = r.y;
	} else {
		Drawing::GetResolution(wres, hres);	
	}
	for(Control* control : controls) {
		const Anchor &a = control->GetAnchor();
		const Rect &r = control->GetRect();
		const Point &coord = a.coord;
		control->SetRect( 0, 0, a.sx == 0 && a.sW == 0 ? r.w : wres * a.sW + a.sx, a.sy == 0 && a.sH == 0 ? r.h : hres * a.sH + a.sy );
		Point p(a.W * wres, 
				a.H * hres);
		if(a.isrelative) {
			grouping[p].push_back(control);
		} else {
			control->SetPosition(p.x + a.w * r.w + a.x, p.y + a.h * r.h + a.y);
		}
		
		if(control->IsWidget()) reinterpret_cast<Widget*>(control)->ApplyAnchoring();
	}
	
	for(auto &kv : grouping) {
		const Point &pt = kv.first;
		std::list<Control*> &l = kv.second;
		
		l.sort([](Control* a, Control* b) -> bool {
			const Point &p1 = a->GetAnchor().coord;
			const Point &p2 = b->GetAnchor().coord;
			return p1.x < p2.x || (p1.x == p2.x && p1.x < p2.x);
		});
		
		int lastx = 0, lasty = 0;
		int min_x = pt.x, min_y = pt.y;
		int max_x = min_x, max_y = min_y;
		
		for(Control* c : l) {
			const Anchor& a = c->GetAnchor();
			const Rect &r = c->GetRect();
			const Point &p = a.coord;
			
			if(p.y != lasty || (
			(a.ax >= 0 && max_x + a.x + r.w > wres) ||
			(a.ax < 0 && max_x - a.x - r.w < 0))) {
				lasty = p.y;
				lastx = 0;
				max_x = min_x;
				min_y = max_y;
			}
			int yc = (a.ay >= 0) ? 0 : (- r.h - a.y);
			if(a.ax >= 0) {
				c->SetPosition(max_x + a.x, min_y + yc);
				max_x += a.x + r.w;
			} else {
				c->SetPosition(max_x - a.x - r.w, min_y + yc);
				max_x -= a.x + r.w;
			}
				
			
			if(a.ay >= 0) {
				max_y = std::max<int>(min_y + r.h, max_y);
			} else {
				max_y = std::min<int>(min_y - r.h, max_y);
			}
			lastx++;
		}
	}
}

}

