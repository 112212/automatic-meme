#include "Layout.hpp"
#include <cstring>
#include <vector>
namespace ng {

static void monomial_expr_parse(const char* str, const char *names, float *result) {
	int current_index=0;
	const char* s = str;
	int names_length = strlen(names);
	int is_first = 1;
	int is_minus = 0;
	int is_div = 0;
	memset(result, 0, sizeof(float)*(names_length+1));
	float res = 0;
	while(1) {
		const char c = *s;
		if(c == '\0' || c == '+' || c == '-' || c == '/') {
			if(c == '-') {
				is_minus = 1;
			} else if(c == '/') {
				is_div = 1;
				s++;
				continue;
			}
			result[current_index] += res;
			is_first = 1;
			if(c == '\0') {
				break;
			}
			res = 0;
			current_index = 0;
		} else if(isdigit(c) || c == '.') {
			float num = strtof(s, (char**)&s);
			if(is_minus) {
				is_minus = 0;
				num = -num;
			}
			if(is_first) {
				res = num;
				is_first = 0;
			} else if(is_div) {
				res /= num;
				is_div = 0;
			} else {
				res *= num;
			}
			continue;
		} else if(c == '%') {
			res *= 0.01;
		} else {
			int i;
			for(i=0; i < names_length; i++) {
				if(names[i] == c) {
					if(res == 0) res = 1;
					current_index=i+1;
					break;
				}
			}
		}
		s++;
	}
}

Rect Layout::GetPadding() {
	return padding;
}

void Layout::SetPadding(Rect padding) {
	this->padding = padding;
}

void Layout::SetPadding(std::string str) {
	
	std::vector<std::string> res;
	split_string(str, res, ',');
	if(res.size() == 1) {
		int num = std::stoi(res.front());
		padding.x=padding.y=num;
		padding.w=padding.h=num;
	} else if(res.size() == 2) {
		int num1 = std::stoi(res[0]);
		int num2 = std::stoi(res[1]);
		padding.x=padding.w=num1;
		padding.y=padding.h=num2;
	} else if(res.size() >= 3) {
		int num1 = std::stoi(res[0]);
		int num2 = std::stoi(res[1]);
		int num3 = std::stoi(res[2]);
		padding.x=num1;
		padding.w=num2;
		padding.y=num3;
		if(res.size() >= 4) {
			int num4 = std::stoi(res[3]);
			padding.h=num4;
		}
	}
}

Layout::Layout( Point coord, float x, float y, float W, float w, float H, float h ) {
	this->coord = coord;
	this->x = x;
	this->y = y;
	this->W = W;
	this->w = w;
	this->H = H;
	this->h = h;
	this->w_func = SizeFunction::none;
	this->h_func = SizeFunction::none;
	padding=Rect(0,0,0,0);
	absolute_coordinate_x=false;
	absolute_coordinate_y=false;
	enabled = true;
}

Layout::Layout( const Rect& rect ) : Layout(ng::Point(0,0), rect.x, rect.y, 0, 0, 0, 0) {
	w_min[0]=w_max[0]=rect.w;
	h_min[0]=h_max[0]=rect.h;
	w_min[1]=w_max[1] = 0;
	h_min[1]=h_max[1] = 0;
}

void Layout::SetFuncs(SizeFunction w_func, SizeFunction h_func) {
	this->w_func = w_func;
	this->h_func = h_func;
}

Layout::Layout() {
	enabled = true;
	coord = {0,0};
	x=0;y=0;
	W=0;w=0;
	H=0;h=0;
	absolute_coordinate_x=false;
	absolute_coordinate_y=false;
	w_func = SizeFunction::none;
	h_func = SizeFunction::none;
	w_min[0]=w_min[1]=0;
	w_max[0]=w_max[1]=0;
	h_min[0]=h_min[1]=0;
	h_max[0]=h_max[1]=0;
	padding=Rect(0,0,0,0);
}

void Layout::SetCoord( Point coord ) {
	this->coord = coord;
}

void Layout::SetPosition( float x, float y, float w, float W, float h, float H, bool absolute_coordinates ) {
	this->x = x;
	this->y = y;
	this->w = w;
	this->W = W;
	this->h = h;
	this->H = H;
	this->absolute_coordinate_x = absolute_coordinates;
	this->absolute_coordinate_y = absolute_coordinates;
}

void Layout::SetSizeRange( float min_w, float min_W, float min_h, float min_H, float max_w, float max_W, float max_h, float max_H, SizeFunction w_func, SizeFunction h_func ) {
	w_min[0]=min_w;
	w_max[0]=max_w;
	w_min[1]=min_W;
	w_max[1]=max_W;
	
	h_min[0]=min_h;
	h_max[0]=max_h;
	h_min[1]=min_H;
	h_max[1]=max_H;
	this->w_func = w_func;
	this->h_func = h_func;
}

void Layout::SetSize( float w, float W, float h, float H ) {
	w_min[0]=w_max[0]=w;
	w_min[1]=w_max[1]=W;
	h_min[0]=h_max[0]=h;
	h_min[1]=h_max[1]=H;
	w_func = SizeFunction::none;
	h_func = SizeFunction::none;
}

Layout::Layout(std::string s) : Layout() {
	*this = parseRect(s);
}

void Layout::SetEnabled(bool enabled) {
	this->enabled = enabled;
}

bool Layout::Enabled() {
	return enabled;
}

Layout Layout::parseRect(std::string s) {
	Layout a;
	std::vector<std::string> parts;
	split_string(s, parts, ',');
	const char* names[] = { "wW", "hH", "W", "H" };
	float res[4];
	int p = 0;
	if(s[0] == 'A') {
		// std::cout << "ABS\n";
		a.absolute_coordinate_x = true;
		a.absolute_coordinate_y = true;
	}
	a.w_func = a.h_func = fit;
	
	a.w_min[0] = 0;
	a.w_max[0] = 9999;
	a.w_min[1] = 0;
	a.w_max[1] = 0;
	
	a.h_min[0] = 0;
	a.h_max[0] = 9999;
	a.h_min[1] = 0;
	a.h_max[1] = 0;
					
	for(int i=0; i < parts.size(); i++) {
		const std::string& cur = parts[i];
		if(p <= 1) {
			monomial_expr_parse(cur.c_str(), names[p], res);
			// std::cout << "parsing rect(" << p << "):" << cur << " => " << res[0] << ", " << res[1] << ", " << res[2] << std::endl;
			size_t pos;
			if(p == 0) { // x part
				if((pos=cur.find_first_of("LRM")) != std::string::npos) {
					if(cur[pos] == 'L') {
						a.absolute_coordinate_x = true;
					} else if(cur[pos] == 'R') {
						a.W += 1;
						a.w += -1;
						a.absolute_coordinate_x = true;
					} else if(cur[pos] == 'M') {
						a.W += 0.5;
						a.w += -0.5;
						a.absolute_coordinate_x = true;
					}
				}
				if((pos=cur.find_first_of("ra")) != std::string::npos) {
					if(cur[pos] == 'a') {
						a.absolute_coordinate_x = true;
					} else if(cur[pos] == 'r') {
						a.absolute_coordinate_x = false;
					}
				}
				a.x += res[0];
				a.w += res[1];
				a.W += res[2];
			} else if(p == 1) { // y part
				if((pos=cur.find_first_of("TBM")) != std::string::npos) {
					if(cur[pos] == 'T') { // T - Top
						a.absolute_coordinate_y = true;
					} else if(cur[pos] == 'M') {
						a.H += 0.5;
						a.h += -0.5;
						a.absolute_coordinate_y = true;
					} else if(cur[pos] == 'B') {
						a.H += 1;
						a.h += -1;
						a.absolute_coordinate_y = true;
					}
				}
				
				if((pos=cur.find_first_of("ra")) != std::string::npos) {
					if(cur[pos] == 'a') {
						a.absolute_coordinate_y = true;
					} else if(cur[pos] == 'r') {
						a.absolute_coordinate_y = false;
					}
				}
				a.y += res[0];
				a.h += res[1];
				a.H += res[2];
			}
		} else if(p >= 2) { // w and h part
			size_t pos = 0;
			
			auto set_func = [&](Layout::SizeFunction f){
				// std::cout << "setting " << (const char*[]){"none","keep","fit","expand"}[f] << "\n";
				if(p == 2) {
					a.w_func = f;
				} else if(p == 3) {
					a.h_func = f;
				}
			};
			
			
			auto& part = parts[i];
			// TODO: keep == expand???
			if(part.find("keep", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::keep);
			} else if(part.find("fit", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::fit);
			} else if(part.find("expand", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::expand);
			} else if(part.find("fill", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::fill);
			} else if(part.find("none", pos) != std::string::npos) {
				set_func(Layout::SizeFunction::none);
			}
			
			if((pos = parts[i].find("(", pos)) != std::string::npos) {
					monomial_expr_parse(parts[i].c_str()+pos, names[p], res);
					if(i+1 >= parts.size() || parts[i].find(")", pos+1) != std::string::npos) {
						// has only min
						if(p == 2) {
							a.w_min[0] = res[0];
							a.w_min[1] += res[1];
						} else {
							a.h_min[0] = res[0];
							a.h_min[1] += res[1];
						}
						continue;
					}
					if(p == 2) {
						a.w_min[0] = res[0];
						a.w_min[1] += res[1];
					} else {
						a.h_min[0] = res[0];
						a.h_min[1] += res[1];
					}
					monomial_expr_parse(parts[i+1].c_str(), names[p], res);
					if(p == 2) {
						a.w_max[0] = res[0];
						a.w_max[1] += res[1];
					} else {
						a.h_max[0] = res[0];
						a.h_max[1] += res[1];
					}
					
					// std::cout << "parsing rect(" << parts[i+1] << "):" << cur << " => " << res[0] << ", " << res[1] << std::endl;
					i++;
			} else { // h part
				monomial_expr_parse(cur.c_str(), names[p], res);
				// std::cout << "parsing rect(" << p << "): " << cur << " => " << res[0] << ", " << res[1] << std::endl;
				if(p == 2) {
					a.w_min[0] += res[0];
					a.w_min[1] += res[1];
					a.w_max[0] = a.w_min[0];
					a.w_max[1] = a.w_min[1];
					a.w_func = Layout::SizeFunction::none;
				} else if(p == 3) {
					a.h_min[0] += res[0];
					a.h_min[1] += res[1];
					a.h_max[0] = a.h_min[0];
					a.h_max[1] = a.h_min[1];
					a.h_func = Layout::SizeFunction::none;
				}
			}
			
		}
		p++;
	}
	return a;
}

void Layout::SetAbsoluteCoords(bool abs_x, bool abs_y) {
	absolute_coordinate_x = abs_x;
	absolute_coordinate_y = abs_y;
}

Layout& Layout::operator+=(const Layout& b) {
	W += b.W;
	w += b.w;
	x += b.x;
	
	H += b.H;
	h += b.h;
	y += b.y;
	
	absolute_coordinate_x |= b.absolute_coordinate_x;
	absolute_coordinate_y |= b.absolute_coordinate_y;
	
	return *this;
}

std::ostream& operator<< (std::ostream& stream, const Layout& a) {
	return stream << 
		"X(" << a.x << ", " << a.w << ", " << a.W << ") " << 
		"Y(" << a.y << ", " << a.h << ", " << a.H << ") " <<
		"MIN(" << a.w_min[1] << "W+" << a.w_min[0] << ", " << a.h_min[1] << "H+" << a.h_min[0] << ") " << 
		"MAX(" << a.w_max[1] << "W+" << a.w_max[0] << ", " << a.h_max[1] << "H+" << a.h_max[0] << ") " <<
		"coord(" << a.coord.x << ", " << a.coord.y << ") " << 
		"ABS(" << a.absolute_coordinate_x << ", " << a.absolute_coordinate_y << ") " << 
		"func(" << a.w_func << ", " << a.h_func << ")" << std::endl;
}

}
