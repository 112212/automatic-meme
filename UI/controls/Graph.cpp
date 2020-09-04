#include "Graph.hpp"
#include "../Cursor.hpp"
#include "../Gui.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include "ScrollBar.hpp"
#include "ContextMenu.hpp"
#include "CircularIterator.hpp"

namespace ng {
	
Graph::Graph() : Control(){
	setType( "graph" );
	m_frame = 0;
	
	m_hscroll = 0;
	m_vscroll = 0;
	m_cloud_label = 0;
	m_frame_color = 0;
	m_grid_color = Color::DGray;
	m_style.background_color = 0;
	m_redraw = true;
	m_value_sum = 0;
	m_follow = false;
	m_show_grid = true;
	m_show_scrollbars = false;
	m_timeline_mode = false;
	m_show_cross = true;
	m_coords_on_scales = false;
	m_coords_on_cursor = true;
	m_follow_any = false;
	m_is_rgrabbed = false;
	m_referent_signal = 0;
	setInterceptMask(imask::mouse_down | imask::mouse_up | imask::mouse_move);
	configureFrame();
	
}

Graph::~Graph() {
	
}

bool Graph::Scale::Visible() {
	return is_visible;
}
void Graph::Scale::Visible(bool visible) {
	for(auto *l : labels) {
		l->SetVisible(visible);
	}
	is_visible = visible;
}

int Graph::getNumDigits(double num) {
	return 0;
}

void Graph::configureFrame() {
	SetDivs(Scales::time, 5);
	SetDivs(Scales::value, 5);
	SetDivs(Scales::value2, 5);
	SetDivs(Scales::time2, 5);
	
	initScales();
	
	m_diff.show = false;
	
	m_time_scale_height = 30;
	m_value_scale_width = 40;
	
	int scroll_thickness = 10;
	
	m_show_scrollbars =true;
	
	scales[Scales::time].color = Color::White;
	scales[Scales::time].Visible(true);
	scales[Scales::time].is_static = false;
	scales[Scales::time].scale_type = ScaleType::linear;
	scales[Scales::time].name = "t[s]";
	scales[Scales::time].notation = number;
	scales[Scales::time].offset = 0;
	scales[Scales::time].div_size = 5;
	
	
	// 
	scales[Scales::value].is_visible = true;
	scales[Scales::value].color = Color::White;
	scales[Scales::value].offset = 0;
	scales[Scales::value].div_size = 5;
	scales[Scales::value].is_static = false;
	
	
	scales[Scales::value2].Visible(false);
	scales[Scales::value2].color = Color::White;
	scales[Scales::value2].offset = 0;
	scales[Scales::value2].div_size = 5;
	
	scales[Scales::time2].Visible(false);
	scales[Scales::time2].color = Color::White;
	scales[Scales::time2].offset = 0;
	scales[Scales::time2].div_size = 5;
	
	scales[Scales::time].drawing_vector = Point(1, 0);
	scales[Scales::value].drawing_vector = Point(0, -1);
	scales[Scales::value2].drawing_vector = Point(0, -1);
	scales[Scales::time2].drawing_vector = Point(1,0);
	
	// Label* cloud = createControl<Label>("label", "cloud");
	// cloud->SetLayout("300,100,100,30");
	// cloud->SetStyle("bordercolor", "#ffffffff");
	// cloud->SetStyle("align", "center");
	// cloud->SetText("HEYYA");
	// AddControl(cloud);
	
	// configure scrollbars
	if(m_show_scrollbars) {
		m_vscroll = createControl("scrollbar", "vscroll");
		m_vscroll->SetStyle("vertical", "true");
		m_vscroll->SetStyle("bordercolor", "#ffffff");
		m_vscroll->SetStyle("slider_size", "30");
		m_vscroll->SetStyle("range", "-1000,1000");
		m_vscroll->SetLayout(getString("AR,0,%d,H-%d", scroll_thickness,scroll_thickness));
		
		AddControl(m_vscroll);
		
		m_vscroll->OnEvent("change", [&](Args s) {
			
			SetScaleOffset(-(double)s.event_args.front().i / m_vscroll_mult.y + m_vscroll_mult.x);
		});
		
		m_hscroll = createControl("scrollbar", "hscroll");
		m_hscroll->SetStyle("vertical", "false");
		m_hscroll->SetStyle("bordercolor", "#ffffff");
		// m_hscroll->SetStyle("slidersize", "30");
		m_hscroll->SetLayout(getString("A0,B,W-%d,%d", scroll_thickness, scroll_thickness));
		m_hscroll->SetStyle("range", "0,1000");
		AddControl(m_hscroll);
		
		m_hscroll->OnEvent("change", [&](Args s) {
			SetTimeOffset((double)s.event_args.front().i/m_hscroll_mult.y + m_hscroll_mult.x);
		});
	}
	
	m_hscroll_mult = Point(0,1000);
	m_vscroll_mult = m_hscroll_mult;
	
	// TODO: cloud
	// m_cloud_label = createControl("label", "cloud");
	// m_cloud_label->SetLayout("0,0,100,30");
	
	// m_cloud_label->SetStyle("value", "heyyyyy");
	// AddControl(m_cloud_label);
	
	ProcessLayout();
}

std::string Graph::getNum(double num, double c) {
	// std::stringstream ss;
	// ss.precision(clip<int>(-log10(c), 1, 5));
	// ss << std::fixed << num;
	// return ss.str();
	
	std::string tmp = getString("%%.%dlf", 4);
	return getString(tmp.c_str(), num);
			
	double n = 20.0;
	int i;
	for(i=2; i < 5; i++) {
		if( c > n ) {
			std::string tmp = getString("%%.%dlf", i);
			return getString(tmp.c_str(), num);
		}
		n/=10.0;
	}
	
	
	
	std::string str = getString("%.2lf", num);
	// std::cout << "tt " << str << "\n";
	return str;
}

void Graph::SetSignalCircular(int signal, bool is_circular, size_t alloc_size) {
	m_signals[signal].is_circular = is_circular;
	m_signals[signal].signal.resize(alloc_size);
	m_signals[signal].circ_tail = 0;
	m_signals[signal].circ_head = 0;
}

void Graph::SetReference(int signal) {
	m_referent_signal = signal;
}

Graph::Scale::Scale() : num_divs(0) {
	
}

void Graph::initScales() {
	for(Scale& s : scales) {
		s.cursor_coord = createControl<Label>("label", "cursor_coord");
		s.cursor_coord->SetSize(100,30);
		s.cursor_coord->SetStyle("backgroundcolor", "#ff101010");
		// s.cursor_coord->SetAlignment(Alignment::center);
		AddControl(s.cursor_coord);
	}
}

std::vector<std::pair<double,double>>& Graph::GetSignal(int n) {
	return m_signals[n].signal;
}

void Graph::SetDivs(int scale, int ndivs) {
	Scale &s = scales[scale];
	int prev_divs = s.labels.size();
	s.num_divs = ndivs;
	while(prev_divs++ <= s.num_divs) {
		Label* lbl = createControl<Label>("label", "scale");
		if(!lbl) {
			break;
		}
		if(scale == 0 || scale == 3) {
			// lbl->SetAlignment(Alignment::center);
		}
		// lbl->SetTransparentBackground();
		lbl->SetLayoutEnabled(false);
		s.labels.push_back(lbl);
		AddControl(lbl);
	}
}

int Graph::CreateSignal(std::string name, uint32_t color, int y_scale, int t_scale, bool differential) {
	Signal sig;
	sig.is_circular = false;
	sig.using_differential_input = differential;
	sig.time_scale = t_scale;
	sig.value_scale = y_scale;
	sig.name = name;
	sig.color = color;
	m_signals.push_back(sig);
	return m_signals.size()-1;
}

void Graph::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Control::RenderBase(pos, isSelected);
	
