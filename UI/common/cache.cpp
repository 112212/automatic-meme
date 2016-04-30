#include "cache.hpp"
#include "../Widget.hpp"

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
	  
Cache::Cache() : next_z_index(0) {
}

Cache::~Cache() {
	cache.clear();
	int len = controls.size();
	for(int i=0; i < len; i++) {
		delete controls[i];
	}
	controls.clear();
}

int Cache::binary_search(int z_index) {
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

void Cache::setZIndex(Control* control, int new_z_index) {
	
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

void Cache::updateCache(Control* control, CacheUpdateFlag flag) {
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

void Cache::rescale_z_indices(int spacing) {
	int size = cache.size();
	int cur = 0;
	for(int i=0; i < size; i++, cur+=spacing) {
		cache[i].z_index = cur;
		cache[i].control->z_index = cur;
	}
	next_z_index = cur;
}


void Cache::addControlToCache(Control* control) {
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

void Cache::removeControlFromCache(Control* control) {
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
}
