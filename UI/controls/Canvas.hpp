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
		void onRectChange();
		int pixel_size;
		
		int pixel_color;
		int grid_color;
		int last_x, last_y;
		bool align_to_grid;
		bool display_grid;
		bool maketex;
		
		void Render( Point pos, bool isSelected );
		void OnMouseDown( int mX, int mY, MouseButton which_button );
		void OnMouseMove( int mX, int mY, bool mouseState );
		void OnMouseUp( int mX, int mY, MouseButton button );
		void OnLostFocus();
		void OnSetStyle(std::string& style, std::string& value);
		
		void updateTextureSizes();
	public:
		Canvas();
		~Canvas();
		
		Canvas* Clone();
		
		void RefreshTexture();
		void PutPixel(int x, int y, int layer = 0);
		void SetPixelSize(int size);
		
		void AddLayers(int layers);
		void SetLayers(int layers);
		void SetBackgroundColor(int color);
		void SetAlignToGrid(bool align) { align_to_grid = align; }
		void DisplayGrid( bool grid ) { display_grid = grid; }
		void SetPixelColor(int color) { pixel_color = color; }
		BasicImage& GetLayer(int layer=0);
		void Clear(int color, int layer = 0);
		void SetReadOnly( bool isReadOnly ) { m_is_readonly = isReadOnly; }
		

};

}
#endif