	// Rect vsr(0,0,0,0);
	// Rect hsr(0,0,0,0);
	
	// if(m_vscroll) {
		// vsr = m_vscroll->GetRect();
	// }
	// if(m_hscroll) {
		// hsr = m_hscroll->GetRect();
	// }
	
		
	// draw frame
	if(m_frame) {
		if(m_redraw) {
			m_redraw = false;
			drawFrame();
		}
		
		// show frame
		Drawing().TexRect(m_display_area.x + x, m_display_area.y + y, m_display_area.w, m_display_area.h, m_frame);
		// -------- draw cursor and its coords ----------
		drawCursor(pos);
		// -----------------------
	}
	Control::RenderWidget(pos, isSelected);
}

void Graph::drawCursor(Point pos) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	if(IsSelected()) {
			Point cursor = getCursor();
			Point pt = cursor;
			cursor -= m_display_area;
			// std::cout << "pt: " << pt << "\n";
			
			// pt = pt-m_display_area;
			if(m_show_cross) {
				pt.x = clip(pt.x, m_display_area.x, m_display_area.x + m_display_area.w) + x;
				pt.y = clip(pt.y, m_display_area.y, m_display_area.y + m_display_area.h) + y;
				int x2 = m_display_area.x+m_display_area.w + x;
				int y2 = m_display_area.y+m_display_area.h + y;
				Drawing().Line(m_display_area.x+x, pt.y, x2, pt.y, Color(Color::White, 150).GetUint32());
				Drawing().Line(pt.x, m_display_area.y+y, pt.x, y2, Color(Color::White, 150).GetUint32());
			}
			
			// draw cursor coords on scales
			if(m_coords_on_scales) {
				for(int s = 0; s < 4; s++) {
					Scale& scale = scales[s];
					if(!scale.is_visible) continue;
					if(!scale.cursor_coord->IsVisible()) {
						continue;
					}
					Rect dst(scale.drawing_offset.x + cursor.x * scale.drawing_vector.x-5, 
							 scale.drawing_offset.y + (m_display_area.h - cursor.y) * scale.drawing_vector.y, 50, 20);
					// scale.cursor_coord->SetTransparentBackground();
					scale.cursor_coord->SetRect(dst);
					// if(s == 0 || s == 3) {
						// scale.cursor_coord->SetAlignment(Alignment::center);
					// }
					
					auto coord = getPointerGraphCoords( 
						s == Scales::time ? Scales::time : Scales::time2, 
						s == Scales::value ? Scales::value : Scales::value2
					);
					
					scale.cursor_coord->SetText(
						getNum((s == Scales::time || s == Scales::time2) ? coord.x : coord.y, scale.div_size));
						
					Rect cr = scale.cursor_coord->GetContentRect();
					scale.cursor_coord->SetSize(cr.w, cr.h);
				}
			}
			
			// draw coordinates on cursor
			if(m_coords_on_cursor) {
				cursor = getAbsCursor();
				auto coord = getPointerGraphCoords( Scales::time, Scales::value );
				Image* img = m_style.font->GetTextImage( "(" + getNum(coord.x, scales[Scales::time].div_size) + ", " + getNum(coord.y, scales[Scales::value].div_size) + ")", Color::White);
				Size s = img->GetImageSize();
				Drawing().TexRect(cursor.x + 10, cursor.y + 10, s.w, s.h, img);
				delete img;
			}
			// m_frame->LineAA(Point(m_display_area.x+m_display_area.w, m_display_area.y), Point(m_display_area.x+m_display_area.w, m_display_area.y+m_display_area.h), m_grid_color);
		}
}

