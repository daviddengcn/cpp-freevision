// Canvas.h: interface for the TCanvas class.
// by David, 2002.4
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_CANVAS_H_)
#define _CANVAS_H_

#include "Persistent.h"

//Colors
const int clScrollBar			= COLOR_SCROLLBAR | 0x80000000;
const int clBackground			= COLOR_BACKGROUND | 0x80000000;
const int clActiveCaption		= COLOR_ACTIVECAPTION | 0x80000000;
const int clInactiveCaption		= COLOR_INACTIVECAPTION | 0x80000000;
const int clMenu				= COLOR_MENU | 0x80000000;
const int clWindow				= COLOR_WINDOW | 0x80000000;
const int clWindowFrame			= COLOR_WINDOWFRAME | 0x80000000;
const int clMenuText			= COLOR_MENUTEXT | 0x80000000;
const int clWindowText			= COLOR_WINDOWTEXT | 0x80000000;
const int clCaptionText			= COLOR_CAPTIONTEXT | 0x80000000;
const int clActiveBorder		= COLOR_ACTIVEBORDER | 0x80000000;
const int clInactiveBorder		= COLOR_INACTIVEBORDER | 0x80000000;
const int clAppWorkSpace		= COLOR_APPWORKSPACE | 0x80000000;
const int clHighlight			= COLOR_HIGHLIGHT | 0x80000000;
const int clHighlightText		= COLOR_HIGHLIGHTTEXT | 0x80000000;
const int clBtnFace				= COLOR_BTNFACE | 0x80000000;
const int clBtnShadow			= COLOR_BTNSHADOW | 0x80000000;
const int clGrayText			= COLOR_GRAYTEXT | 0x80000000;
const int clBtnText				= COLOR_BTNTEXT | 0x80000000;
const int clInactiveCaptionText	= COLOR_INACTIVECAPTIONTEXT | 0x80000000;
const int clBtnHighlight		= COLOR_BTNHIGHLIGHT | 0x80000000;
const int cl3DDkShadow			= COLOR_3DDKSHADOW | 0x80000000;
const int cl3DLight				= COLOR_3DLIGHT | 0x80000000;
const int clInfoText			= COLOR_INFOTEXT | 0x80000000;
const int clInfoBk				= COLOR_INFOBK | 0x80000000;

const int clBlack	= 0x000000;
const int clMaroon	= 0x000080;
const int clGreen	= 0x008000;
const int clOlive	= 0x008080;
const int clNavy	= 0x800000;
const int clPurple	= 0x800080;
const int clTeal	= 0x808000;
const int clGray	= 0x808080;
const int clSilver	= 0xC0C0C0;
const int clRed		= 0x0000FF;
const int clLime	= 0x00FF00;
const int clYellow	= 0x00FFFF;
const int clBlue	= 0xFF0000;
const int clFuchsia	= 0xFF00FF;
const int clAqua	= 0xFFFF00;
const int clLtGray	= 0xC0C0C0;
const int clDkGray	= 0x808080;
const int clWhite	= 0xFFFFFF;
const int clNone	= 0x1FFFFFFF;
const int clDefault	= 0x20000000;

class TCanvas;


//Events
typedef void(CObject::*TNotifyFunc)(CObject*);

typedef struct{
	CObject* self;
	TNotifyFunc func;
}TNotifyEvent;

#define PERFORMEVENT(e) ((e).self->*((e).func))

//Set
#define SET_EMPTY	(0)
#define SET_IN(set,elem)	(((set)&(elem))!=0)
#define SET_EXCLUDE(set,elem) (set)&=(~elem)
#define SET_INCLUDE(set,elem) (set)|=(elem)
#define SET_PLUS(set1,set2) ((set1)|(set2))
#define SET_MINUS(set1,set2) ((set1)&(~(set2)))

/* Resource */
//Resource data
typedef struct{
	HANDLE Handle;
}TResData, *PResData;

//Font data
typedef enum{
	fpDefault, fpVariable, fpFixed
}TFontPitch;

typedef char TFontDataName[LF_FACESIZE];

typedef struct{
	HFONT	Handle;
	int	Height;
    TFontPitch	Pitch;
    int	Style;
    BYTE	CharSet;
    TFontDataName	Name;
}TFontData;

//Pen data
typedef enum{
	psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame
}TPenStyle;

