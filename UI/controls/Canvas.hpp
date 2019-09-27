#ifndef NG_CANVAS_HPP
#define NG_CANVAS_HPP

#include "../Control.hpp"
#include "../Image.hpp"
#include "../BasicImage.hpp"
namespace ng {
class Canvas : public Control {
	private:
		std::vector<BasicImage> layers;
		bool m_is_mouseDown;
		bool m_is_readonly;
		bool m_should_update_texture_sizes;
		bool m_grid_size_mode;
		Size_t<float> m_pixel_size;
		Size m_grid_size;
		void onRectChange();
		int pixel_size;
		
		int m_pixel_color;
		int m_grid_color;
		Point last_pt;
		bool m_align_to_grid;
		bool m_display_grid;
		bool maketex;
		
		void Render( Point pos, bool isSelected );
		void drawGrid(Point p);
		void alignToGrid(Point& p);
		
		void OnMouseDown( int mX, int mY, MouseButton btn );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseUp( int mX, int mY, MouseButton btn );
		void OnLostFocus();
		void OnSetStyle(std::string& style, std::string& value);
		
		void updateTextureSizes();
	public:
		Canvas();
		~Canvas();
		
		Canvas* Clone();
		
		void RefreshTexture();
		void PutPixel(int x, int y, int layer = 0);
		int GetPixel(int x, int y, int layer=0);
		int GetGridPixel(int x, int y, int layer=0);
		void SetPixelSize(int size);
		
		void AddLayers(int layers);
		void SetLayers(int layers);
		void SetBackgroundColor(int color);
		void SetAlignToGrid(bool align) { m_align_to_grid = align; }
		void DisplayGrid( bool grid ) { m_display_grid = grid; }
		void SetGridSize(Size size);
		void SetPixelColor(int color) { m_pixel_color = color; }
		BasicImage& GetLayer(int layer=0);
		void Clear(int color, int layer = 0);
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		

};

}
#endif