void Graph::drawFrame() {
	m_frame->Clear(m_frame_color);
	// Size r = m_frame->GetImageSize();
	
	/*
		m_time_div
		1st -- v --- 2d 3d
	*/
	
	// draw grid
	if(m_show_grid) {
		drawGrid();
	}

	// print scales
	drawScales();
	
	// ---- draw signal -----
	if(m_timeline_mode) {
		drawTimeline();
	} else {
		drawSignal();
	}
	
	
	drawDiffScale();
	
}

void Graph::drawTimeline() {
	int n=0;
	int sig_id = 0;
	
	for(auto& sig : m_signals) {
		
		
		Scale& time_scale = scales[sig.time_scale];
		Scale& value_scale = scales[sig.value_scale];
		double first = floor(time_scale.offset / time_scale.div_size) * time_scale.div_size;
		double last = first + time_scale.num_divs * time_scale.div_size + time_scale.div_size;
		
		
		
		int cnt = 0;
		auto compare = [](SignalType &p, const double &v) -> bool {
			return p.first < v;
		};
		auto compare2 = [](const double &v, SignalType &p) -> bool {
			return v < p.first;
		};
		
		int tail = 0;
		int head = sig.signal.size()-1;
		if(sig.signal.empty()) {
			head = 0;
		}
		
		if(sig.is_circular) {
			tail = sig.circ_tail;
			head = sig.circ_head;
		} else {
			// head = sig.signal.size();
		}
		
		auto circ = make_circular_iterator(sig.signal, tail, head);
		auto it_beg = std::lower_bound(circ.begin(), circ.end(), first, compare);
		auto it_end = std::upper_bound(circ.begin(), circ.end(), last, compare2);
		
		int iter_cnt = 1;
		if(it_beg != circ.begin()) {
			it_beg -= iter_cnt;
		}
			// std::cout << "it_end: " << it_end.GetIndex() << " " << circ.end().GetIndex() << "\n";
		if(it_end != circ.end()) {
			it_end += iter_cnt;
		}
		
		int dist_diff = getScaleScreenDivSize(sig.time_scale);
		int scale_dist_diff = getScaleScreenDivSize(sig.value_scale);
		bool fst=true;
		Point prev;
		
		double interp_v = graphToScreenMetric(-sig_id*1.5 - value_scale.offset, sig.value_scale);
		
		// std::cout << "drw rect: " << sig.name << "\n";
		// std::cout << "plt: " << it_end->first<< "\n";
		for(auto it = it_beg; it != it_end; ++it, ++cnt) {
			double& t = it->first;
			double& v = it->second;
			// std::cout << "plt: " << t << " : " << v << "\n";
			// double to_screen = scale_dist_diff/value_scale.div_size;
			double interp_t = graphToScreenMetric(t - time_scale.offset, sig.time_scale);
			
			int x2 = graphToScreenMetric(t + v - time_scale.offset, sig.time_scale);
			Point pt(interp_t, m_display_area.h - interp_v);
			int dif = graphToScreenMetric(0.5, sig.value_scale);
			Point a{pt.x, pt.y-dif}, b{x2, pt.y+dif};
			// std::cout << "R: " << a << b << "\n";
			m_frame->DrawRect(a, b, sig.color, true);
			
		}
		// draw labels
		Image* img = m_style.font->GetTextImage(sig.name, Color::White);
		Size s = img->GetImageSize();
		int pty = m_display_area.h - interp_v;
		m_frame->PutImage(img, Rect(0, pty - s.h/2, s.w,s.h), Rect(0,0,s.w,s.h), 0x00000000, 0xff000000 );
		delete img;
		sig_id++;
	}
}