typedef struct{
    HPEN	Handle ;
    int	Color;
    int	Width;
    TPenStyle	Style;
}TPenData;

//Brush data
typedef enum{
	bsSolid, bsClear, bsHorizontal, bsVertical, bsFDiagonal, bsBDiagonal, bsCross, bsDiagCross
}TBrushStyle;

typedef struct{
    HBRUSH	Handle;
    int	Color;
    HBITMAP	Bitmap;
    TBrushStyle	Style;
}TBrushData;

typedef struct tagResource{
	tagResource*	Next;
	int	RefCount;
	HANDLE	Handle;
	WORD	HashCode;
	union{
		TResData	Data;
		TFontData	Font;
		TPenData	Pen;
		TBrushData	Brush;
	};
}TResource, *PResource;

class TGraphicsObject : public TPersistent  
{
private:
	TNotifyEvent m_OnChange;
	PResource m_Resource;

	friend class TBrush;
	friend class TFont;
	friend class TPen;
	friend class TResourceManager;
public:
	void SetOnChange(CObject* self, TNotifyFunc func);

	TGraphicsObject();
	virtual ~TGraphicsObject();
	DECLARE_DYNAMIC(TGraphicsObject);

protected:
	virtual void Changed();
};

//Brush
class TBrush : public TGraphicsObject
{
public:
	//Color property
	int GetColor();
	void SetColor(int value);

	//Style property
	TBrushStyle GetStyle();
	void SetStyle(TBrushStyle value);

	//Handle Property
	HBRUSH GetHandle();
	void SetHandle(HBRUSH value);

	//Bitmap Property
	HBITMAP GetBitmap();
	void SetBitmap(HBITMAP value);

	virtual void Assign(TPersistent* Source);

	TBrush();
	virtual ~TBrush();

	DECLARE_DYNAMIC(TBrush)

private:
	void SetData(const TBrushData& BrushData);
	void GetData(TBrushData& BrushData);
};

//Font
const int fsBold	= 1;
const int fsItalic	= 2;
const int fsUnderline	= 4;
const int fsStrikeOut	= 8;

class TFont : public TGraphicsObject
{
private:
	void SetData(const TFontData& FontData);
	void GetData(TFontData& FontData);

	int	m_Color;
	int m_PixelsPerInch;
public:
	virtual void Assign(TPersistent* Source);

	//PixelsPerInch property
	int GetPixelsPerInch();
	void SetPixelsPerInch(int value);

	//CharSet property
	BYTE GetCharSet();
	void SetCharSet(BYTE value);

	//Style property
	int GetStyle();
	void SetStyle(int value);

	//Size property
	int GetSize();
	void SetSize(int value);

	//Pitch property
	TFontPitch GetPitch();
	void SetPitch(TFontPitch value);

	//Name property
	const char* GetName();
	void SetName(const char* value);

	//Height property
	int GetHeight();
	void SetHeight(int value);

	//Handle property
	HFONT GetHandle();
	void SetHandle(HFONT value);

	//Color property
	int GetColor();
	void SetColor(int value);

	TFont();
	virtual ~TFont();

	DECLARE_DYNAMIC(TFont)

protected:
	virtual void Changed();
};

//Pen
typedef enum {
	pmBlack, pmWhite, pmNop, pmNot, pmCopy, pmNotCopy,
    pmMergePenNot, pmMaskPenNot, pmMergeNotPen, pmMaskNotPen, pmMerge,
    pmNotMerge, pmMask, pmNotMask, pmXor, pmNotXor
}TPenMode;

class TPen : public TGraphicsObject
{
private:
	TPenMode m_Mode;

	void GetData(TPenData& PenData);
	void SetData(const TPenData& PenData);
public:
	virtual void Assign(TPersistent* Source);
	//Width property
	int GetWidth();
	void SetWidth(int value);

	//Style property
	TPenStyle GetStyle();
	void SetStyle(TPenStyle value);

	//Mode property
	void SetMode(TPenMode value);
	TPenMode GetMode();

	//Handle property
	HPEN GetHandle();
	void SetHandle(HPEN value);

	//Color property
	int GetColor();
	void SetColor(int value);

	TPen();
	virtual ~TPen();

	DECLARE_DYNAMIC(TPen)
};

//TCanvas

