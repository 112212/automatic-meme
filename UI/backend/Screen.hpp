#ifndef NG_GRAPHICS_HPP
#define NG_GRAPHICS_HPP
#include "../Image.hpp"
#include <stack>
#include <string>
namespace ng {
class Screen {
	protected:
		void SetCache(Image* img, uint32_t cache_id);
		std::stack<ng::Rect> clip_stack;
		std::stack<float> maxAlpha;
		float current_alpha;
		bool using_clip_region;
	public:
		Screen();
		virtual void Init();
		
		virtual void SetResolution( int w, int h );
		virtual void GetResolution( int &w, int &h );

		virtual void PutPixel(int x, int y, uint32_t color);
		virtual void Rect(int x, int y, int w, int h, uint32_t color);
		virtual void FillRect(int x, int y, int w, int h, uint32_t color);

		virtual void FillCircle(int x, int y, float radius, uint32_t color);
		virtual void Circle(int x, int y, float radius, uint32_t color);

		virtual void Line(int xA, int yA, int xB, int yB, uint32_t color);
		virtual void HLine(int x1, int y1, int y2, uint32_t color);
		virtual void VLine(int x1, int y1, int y2, uint32_t color);

		virtual void SetMaxAlpha(float _max_alpha);
		virtual void SetRotation(float rotation, int cx=0, int cy=0);
		virtual void SetRotationPoint(int x, int y);
		
		virtual void SetClipRegion(int x, int y, int w, int h, bool enable=true);
		virtual bool GetClipRegion(int &x, int &y, int &w, int &h);
		
		void Circle(Point p, float radius, uint32_t color);
		void Line(Point a, Point b, int color);
		
		void EnableClipRegion();
		void DisableClipRegion();
		void PushClipRegion(int x, int y, int w, int h);
		void PopClipRegion();
		
		void PushMaxAlpha(float alpha);
		void PopMaxAlpha();
		
		virtual int	 SetNewOffScreenRender();
		virtual bool FreeOffScreenRender(int id);
		virtual bool SetOffScreenRender(int id);
		void PushOffScreenRender();
		void PopOffScreenRender();
		
		virtual uint32_t CompileShader(std::string vertexShader, std::string fragmentShader);
		virtual void SetShader(uint32_t shader_id);
		virtual bool IsShadersSupported();

		
		virtual void TexRect(int x, int y, int w, int h, Image* tex, bool repeat = false, int texWidth = 1, int texHeight = 1);
		virtual void TexRect(int x, int y, Image* tex, bool repeat = false);
		virtual void DeleteTexture(uint32_t textureID);
		virtual void RemoveFromCache(uint32_t cache_id);
		virtual Image* GetOffScreenTexture(int id);
};

}

#endif