void Graph::drawGrid() {
	Scale &ts = scales[Scales::time];
	double first = ts.offset;
	int dist_diff = getScaleScreenDivSize(Scales::time);
	int dist = 0;
	
	Scale &vs = scales[Scales::value];
	double val_first = vs.offset;
	int val_dist_diff = getScaleScreenDivSize(Scales::value);
	int val_dist = 0;
	
	if(!ts.is_static) {
		first = floor(ts.offset / ts.div_size) * ts.div_size;
		dist = dist_diff * (ts.div_size - (ts.offset - first)) / ts.div_size;
	}
	
	// vertical lines
	for(int i=0; i < ts.num_divs; i++) {
		int x = dist + dist_diff*i;
		m_frame->LineAA(Point(x, 0), Point(x, m_display_area.h), m_grid_color);
	}
	
	if(!vs.is_static) {
		val_first = floor(vs.offset / vs.div_size) * vs.div_size;
		val_dist = (val_dist_diff * (vs.offset - val_first) / vs.div_size);
	}
	
	// horizontal lines
	for(int i=0; i < vs.num_divs; i++) {
		int y = val_dist + val_dist_diff*i;
		m_frame->LineAA(Point(0, y), Point(m_display_area.w, y), m_grid_color);
	}
	// m_frame->LineAA(Point(0, 0), Point(0, m_display_area.h), m_grid_color);
	// m_frame->LineAA(Point(0, m_display_area.h), Point(m_display_area.w, m_display_area.h), m_grid_color);
	// m_frame->LineAA(Point(m_display_area.w, 0), Point(m_display_area.w, m_display_area.h), m_grid_color);
}

void Graph::drawScales() {
	for(int s = 0; s < 4; s++) {
		Scale &scale = scales[s];
		if(!scale.is_visible) continue;
		// std::cout << "drawing scale: " << s << "\n";
		double first = scale.offset; 
		int dist_diff = getScaleScreenDivSize(s);
		int dist = 0;
		
		if(!scale.is_static) {
			first = floor(scale.offset / scale.div_size) * scale.div_size;
			dist = dist_diff * (scale.div_size - (scale.offset - first)) / scale.div_size - dist_diff;
		}
		// std::cout << "pr scale: " << scale << " : " << scale.num_divs << "\n";
		// ------------- print scales ------------
		for(int i=0; i < scale.num_divs+1; i++) {
			double t = first + i*scale.div_size;
			std::string num = getNum(t, scale.div_size);
			
			Rect dst(scale.drawing_offset.x + dist * scale.drawing_vector.x - 5, 
					 scale.drawing_offset.y + dist * scale.drawing_vector.y, 50, 20);
					 
			Label* l = scale.labels[i];
			
			l->SetText(num);
			l->SetRect(dst);
			
			Rect cr = l->GetContentRect();
			// l->SetSize(cr.w, cr.h); // TODO: this breaks rendering of scales
			
			
			dist += dist_diff;
		}
		// -------------------------------------
	}
}

int skp = 3;

void Graph::SetSkip(int _skp) {
	skp = _skp;
}