//const for copy modes
const int cmBlackness = BLACKNESS;
const int cmDstInvert = DSTINVERT;
const int cmMergeCopy = MERGECOPY;
const int cmMergePaint = MERGEPAINT;
const int cmNotSrcCopy = NOTSRCCOPY;
const int cmNotSrcErase = NOTSRCERASE;
const int cmPatCopy = PATCOPY;
const int cmPatInvert = PATINVERT;
const int cmPatPaint = PATPAINT;
const int cmSrcAnd = SRCAND;
const int cmSrcCopy = SRCCOPY;
const int cmSrcErase = SRCERASE;
const int cmSrcInvert = SRCINVERT;
const int cmSrcPaint = SRCPAINT;
const int cmWhiteness = WHITENESS;

//const for canvas states
const int csHandleValid	=1;
const int csFontValid	=2;
const int csPenValid	=4;
const int csBrushValid	=8;
const int csAllValid	=0xf;

typedef enum{
	coLeftToRight, coRightToLeft
}TCanvasOrientation;

typedef enum{
	fsSurface, fsBorder
}TFillStyle;

class TCanvas : public TPersistent
{
private:
	TFont*	m_Font;
	TPen*	m_Pen;
	TBrush*	m_Brush;

	int		m_CopyMode;
	int		m_State;
	HDC		m_Handle;
	POINT	m_PenPos;
	int		m_TextFlags;
private:
	void BrushChanged(CObject* Sender);
	void PenChanged(CObject* Sender);
	void FontChanged(CObject* Sender);

	void CreatePen();
	void CreateFont();
	void CreateBrush();

	void DeselectHandles();
public:
	/* Encapsuled Apis */
	void TextRect(const RECT& Rect, int x, int y, const char* Text);
	void PolyBezier(const POINT* Points, int Count);
	void PolyBezierTo(const POINT* Points, int Count);
	void Polyline(const POINT* Points, int Count);
	void Polygon(const POINT* Points, int Count);
	void Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	void FloodFill(int x, int y, int Color, TFillStyle FillStyle);
	void FrameRect(const RECT& rect);
	void FillRect(const RECT& rect);
	void DrawFocusRect(const RECT& rect);
	void TextOut(int x, int y, const char* Text);
	void RoundRect(int left, int top, int right, int bottom, int width, int height);
	void Rectangle(const RECT& rect);
	void Rectangle(int left, int top, int right, int bottom);
	void Ellipse(const RECT& rect);
	void Ellipse(int left, int top, int right, int bottom);
	void Chord(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc);
	void Arc(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc);
	void MoveTo(int x, int y);
	void LineTo(int x, int y);
public:
	/* Properties */
	//ClipRect property
	RECT GetClipRect();

	//Font property
	TFont* GetFont();
	void SetFont(TFont* value);

	//Brush property
	TBrush* GetBrush();
	void SetBrush(TBrush* value);

	//Pen property
	TPen* GetPen();
	void SetPen(TPen* value);

	//PenPos property
	POINT GetPenPos();
	void SetPenPos(const POINT value);

	//Handle property
	HDC GetHandle();
	void SetHandle(HDC value);

	//CopyMode property
	int GetCopyMode();
	void SetCopyMode(int value);

	//Pixel property
	int GetPixel(int x, int y);
	void SetPixel(int x, int y, int value);

	//CanvasOrientation property
	TCanvasOrientation GetCanvasOrientation();

	//TextFlags
	int GetTextFlags();
	void SetTextFlags(int value);
public:
	void StretchDraw(int xdst, int ydst, int width, int height, HBITMAP bmp);
	void StretchBlt(int xdst, int ydst, int width, int height, HBITMAP bmp, int xsrc, int ysrc, int wsrc, int hsrc);
	void BitBlt(int xdst, int ydst, int width, int height, HBITMAP bmp, int xsrc, int ysrc);
	void Draw(int x, int y, HBITMAP bmp);
	void Refresh();

	void CopyRect(const RECT& Dest, TCanvas& Canvas, const RECT& Source);

	SIZE TextExtent(const char* Text);
	int TextWidth(const char* Text);
	int TextHeight(const char* Text);

	TCanvas();
	virtual ~TCanvas();
protected:
	virtual void CreateHandle();
	void RequiredState(int ReqState);
};

#endif // !defined(_CANVAS_H_)
