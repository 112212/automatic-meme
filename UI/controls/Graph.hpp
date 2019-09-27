#ifndef NG_GRAPH_HPP
#define NG_GRAPH_HPP

#include "../Control.hpp"
#include "Label.hpp"
#include "../Image.hpp"
#include "../BasicImage.hpp"
#include <array>
namespace ng {


class Graph : public Control {
	private:
		BasicImage* m_frame;
		uint32_t m_frame_color;
		bool m_is_mouseDown;
		bool m_show_grid;
		bool m_has_multiscale;
		bool m_redraw;
		bool m_is_grabbed;
		bool m_sync_time_scales;
		bool m_sync_value_scales;
		bool m_show_scrollbars;
		bool m_show_cross;
		bool m_timeline_mode;
		bool m_coords_on_scales;
		bool m_coords_on_cursor;
		
		enum FollowMode {
			no_follow,
			follow,
			trigger,
			any,
		};
		
		// no_follow, follow, trigger, ANY
		int m_follow_mode;
		bool m_follow;
		// follow reference or any signal
		bool m_follow_any;
		
		Point m_grab_control_point;
		Point m_mdown_pt;
		
		int m_time_scale_height;
		int m_value_scale_width;
		
		bool m_is_rgrabbed;
		struct Diff {
			Point_t<double> diff_time;
			double diff_val;
			bool show;
		};
		Diff m_diff;
		
		
		int m_referent_signal;
		
		Point m_hscroll_mult;
		Point m_vscroll_mult;
		
		Rect m_display_area;
		
		struct Signal {
			std::string name;
			uint32_t color;
			int time_scale;
			int value_scale;
			// buffer, possibly circular
			std::vector<std::pair<double,double>> signal;
			bool using_differential_input;
			bool is_circular;
			int circ_tail;
			int circ_head;
		};
		
		// std::function<std::pair<>
		std::vector<Signal> m_signals;
		
		enum ScaleType {
			linear,
			logarithmic
		};
		
		enum Notation {
			number,
			scientific,
			engineering
		};
		
		struct Scale {
			Scale();
			std::string name;
			std::string postfix;
			bool is_static;
			bool is_visible;
			ScaleType scale_type;
			uint32_t color;
			Notation notation;
			Range_t<double> viewed;
			Point drawing_offset;
			Point drawing_vector;
			double offset;
			double div_size;
			int num_divs;
			Label* cursor_coord;
			std::vector<Label*> labels;
			
			bool Visible();
			void Visible(bool visible);
		};
		
		std::array<Scale,4> scales;
		
		double m_value_sum;
		
		Control* m_hscroll;
		Control* m_vscroll;
		Control* m_cloud_label;
		Control* m_context_menu;
		
		uint32_t m_grid_color;
		
		bool isInsideFrame(Point pt);
		void showContextMenu();
		void initScales();
		int getNumDigits(double num);
		void configureFrame();
		void drawCursor(Point pos);
		void drawFrame();
		void drawScales();
		void drawGrid();
		void drawSignal();
		void drawTimeline();
		void drawDiffScale();
		void onRectChange();
		void OnLostFocus();
		void updateDisplayArea();
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void OnMWheel( int updown );
		void OnSetStyle(std::string& style, std::string& value);
		void OnClick(int x, int y, MouseButton btn);
		std::string getNum(double num, double c);
		
		void updateTextureSizes();
		Range_t<double> getRangeOfVisibleValues(int which_scale = 0);
		int getTimeDivSize();
		int getScaleScreenDivSize(int which_scale=0);
		double screenToGraphMetric(int screen_length, int scale);
		int graphToScreenMetric(double graph_pt, int scale);
		Point_t<double> getPointerGraphCoords(int time_scale, int value_scale);
	public:
		Graph();
		~Graph();
		
		enum Scales {
			time,
			value,
			value2,
			time2
		};
		
		using SignalType = std::pair<double,double>;
		
		Graph* Clone();
		
		
		void SetTimeOffset(double offset);
		void SetTimeDiv(double div);
		void SetTimeSize(double size);
		
		void SetScaleOffset(double offset, int which_scale = 0);
		void SetScaleDiv(double div, int which_scale = 0);
		void SetScaleSize(double size, int which_scale = 0);
		void SetScaleStatic(int scale, bool is_static);
		
		void SetDivs(int scale, int ndivs);
		
		void OffsetLatest();
		
		Point_t<double> ScreenToGraphCoords(Point screen_coords, int time_scale, int value_scale);
		
		int CreateSignal(std::string name, uint32_t color=Color::White, int y_scale=1, int t_scale=0, bool differential = false);
		void SetSignalCircular(int signal, bool is_circular=true, size_t alloc_size=100);
		void SetReference(int signal);
		void AddPoints(int sig_idx, const std::vector<SignalType> &signal);
		std::vector<std::pair<double,double>>& GetSignal(int n);
		
		void SetSkip(int skp);
};

}
#endif