void Graph::drawSignal() {
	int n=0;
	int sig_id = 0;
	
	for(auto& sig : m_signals) {
		
		// std::cout << "NEW DRAW " << m_frame->GetImageSize() << "\n";
		// std::cout << "start\n";
		Scale& time_scale = scales[sig.time_scale];
		Scale& value_scale = scales[sig.value_scale];
		double first = floor(time_scale.offset / time_scale.div_size) * time_scale.div_size;
		double last = first + time_scale.num_divs * time_scale.div_size + time_scale.div_size;
		
		// TODO: optimize for big zoomout, huge data
		
		// if(len > 100) skp = std::max(2, 1000-len);
		// int skp_l = 0;
		
		int cnt = 0;
		auto compare = [](SignalType &p, const double &v) -> bool {
			return p.first < v;
		};
		auto compare2 = [](const double &v, SignalType &p) -> bool {
			return v < p.first;
		};
		
		int tail = 0;
		int head = sig.signal.size()-1;
		if(sig.signal.empty()) {
			head = 0;
		}
		
		if(sig.is_circular) {
			tail = sig.circ_tail;
			head = sig.circ_head;
		}
		
		auto circ = make_circular_iterator(sig.signal, tail, head);
		auto it_beg = std::lower_bound(circ.begin(), circ.end(), first, compare);
		auto it_end = std::upper_bound(circ.begin(), circ.end(), last, compare2);
		// std::cout << "tail: " << tail << ", head: " << head <<"\n";
		// std::cout << it_beg.GetIndex() << ", " << it_end.GetIndex() << " => " << it_beg.GetHead() << ", " << it_end.GetHead() << " : " << (it_beg < it_end) << "\n";
		int iter_cnt = 1;
		
		if(it_beg != circ.begin()) {
			// it_beg -= iter_cnt;
			it_beg --;
		}
			// std::cout << "it_end: " << it_end.GetIndex() << " " << circ.end().GetIndex() << "\n";
		if(it_end != circ.end()) {
			// it_end += iter_cnt;
			it_end ++;
		}
		
		
		
		
		int dist_diff = getScaleScreenDivSize(sig.time_scale);
		int scale_dist_diff = getScaleScreenDivSize(sig.value_scale);
		bool fst=true;
		Point prev;
		
		for(auto it = it_beg; it < it_end; ++it, ++cnt) {
			double& t = it->first;
			double& v = it->second;
			double interp_t = (t - time_scale.offset) * dist_diff/time_scale.div_size;
			double interp_v = (v - value_scale.offset) * scale_dist_diff/value_scale.div_size;
			
			// std::cout << "plotting\n";
			// TODO: optimize for big zoomout, huge data
			// if(skp_l > 0) {
				// skp_l--;
				// continue;
			// }
			// if(skp>0 && (cnt >= skp)) {
				// cnt = 0;
				// skp_l = 1;
				// continue;
			// }
			
			if(cnt < skp) {
				continue;
			} else {
				cnt = 0;
			}
			
			Point pt(interp_t, m_display_area.h - interp_v);
			if(fst) {
				fst = false;
				m_frame->Pixel(pt, sig.color);
			} else {
				// m_frame->Pixel(pt, sig.color);
				m_frame->Line(prev, pt, sig.color);
			}
			prev = pt;
		}
		
		// std::cout << "rendering " << sig_id << ": " << cnt << "\n";
		sig_id++;
	}
}

Range_t<double> Graph::getRangeOfVisibleValues(int scale) {
	Scale& sc = scales[scale];
	return {sc.offset, sc.offset + sc.div_size*sc.num_divs};
}

void Graph::AddPoints(int sig_idx, const std::vector<SignalType> &signal) {
	if(sig_idx >= m_signals.size()) return;

	auto& s = m_signals[sig_idx];
	auto& sig = s.signal;
	
	std::vector<SignalType>::const_iterator it,it_end;
	auto back = [&]() {
		if(s.is_circular) {
			int cp = s.circ_head-1;
			if(cp < 0) {
				cp = sig.size()-1;
			}
			return sig.empty() ? SignalType(0,0) : sig[cp];
		} else {
			return sig.size() < 1 ? SignalType(0,0) : sig[sig.size()-2];
		}
	};
	
	auto front = [&](){
		if(s.is_circular) {
			return sig.empty() ? SignalType(0,0) : sig[s.circ_tail];
		} else {
			return sig.empty() ? SignalType(0,0) : sig.front();
		}
	};
	
	
	double old_offs = back().first;
	
	std::vector<SignalType> signal_copy(0);
	if(s.using_differential_input) {
		double accum = sig.empty() ? 0 : back().first;
		signal_copy.assign(signal.begin(), signal.end());
		for(auto& sig2 : signal_copy) {
			accum += sig2.first;
			sig2.first = accum;
		}
		it = signal_copy.cbegin();
		it_end = signal_copy.cend();
	} else {
		it = signal.cbegin();
		it_end = signal.cend();
	}
	
	if(s.is_circular) {
		auto circ = make_circular_iterator(sig, s.circ_tail, s.circ_head);
		circ.insert(it, it_end);
		s.circ_tail = circ.begin().GetIndex();
		s.circ_head = circ.end().GetIndex();
	} else {
		// std::cout << "non circ\n";
		if(!sig.empty())
		sig.pop_back();
		sig.insert(sig.end(), it, it_end);
		sig.push_back({0,0});
	}
	
	/*
	if(signal.front().first > scales[0].offset+scales[0].div_size*(scales[0].num_divs) || signal.back().first < scales[0].offset) {
		// don't redraw, added points are outside of visible space
	} else {
		m_redraw = true;
	}
	*/
	
	if(m_follow_any || ((m_follow && m_referent_signal == sig_idx) && sig.size() >= 2)) {
		ScrollBar* sb = (ScrollBar*)m_hscroll;
		
		double new_offs = back().first;
		
		if(!(sb && sb->IsDraggingSlider())) {
			SetScaleOffset(scales[Scales::time].offset + (new_offs - old_offs));
		}
		
		/*
		double dispersion = 0;
		auto range = getRangeOfVisibleValues();
		if(signal.front().second < range.x || signal.front().second > range.y) {
			double avg = 0;
			for(auto &v : signal) {
				// avg += v;
				m_value_viewed[0].min = std::min(m_value_viewed[0].min, v.second);
				m_value_viewed[0].max = std::max(m_value_viewed[0].max, v.second);
				
				m_value_sum += v.second;
			}
			avg = m_value_sum / m_value.size();
			
			// scale vertically by dispersion
			double d = (m_value_viewed[0].max-m_value_viewed[0].min) * 3;
			// SetScaleSize(d);
			// center vertically
			SetScaleOffset(avg - (m_value_div_count[0]*0.1)*m_limit_value[0].y);
			std::cout << "avg: " << avg << " d: " << d << "\n";
		}
		*/
		
		if(m_hscroll) {
			Range rng = sb->GetRange();
			// sb->SetRange(std::min(rng.min, (int)front().first), 
						 // std::max(rng.max, (int)back().first));
			// std::cout << (int)(front().first*m_hscroll_mult.y) << " " << (int)(back().first*m_hscroll_mult.y) << "\n";
			m_hscroll_mult.x = front().first*m_hscroll_mult.y;
			sb->SetRange((int)(0), 
						 (int)(back().first*m_hscroll_mult.y) - m_hscroll_mult.x);
			// std::cout << "range: " << scales[Scales::time].div_size*scales[Scales::time].num_divs*m_hscroll_mult.y << "\n";
			sb->SetVisibleRange(scales[Scales::time].div_size*scales[Scales::time].num_divs*m_hscroll_mult.y);
			if(!sb->IsDraggingSlider()) {
				sb->SetValue(scales[Scales::time].offset*m_hscroll_mult.y - m_hscroll_mult.x);
			}
		}
		
	}
	m_redraw = true;
}


void Graph::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("grid"):
			m_show_grid = toBool(value);
		_case("grid_color"):
			m_grid_color = Color(value).GetUint32();
		_case("signal_color"):
			// m_value_color[0] = Color(value).GetUint32();
		_case("scale1_color"):
			// m_scale_color[0] = Color(value).GetUint32();
		_case("follow"):
			m_follow = toBool(value);
		_case("framecolor"):
			m_frame_color = Color(value).GetUint32();
		_case("timeline"):
			m_timeline_mode = toBool(value);
			scales[Scales::value].Visible(false);
			updateDisplayArea();
		_case("cross"):
			m_show_cross = toBool(value);
		_case("cursorcoords"):
			m_coords_on_cursor = toBool(value);
		_case("scalescoords"):
			m_coords_on_scales = toBool(value);
		_case("followany"):
			m_follow_any = toBool(value);
		
	}
}

void Graph::updateDisplayArea() {
	const Rect &r = GetRect();
	Rect vsr(0,0,0,0);
	Rect hsr(0,0,0,0);
	if(m_vscroll) {
		vsr = m_vscroll->GetRect();
	}
	if(m_hscroll) {
		hsr = m_hscroll->GetRect();
	}
	
	Point ofs(30,20);
	Point p2;
	if(!scales[Scales::value].is_visible) {
		ofs.x = 0;
	}
	if(!scales[Scales::time].is_visible) {
		ofs.y = 0;
	}
	if(scales[Scales::time2].is_visible) {
		p2.y = 20;
	}
	if(scales[Scales::value2].is_visible) {
		p2.x = 30;
	}
	m_display_area = Rect(ofs.x, 
						  p2.y, 
						  r.w - vsr.w - ofs.x - p2.x,
						  r.h - hsr.h - ofs.y - p2.y);
}
void Graph::updateTextureSizes() {
	updateDisplayArea();
	// std::cout << m_display_area << " vs " << GetRect() << "\n";
	// std::cout <<(vsr.w) << "\n";
	
	if(m_frame) {
		// std::cout << "resizing: " << r << "\n";
		m_frame->Resize(m_display_area.w, m_display_area.h);
	} else {
		// std::cout << "new: " << r << "\n";
		m_frame = new BasicImage(m_display_area.w, m_display_area.h);
	}
}


Graph* Graph::Clone() {
	Graph* c = new Graph();
	copyStyle(c);
	return c;
}

int Graph::getScaleScreenDivSize(int scale) {
	return std::max(1, (scales[scale].drawing_vector.x == 0 ? m_display_area.h : m_display_area.w) / scales[scale].num_divs);
}

double Graph::screenToGraphMetric(int screen_length, int scale) {
	return screen_length * scales[scale].div_size / getScaleScreenDivSize(scale);
}

int Graph::graphToScreenMetric(double graph_pt, int scale) {
	return graph_pt * getScaleScreenDivSize(scale) / scales[scale].div_size;
}

Point_t<double> Graph::ScreenToGraphCoords(Point screen_coords, int time_scale, int value_scale) {
	return Point_t<double>(
		scales[time_scale].offset + screenToGraphMetric(screen_coords.x, time_scale), 
		scales[value_scale].offset + screenToGraphMetric(screen_coords.y, value_scale));
}

Point_t<double> Graph::getPointerGraphCoords(int time_scale, int value_scale) {
	Point cursor = getCursor();
	return ScreenToGraphCoords( Point(cursor.x - m_display_area.x, m_display_area.h - (cursor.y - m_display_area.y)), time_scale, value_scale);
}

void Graph::OffsetLatest() {
	auto &sig = m_signals[ m_referent_signal ].signal;
	if(sig.size() < 2) return;
	SetScaleOffset( sig[ sig.size()-2 ].first - screenToGraphMetric(m_display_area.w/2, 0)  );
}

void Graph::drawDiffScale() {
	// int x = graphToScreenMetric(m_diff.diff_time.x, Graph::time);
	// int dx = graphToScreenMetric(m_diff.diff_time.y, Graph::time);
	// int y = graphToScreenMetric(m_diff.diff_val, Graph::value);
	if(!m_diff.show) return;
	// m_frame->Line(Point(x,y), Point(x+dx, y), Color::White);
	int diff = m_diff.diff_time.y - m_diff.diff_time.x;
	int rng = 30;
	
	if(m_is_rgrabbed) {
		rng = 1;
	}
	
	for(int i=-rng+1; i <= rng; i++) {
		m_frame->Line(Point(m_diff.diff_time.x, m_diff.diff_val), Point(m_diff.diff_time.x + diff*i, m_diff.diff_val), Color::White);
		m_frame->Line(Point(m_diff.diff_time.x + diff*i, m_diff.diff_val-10), Point(m_diff.diff_time.x + diff*i, m_diff.diff_val+10), Color::White);
	}
}

void Graph::OnMouseMove( int x, int y, bool mouseState ) {
	// !isIntercepted() && 
	if(mouseState && m_is_grabbed && !m_is_rgrabbed) {
		
		Point p = getCursor();
		Point delta = m_grab_control_point - p;
		m_grab_control_point = Point(x,y);
		double new_x_offset = (scales[Scales::time].offset + screenToGraphMetric(delta.x,0));
		SetTimeOffset( new_x_offset );
		for(int scale : {Scales::value, Scales::value2}) {
			double new_y_offset = (scales[scale].offset - screenToGraphMetric(delta.y,scale));
			SetScaleOffset( new_y_offset, scale );
		}
	}
	
	if(m_is_rgrabbed) {
		// TODO: draw diff scale
		Point pt(x,y);
		// m_diff.diff_time.y = x - m_mdown_pt.x;
		
		// auto p1 = ScreenToGraphCoords(m_mdown_pt, Scales::time, Scales::value);
		// auto p2 = ScreenToGraphCoords(pt, Scales::time, Scales::value);
		
		// m_diff.diff_time.x = p1.x;
		// m_diff.diff_time.y = p2.x - p1.x;
		
		m_diff.diff_time.x = m_mdown_pt.x - m_display_area.x;
		m_diff.diff_time.y = pt.x - m_display_area.x;
		m_diff.diff_val = m_mdown_pt.y;
		m_diff.show = true;
		m_redraw = true;
	}
	// std::cout << "PT: " << ScreenToGraphCoords( Point(mX - m_display_area.x, m_display_area.h - (mY - m_display_area.y)), Scales::time, Scales::value) << "\n";
}

void Graph::OnMWheel( int updown ) {
	Point p = getCursor();
	
	p.x -= m_display_area.x;
	p.y -= m_display_area.y;
	
	double mult=3;
	if(!GetSystem().GetKeystate(KEYBOARD_SCANCODE_LSHIFT)) {
		
		int screen_x = p.x;
		double virtual_x = scales[Scales::time].offset + screenToGraphMetric(screen_x, Scales::time);
		
		double new_x_scale = scales[Scales::time].div_size;
		if(updown == 1) {
			if(new_x_scale/mult > 0) {
				new_x_scale /= mult;
			}
		} else {
			if(new_x_scale*mult > 0) {
				new_x_scale *= mult;
			}
		}
		SetScaleDiv( new_x_scale, Scales::time );
		
		SetTimeOffset( virtual_x - screenToGraphMetric(screen_x, Scales::time) );
	} else {
		mult=2;
		for(int scale : {Scales::value, Scales::value2}) {
				
			int screen_y = m_display_area.h - p.y;
			double virtual_y = scales[scale].offset + screenToGraphMetric(screen_y, scale);
			
			double new_y_scale = scales[scale].div_size;
			if(updown == 1) {
				if(new_y_scale/mult > 0) {
					new_y_scale /= mult;
				}
			} else {
				if(new_y_scale*mult > 0) {
					new_y_scale *= mult;
				}
			}
			SetScaleDiv( new_y_scale, scale );
			
			SetScaleOffset( virtual_y - screenToGraphMetric(screen_y, scale), scale );
			
		}
	}
	
}

int Graph::getTimeDivSize() {
	return m_display_area.w / scales[Scales::time].num_divs;
}



void Graph::SetTimeOffset(double offset) {
	scales[Scales::time].offset = offset;
	if(m_hscroll) {
		// m_hscroll->SetStyle("value", getString("%d", 10*(int)scales[0].offset));
	}
	m_redraw = true;
}

void Graph::SetTimeDiv(double div) {
	if(div == 0) {
		return;
	}
	scales[Scales::time].div_size = div;
	m_redraw = true;
}

void Graph::SetScaleOffset(double offset, int scale) {
	if(scale >= 4) return;
	scales[scale].offset = offset;
	m_redraw = true;
}

void Graph::SetScaleDiv(double div, int scale) {
	if(div == 0) {
		return;
	}
	scales[scale].div_size = div;
	m_redraw = true;
}

void Graph::SetScaleSize(double size, int scale) {
	SetScaleDiv(size/scales[scale].num_divs, scale);
}

void Graph::SetScaleStatic(int scale, bool is_static) {
	scales[scale].is_static = is_static;
}

void Graph::OnMouseUp( int x, int y, MouseButton which_button ) {
	// m_is_mouseDown = false;
	// last_x = last_y = -1;
	if(m_is_grabbed) {
		m_is_grabbed = false;
	}
	
	if(which_button == MouseButton::BUTTON_RIGHT && Point(x, y) == m_mdown_pt) {
		showContextMenu();
	}
	
	m_is_rgrabbed = false;
	m_redraw = true;
}

void Graph::OnClick(int x, int y, MouseButton btn) {
	
}

void Graph::showContextMenu() {
	std::vector<MenuItem> mi;
	for(auto s : m_signals) {
		mi.push_back( {s.name} );
	}
	ContextMenu* cm = new ContextMenu({
		{"fit all"},
		{"follow"},
		{"get last"},
		{"set reference signal", mi}
	});
	cm->OnEvent("action", [&](Args& a) {
		auto act = a.event_args[0].s;
		std::cout << "action: " << a.event_args[0].s << "\n";
		if(act == "follow") {
			m_follow = !m_follow;
		} else if(act == "fit all") {
			
		}
		// getEngine()->RemoveControl(cm);
	});
	getEngine()->AddControl(cm);
	cm->Show(this);
}

bool Graph::isInsideFrame(Point pt) {
	return pt.x >= m_display_area.x && pt.x < m_display_area.x+m_display_area.w && 
		   pt.y >= m_display_area.y && pt.y < m_display_area.y+m_display_area.h;
}

void Graph::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	m_mdown_pt = {mX,mY};
	m_redraw = true;
	if(isIntercepted()) {
		m_is_grabbed = true;
		// std::cout << "mdown is intercepted\n";
	} else {
		if(which_button == MouseButton::BUTTON_LEFT) {
			m_is_grabbed = true;
		}
		m_grab_control_point = m_mdown_pt;
	}
	if(which_button == MouseButton::BUTTON_RIGHT) {
		if(isInsideFrame(m_mdown_pt)) {
			m_is_rgrabbed = true;
		}
	}
}

void Graph::OnLostFocus() {
	// m_is_mouseDown = false;
}

void Graph::onRectChange() {
	
	updateTextureSizes();
	int scroll_thickness = 10;
	
	scales[Scales::time].drawing_offset = Point(m_display_area.x, m_display_area.y + m_display_area.h);
	scales[Scales::value].drawing_offset = Point(5, m_display_area.y + m_display_area.h-10);
	scales[Scales::value2].drawing_offset = Point(m_display_area.x+m_display_area.w, m_display_area.y + m_display_area.h-10);
	scales[Scales::time2].drawing_offset = Point(m_display_area.x, 0);
}

}
