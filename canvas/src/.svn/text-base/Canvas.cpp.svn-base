// Canvas.cpp: implementation of the TCanvas class.
// by David, 2002.4
//
//////////////////////////////////////////////////////////////////////

#include "Canvas.h"

IMPLEMENT_DYNAMIC(TGraphicsObject, TPersistent)
IMPLEMENT_DYNAMIC(TBrush, TGraphicsObject)
IMPLEMENT_DYNAMIC(TFont, TGraphicsObject)
IMPLEMENT_DYNAMIC(TPen, TGraphicsObject)

class TResourceManager
{
private:
	PResource m_ResList;
	int m_ResDataSize;
public:
	void FreeResource(PResource Resource);
	PResource AllocResource(const void* ResData);
	void AssignResource(TGraphicsObject* GraphicsObject, PResource AResource);
	void ChangeResource(TGraphicsObject* GraphicsObject, const void* ResData);
	TResourceManager(int AResDataSize);
	virtual ~TResourceManager();
};

//init and done object
class CInitDone
{
public:
	CInitDone();
	virtual ~CInitDone();
};

static HPEN		StockPen;
static HBRUSH	StockBrush;
static HFONT	StockFont;
static TFontData	DefFontData= {
    0, 0, fpDefault, 0, DEFAULT_CHARSET, "MS Sans Serif"
};

static TPenData DefPenData = {
	0, clBlack, 1, psSolid
};



static TResourceManager FontManager(sizeof(TFontData));
static TResourceManager PenManager(sizeof(TPenData));
static TResourceManager BrushManager(sizeof(TBrushData));

static int ScreenLogPixels;

static void InitScreenLogPixels()
{
	HDC dc=GetDC(0);
	ScreenLogPixels = GetDeviceCaps(dc, LOGPIXELSY);
	ReleaseDC(0, dc);
}

CInitDone::CInitDone()
{
	InitScreenLogPixels();

	StockPen = (HPEN)GetStockObject(BLACK_PEN);
	StockBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	StockFont = (HFONT)GetStockObject(SYSTEM_FONT);
}

CInitDone::~CInitDone()
{
}
static CInitDone initdone;


//static functions
static COLORREF ColorToRGB(int Color)
{
	if(Color<0)
		return GetSysColor(Color & 0xff);
	else
		return Color;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCanvas::TCanvas()
{
	m_Font=new TFont();
	m_Font->SetOnChange(this, (TNotifyFunc)FontChanged);

	m_Pen=new TPen();
	m_Pen->SetOnChange(this, (TNotifyFunc)PenChanged);

	m_Brush=new TBrush();
	m_Brush->SetOnChange(this, (TNotifyFunc)BrushChanged);

	m_CopyMode = cmSrcCopy;
	m_State = 0;
	
	m_Handle = 0;
	m_TextFlags = 0;
	m_PenPos.x=0;  m_PenPos.y=0;
}

TCanvas::~TCanvas()
{
	SetHandle(0);

	delete m_Font;
	delete m_Pen;
	delete m_Brush;
}

void TCanvas::FontChanged(CObject* Sender)
{
	if(SET_IN(m_State, csFontValid)){
		SET_EXCLUDE(m_State, csFontValid);
		SelectObject(m_Handle, StockFont);
	}
}

void TCanvas::PenChanged(CObject* Sender)
{
	if(SET_IN(m_State, csPenValid)){
		SET_EXCLUDE(m_State, csPenValid);
		SelectObject(m_Handle, StockPen);
	}
}

void TCanvas::BrushChanged(CObject* Sender)
{
	if(SET_IN(m_State, csBrushValid)){
		SET_EXCLUDE(m_State, csBrushValid);
		SelectObject(m_Handle, StockBrush);
	}
}

void TCanvas::SetHandle(HDC value)
{
	if(m_Handle!=value){
		if(m_Handle!=0){
			DeselectHandles();
			m_PenPos = GetPenPos();
			m_Handle = 0;
			SET_EXCLUDE(m_State, csHandleValid);
		}

		if(value!=0){
			SET_INCLUDE(m_State, csHandleValid);
			m_Handle = value;
			SetPenPos(m_PenPos);
		}
	}
}

void TCanvas::DeselectHandles()
{
	if(m_Handle!=0 && (SET_MINUS(m_State, csPenValid|csBrushValid|csFontValid)!=m_State)){
		SelectObject(m_Handle, StockPen);
		SelectObject(m_Handle, StockBrush);
		SelectObject(m_Handle, StockFont);

		m_State=SET_MINUS(m_State, csPenValid|csBrushValid|csFontValid);
	}
}

void TCanvas::RequiredState(int ReqState)
{
	int NeededState=SET_MINUS(ReqState, m_State);
	if(NeededState!=SET_EMPTY){
		if(SET_IN(NeededState, csHandleValid))
			CreateHandle();

		if(SET_IN(NeededState, csFontValid))
			this->CreateFont();

		if(SET_IN(NeededState, csPenValid))
			this->CreatePen();

		if(SET_IN(NeededState, csBrushValid))
			this->CreateBrush();

		m_State=SET_PLUS(m_State, NeededState);
	}
}

void TCanvas::SetPenPos(const POINT value)
{
	MoveTo(value.x, value.y);
}

void TCanvas::CreateHandle()
{
}

void TCanvas::CreateBrush()
{
	UnrealizeObject(m_Brush->GetHandle());
	SelectObject(m_Handle, m_Brush->GetHandle());
	if(m_Brush->GetStyle()==bsSolid){
		SetBkColor(m_Handle, ColorToRGB(m_Brush->GetColor()));
		SetBkMode(m_Handle, OPAQUE);
	}else{
		/* Win95 doesn't draw brush hatches if bkcolor = brush color */
		/* Since bkmode is transparent, nothing should use bkcolor anyway */
		SetBkColor(m_Handle, ~ColorToRGB(m_Brush->GetColor()));
		SetBkMode(m_Handle, TRANSPARENT);
	}
}

void TCanvas::CreateFont()
{
	SelectObject(m_Handle, m_Font->GetHandle());
	SetTextColor(m_Handle, ColorToRGB(m_Font->GetColor()));
}

void TCanvas::CreatePen()
{
	SelectObject(m_Handle, m_Pen->GetHandle());

	int PenModes[16]={R2_BLACK, R2_WHITE, R2_NOP, R2_NOT, R2_COPYPEN, R2_NOTCOPYPEN, R2_MERGEPENNOT,
     R2_MASKPENNOT, R2_MERGENOTPEN, R2_MASKNOTPEN, R2_MERGEPEN, R2_NOTMERGEPEN,
     R2_MASKPEN, R2_NOTMASKPEN, R2_XORPEN, R2_NOTXORPEN};

	SetROP2(m_Handle, PenModes[m_Pen->GetMode()]);
}

POINT TCanvas::GetPenPos()
{
	POINT result;
	RequiredState(csHandleValid);
	GetCurrentPositionEx(m_Handle, &result);
	return result;
}

void TCanvas::MoveTo(int x, int y)
{
	RequiredState(csHandleValid);
	MoveToEx(m_Handle, x, y, NULL);
}

//////////////////////////////////////////////////////////////////////
// TBrush Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const TBrushData DefBrushData={0, clWhite, NULL, bsSolid};

TBrush::TBrush()
{
	m_Resource=BrushManager.AllocResource(&DefBrushData);
}

TBrush::~TBrush()
{
	BrushManager.FreeResource(m_Resource);
}

HBRUSH TBrush::GetHandle()
{
	if(m_Resource->Handle==0){
		LOGBRUSH LogBrush;
		if(m_Resource->Brush.Bitmap!=NULL){
			LogBrush.lbStyle=BS_PATTERN;
			LogBrush.lbHatch=(LONG)m_Resource->Brush.Bitmap;
		}else{
			LogBrush.lbHatch=0;
			switch(m_Resource->Brush.Style){
			case bsSolid:
				LogBrush.lbStyle=BS_SOLID;
				break;
			case bsClear:
				LogBrush.lbStyle=BS_HOLLOW;
				break;
			default:
				LogBrush.lbStyle=BS_HATCHED;
				LogBrush.lbHatch=(int)m_Resource->Brush.Style-(int)bsHorizontal;
			}
			LogBrush.lbColor=ColorToRGB(m_Resource->Brush.Color);
		}
		m_Resource->Handle=CreateBrushIndirect(&LogBrush);
	}
	return (HBRUSH)m_Resource->Handle;
}

TBrushStyle TBrush::GetStyle()
{
	return m_Resource->Brush.Style;
}

void TBrush::SetColor(int value)
{
	TBrushData BrushData;
	GetData(BrushData);

	BrushData.Color=value;
	if(BrushData.Style==bsClear)
		BrushData.Style=bsSolid;

	SetData(BrushData);
}

void TBrush::GetData(TBrushData &BrushData)
{
	BrushData=m_Resource->Brush;
	BrushData.Handle=0;
	BrushData.Bitmap=0;
}

void TBrush::SetData(const TBrushData &BrushData)
{
	BrushManager.ChangeResource(this, &BrushData);
}

int TBrush::GetColor()
{
	return m_Resource->Brush.Color;
}

void TBrush::SetStyle(TBrushStyle value)
{
	TBrushData BrushData;
	GetData(BrushData);

	BrushData.Style = value;
	if(BrushData.Style==bsClear)
		BrushData.Color=clWhite;

	SetData(BrushData);
}

void TBrush::SetHandle(HBRUSH value)
{
	TBrushData BrushData = DefBrushData;
	BrushData.Handle = value;
	SetData(BrushData);
}

void TBrush::SetBitmap(HBITMAP value)
{
	TBrushData BrushData = DefBrushData;
	BrushData.Bitmap = value;
	SetData(BrushData);
}

HBITMAP TBrush::GetBitmap()
{
	return m_Resource->Brush.Bitmap;
}

void TBrush::Assign(TPersistent *Source)
{
	if(Source->IsKindOf(RUNTIME_CLASS(TBrush))){
		BrushManager.AssignResource(this, ((TBrush*)Source)->m_Resource);
		return;
	}
	TPersistent::Assign(Source);
}
//////////////////////////////////////////////////////////////////////
// TFont Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TFont::TFont()
{
	DefFontData.Handle = 0;
	m_Resource = FontManager.AllocResource(&DefFontData);

	m_Color = clWindowText;
	m_PixelsPerInch = ScreenLogPixels;
}

TFont::~TFont()
{
	FontManager.FreeResource(m_Resource);
}

HFONT TFont::GetHandle()
{
	if(m_Resource->Handle==0){
		LOGFONT LogFont;
		if(m_Resource->Handle==0){
			LogFont.lfHeight = m_Resource->Font.Height;
			LogFont.lfWidth = 0;
			LogFont.lfEscapement = 0;
			LogFont.lfOrientation = 0;
			if(SET_IN(m_Resource->Font.Style, fsBold))
				LogFont.lfWeight = FW_BOLD;
			else
				LogFont.lfWeight = FW_NORMAL;
			LogFont.lfItalic = SET_IN(fsItalic, m_Resource->Font.Style);
			LogFont.lfUnderline = SET_IN(fsUnderline, m_Resource->Font.Style);
			LogFont.lfStrikeOut = SET_IN(fsStrikeOut, m_Resource->Font.Style);
			LogFont.lfCharSet = m_Resource->Font.CharSet;
			if(stricmp(m_Resource->Font.Name, "Default")==0)
				strcpy(LogFont.lfFaceName, DefFontData.Name);
			else
				strcpy(LogFont.lfFaceName, m_Resource->Font.Name);
			LogFont.lfQuality = DEFAULT_QUALITY;
			LogFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			switch(m_Resource->Font.Pitch){
				case fpVariable:
					LogFont.lfPitchAndFamily = VARIABLE_PITCH;
					break;
				case fpFixed:
					LogFont.lfPitchAndFamily = FIXED_PITCH;
					break;
				default:
					LogFont.lfPitchAndFamily = DEFAULT_PITCH;
			}
			m_Resource->Handle = CreateFontIndirect(&LogFont);
		}
	}

	return (HFONT)m_Resource->Handle;
}

int TFont::GetColor()
{
	return m_Color;
}

void TFont::GetData(TFontData &FontData)
{
	FontData = m_Resource->Font;
	FontData.Handle = 0;
}

void TFont::SetData(const TFontData &FontData)
{
	FontManager.ChangeResource(this, &FontData);
}

void TFont::Changed()
{
	TGraphicsObject::Changed();
}

const char* TFont::GetName()
{
	return m_Resource->Font.Name;
}

int TFont::GetHeight()
{
	return m_Resource->Font.Height;
}

TFontPitch TFont::GetPitch()
{
	return m_Resource->Font.Pitch;
}

int TFont::GetSize()
{
	return -MulDiv(GetHeight(), 72, m_PixelsPerInch);
}

int TFont::GetStyle()
{
	return m_Resource->Font.Style;
}

BYTE TFont::GetCharSet()
{
	return m_Resource->Font.CharSet;
}

void TFont::SetColor(int value)
{
	if(m_Color!=value){
		m_Color=value;
		Changed();
	}
}

static TFontData GetFontData(HFONT Font)
{
	TFontData result;
	if(Font!=0){
		LOGFONT LogFont;
		if(GetObject(Font, sizeof(LogFont), &LogFont)!=0){
			result.Height = LogFont.lfHeight;
			if(LogFont.lfWeight>=FW_BOLD)
				SET_INCLUDE(result.Style, fsBold);
			if(LogFont.lfItalic)
				SET_INCLUDE(result.Style, fsItalic);
			if(LogFont.lfUnderline)
				SET_INCLUDE(result.Style, fsUnderline);
			if(LogFont.lfStrikeOut)
				SET_INCLUDE(result.Style, fsStrikeOut);
			result.CharSet = LogFont.lfCharSet;
			strcpy(result.Name, LogFont.lfFaceName);

			switch(LogFont.lfPitchAndFamily&0xf){
				case VARIABLE_PITCH:
					result.Pitch=fpVariable;
					break;
				case FIXED_PITCH:
					result.Pitch=fpFixed;
					break;
				default:
					result.Pitch=fpDefault;
			}
			result.Handle=Font;
		}
	}
	return result;
}

void TFont::SetHandle(HFONT value)
{
	SetData(GetFontData(value));
}

void TFont::SetHeight(int value)
{
	TFontData FontData;
	GetData(FontData);
	FontData.Height=value;
	SetData(FontData);
}

void TFont::SetName(const char *value)
{
	if(value!=NULL && strlen(value)!=0){
		TFontData FontData;
		GetData(FontData);
		memset(FontData.Name, 0, sizeof(FontData.Name));
		strcpy(FontData.Name, value);
		SetData(FontData);
	}
}

void TFont::SetPitch(TFontPitch value)
{
	TFontData FontData;
	GetData(FontData);
	FontData.Pitch = value;
	SetData(FontData);
}

void TFont::SetSize(int value)
{
	SetHeight(-MulDiv(value, m_PixelsPerInch, 72));
}

void TFont::SetStyle(int value)
{
	TFontData FontData;
	GetData(FontData);
	FontData.Style = value;
	SetData(FontData);
}

void TFont::SetCharSet(BYTE value)
{
	TFontData FontData;
	GetData(FontData);
	FontData.CharSet = value;
	SetData(FontData);
}

void TFont::Assign(TPersistent *Source)
{
	if(Source->IsKindOf(RUNTIME_CLASS(TFont))){
		FontManager.AssignResource(this, ((TFont*)Source)->m_Resource);
		SetColor(((TFont*)Source)->GetColor());
		if(GetPixelsPerInch()!=((TFont*)Source)->GetPixelsPerInch()){
			SetSize(((TFont*)Source)->GetSize());
		}
		return;
	}
	TGraphicsObject::Assign(Source);
}

int TFont::GetPixelsPerInch()
{
	return m_PixelsPerInch;
}

void TFont::SetPixelsPerInch(int value)
{
	m_PixelsPerInch = value;
}
//////////////////////////////////////////////////////////////////////
// TPen Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TPen::TPen()
{
	m_Resource = PenManager.AllocResource(&DefPenData);
	m_Mode = pmCopy;
}

TPen::~TPen()
{
	PenManager.FreeResource(m_Resource);
}

const UINT PenStyles[7]={
	PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT, PS_NULL, PS_INSIDEFRAME
};

HPEN TPen::GetHandle()
{
	if(m_Resource->Handle==0){
		LOGPEN LogPen;
		if(m_Resource->Handle==0){
			LogPen.lopnStyle = PenStyles[m_Resource->Pen.Style];
			LogPen.lopnWidth.x = m_Resource->Pen.Width;
			LogPen.lopnColor = ColorToRGB(m_Resource->Pen.Color);
			m_Resource->Handle = CreatePenIndirect(&LogPen);
		}
	}
	return (HPEN)m_Resource->Handle;
}

TPenMode TPen::GetMode()
{
	return m_Mode;
}

void TPen::GetData(TPenData &PenData)
{
	PenData = m_Resource->Pen;
	PenData.Handle = 0;
}

void TPen::SetData(const TPenData &PenData)
{
	PenManager.ChangeResource(this, &PenData);
}

void TPen::SetColor(int value)
{
	TPenData PenData;
	GetData(PenData);
	PenData.Color = value;
	SetData(PenData);
}

int TPen::GetColor()
{
	return m_Resource->Pen.Color;
}

void TPen::SetHandle(HPEN value)
{
	TPenData PenData;
	GetData(PenData);
	PenData.Handle = value;
	SetData(PenData);
}

void TPen::SetMode(TPenMode value)
{
	if(m_Mode != value){
		m_Mode = value;
		Changed();
	}
}

void TPen::SetStyle(TPenStyle value)
{
	TPenData PenData;
	GetData(PenData);
	PenData.Style = value;
	SetData(PenData);
}

TPenStyle TPen::GetStyle()
{
	return m_Resource->Pen.Style;
}

void TPen::SetWidth(int value)
{
	if(value>0){
		TPenData PenData;
		GetData(PenData);
		PenData.Width = value;
		SetData(PenData);
	}
}

int TPen::GetWidth()
{
	return m_Resource->Pen.Width;
}

void TPen::Assign(TPersistent *Source)
{
	if(Source->IsKindOf(RUNTIME_CLASS(TPen))){
		PenManager.AssignResource(this, ((TPen*)Source)->m_Resource);
		SetMode(((TPen*)Source)->m_Mode);
		return;
	}
	TGraphicsObject::Assign(Source);
}
//////////////////////////////////////////////////////////////////////
// TGraphicsObject Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TGraphicsObject::TGraphicsObject()
{
	m_OnChange.self=NULL;
}

TGraphicsObject::~TGraphicsObject()
{
}

void TGraphicsObject::Changed()
{
	if(m_OnChange.self!=NULL)
		PERFORMEVENT(m_OnChange)(this);
}

void TGraphicsObject::SetOnChange(CObject* self, TNotifyFunc func)
{
	m_OnChange.self=self;  m_OnChange.func=func;
}

//////////////////////////////////////////////////////////////////////
// TResourceManager Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TResourceManager::TResourceManager(int AResDataSize)
{
	m_ResList = NULL;
	m_ResDataSize = AResDataSize;
}

TResourceManager::~TResourceManager()
{
}

void TResourceManager::ChangeResource(TGraphicsObject *GraphicsObject, const void* ResData)
{
	PResource p=GraphicsObject->m_Resource;
	GraphicsObject->m_Resource = AllocResource(ResData);
	if(GraphicsObject->m_Resource != p)
		GraphicsObject->Changed();
	FreeResource(p);
}

void TResourceManager::AssignResource(TGraphicsObject *GraphicsObject, PResource AResource)
{
	PResource p=GraphicsObject->m_Resource;
	if(p!=AResource){
		AResource->RefCount++;
		GraphicsObject->m_Resource = AResource;
		GraphicsObject->Changed();
		FreeResource(p);
	}
}

static int GetHashCode(const void* Buffer, int Count)
{
	int res;
	__asm{
		mov ecx, Count
		mov edx, Buffer
		xor eax, eax
	l1:
		rol ax, 5
		xor al, [edx]
		inc edx
		dec ecx
		jne l1
		mov res, eax
	}
	return res;
}	

const int ResInfoSize = sizeof(TResource) - sizeof(TFontData);

PResource TResourceManager::AllocResource(const void *ResData)
{
	PResource result;

	int ResHash = GetHashCode(ResData, m_ResDataSize);
	result = m_ResList;
	while(result!=NULL && (result->HashCode!=ResHash||memcmp(&(result->Data), ResData, m_ResDataSize))){
		result=result->Next;
	}
	if(result==NULL){
		result=(PResource)malloc(m_ResDataSize+ResInfoSize);
		result->Next = m_ResList;
		result->RefCount = 0;
		result->Handle = ((PResData)ResData)->Handle;
		result->HashCode = ResHash;
		memmove(&(result->Data), ResData, m_ResDataSize);

		m_ResList= result;
	}
	result->RefCount++;

	return result;
}

void TResourceManager::FreeResource(PResource Resource)
{
	if(Resource!=NULL){
		Resource->RefCount--;
		bool DeleteIt = Resource->RefCount==0;
		if(DeleteIt){
			if(Resource==m_ResList)
				m_ResList = Resource->Next;
			else{
				PResource p = m_ResList;
				while(p->Next!=Resource)
					p=p->Next;
				p->Next=Resource->Next;
			}
			if(Resource->Handle!=0)
				DeleteObject(Resource->Handle);
			free(Resource);
		}
	}
}

void TCanvas::LineTo(int x, int y)
{
	RequiredState(csHandleValid|csPenValid|csBrushValid);
	::LineTo(m_Handle, x, y);
}

TPen* TCanvas::GetPen()
{
	return m_Pen;
}

void TCanvas::Arc(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc)
{
	RequiredState(csHandleValid|csPenValid|csBrushValid);
	::Arc(m_Handle, nLeftRect, nTopRect, nRightRect, nBottomRect, nXStartArc, nYStartArc, nXEndArc, nYEndArc);
}

void TCanvas::Chord(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc)
{
	RequiredState(csHandleValid|csPenValid|csBrushValid);
	::Chord(m_Handle, nLeftRect, nTopRect, nRightRect, nBottomRect, nXStartArc, nYStartArc, nXEndArc, nYEndArc);
}

void TCanvas::CopyRect(const RECT &Dest, TCanvas &Canvas, const RECT &Source)
{
	RequiredState(csHandleValid | csFontValid | csBrushValid);
	Canvas.RequiredState(csHandleValid | csBrushValid);
	::StretchBlt(m_Handle, Dest.left, Dest.top, Dest.right-Dest.left, Dest.bottom-Dest.top,
		Canvas.m_Handle, Source.left, Source.top, Source.right-Source.left, Source.bottom-Source.top, m_CopyMode);
}

void TCanvas::Ellipse(int left, int top, int right, int bottom)
{
	RequiredState(csHandleValid|csPenValid|csBrushValid);
	::Ellipse(m_Handle, left, top, right, bottom);
}

void TCanvas::Ellipse(const RECT& rect)
{
	Ellipse(rect.left, rect.top, rect.right, rect.bottom);
}

void TCanvas::Rectangle(int left, int top, int right, int bottom)
{
	RequiredState(csHandleValid|csPenValid|csBrushValid);
	::Rectangle(m_Handle, left, top, right, bottom);
}

void TCanvas::Rectangle(const RECT& rect)
{
	Rectangle(rect.left, rect.top, rect.right, rect.bottom);
}

void TCanvas::RoundRect(int left, int top, int right, int bottom, int width, int height)
{
	RequiredState(csHandleValid|csPenValid|csBrushValid);
	::RoundRect(m_Handle, left, top, right, bottom, width, height);
}

TBrush* TCanvas::GetBrush()
{
	return m_Brush;
}

TFont* TCanvas::GetFont()
{
	return m_Font;
}

void TCanvas::TextOut(int x, int y, const char *Text)
{
	RequiredState(csHandleValid | csFontValid | csBrushValid);
	if(GetCanvasOrientation()==coRightToLeft){
		x += TextWidth(Text)+1;
	}
	::ExtTextOut(m_Handle, x, y, m_TextFlags, NULL, Text, strlen(Text), NULL);
	MoveTo(x+TextWidth(Text), y);
}

TCanvasOrientation TCanvas::GetCanvasOrientation()
{
	TCanvasOrientation result=coLeftToRight;
	if((m_TextFlags & ETO_RTLREADING) !=0 ){
		POINT Point;
		GetWindowOrgEx(m_Handle, &Point);
		if(Point.x!=0)
			result=coRightToLeft;
	}
	return result;
}

int TCanvas::TextWidth(const char *Text)
{
	return TextExtent(Text).cx;
}

SIZE TCanvas::TextExtent(const char *Text)
{
	SIZE result;
	RequiredState(csHandleValid | csFontValid);
	result.cx=0;  result.cy=0;
	::GetTextExtentPoint32(m_Handle, Text, strlen(Text), &result);
	return result;
}

int TCanvas::TextHeight(const char *Text)
{
	return TextExtent(Text).cy;
}

RECT TCanvas::GetClipRect()
{
	RECT result;
	RequiredState(csHandleValid);
	::GetClipBox(m_Handle, &result);
	return result;
}

void TCanvas::SetFont(TFont *value)
{
	m_Font->Assign(value);
}

void TCanvas::SetPen(TPen *value)
{
	m_Pen->Assign(value);
}

void TCanvas::SetBrush(TBrush *value)
{
	m_Brush->Assign(value);
}

HDC TCanvas::GetHandle()
{
	RequiredState(csAllValid);
	return m_Handle;
}

void TCanvas::SetCopyMode(int value)
{
	m_CopyMode=value;
}

int TCanvas::GetCopyMode()
{
	return m_CopyMode;
}

void TCanvas::SetTextFlags(int value)
{
	m_TextFlags=value;
}

int TCanvas::GetTextFlags()
{
	return m_TextFlags;
}

void TCanvas::SetPixel(int x, int y, int value)
{
	RequiredState(csHandleValid | csPenValid);
	::SetPixel(m_Handle, x, y, ColorToRGB(value));
}

int TCanvas::GetPixel(int x, int y)
{
	RequiredState(csHandleValid);
	return ::GetPixel(m_Handle, x, y);
}

void TCanvas::Refresh()
{
	DeselectHandles();
}

void TCanvas::DrawFocusRect(const RECT& rect)
{
	RequiredState(csHandleValid | csBrushValid);
	::DrawFocusRect(m_Handle, &rect);
}

void TCanvas::FillRect(const RECT& rect)
{
	RequiredState(csHandleValid | csBrushValid);
	::FillRect(m_Handle, &rect, m_Brush->GetHandle());
}

const int FillStyles[2]={FLOODFILLSURFACE, FLOODFILLBORDER};

void TCanvas::FloodFill(int x, int y, int Color, TFillStyle FillStyle)
{
	RequiredState(csHandleValid | csBrushValid);
	::ExtFloodFill(m_Handle, x, y, Color, FillStyles[FillStyle]);
}

void TCanvas::FrameRect(const RECT& rect)
{
	RequiredState(csHandleValid | csBrushValid);
	::FrameRect(m_Handle, &rect, m_Brush->GetHandle());
}

void TCanvas::Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	RequiredState(csHandleValid | csPenValid | csBrushValid);
	::Pie(m_Handle, x1, y1, x2, y2, x3, y3, x4, y4);
}

void TCanvas::Polygon(const POINT *Points, int Count)
{
	RequiredState(csHandleValid | csPenValid | csBrushValid);
	::Polygon(m_Handle, Points, Count);
}

void TCanvas::Polyline(const POINT *Points, int Count)
{
	RequiredState(csHandleValid | csPenValid | csBrushValid);
	::Polyline(m_Handle, Points, Count);
}

void TCanvas::PolyBezier(const POINT *Points, int Count)
{
	RequiredState(csHandleValid | csPenValid | csBrushValid);
	::PolyBezier(m_Handle, Points, Count);
}
void TCanvas::PolyBezierTo(const POINT *Points, int Count)
{
	RequiredState(csHandleValid | csPenValid | csBrushValid);
	::PolyBezierTo(m_Handle, Points, Count);
}

void TCanvas::TextRect(const RECT& Rect, int x, int y, const char *Text)
{
	RequiredState(csHandleValid | csFontValid | csBrushValid);
	int Options= ETO_CLIPPED | m_TextFlags;
	if(m_Brush->GetStyle()!=bsClear)
		Options|=ETO_OPAQUE;
	if ((m_TextFlags & ETO_RTLREADING) != 0 && GetCanvasOrientation() == coRightToLeft)
		x+=TextWidth(Text)+1;
	::ExtTextOut(m_Handle, x, y, Options, &Rect, Text, strlen(Text), NULL);
}

void TCanvas::Draw(int x, int y, HBITMAP bmp)
{
	BITMAP info;
	if(GetObject(bmp, sizeof(info), &info)==0)
		return;

	BitBlt(x, y, info.bmWidth, info.bmHeight, bmp, 0, 0);
}

void TCanvas::BitBlt(int xdst, int ydst, int width, int height, HBITMAP bmp, int xsrc, int ysrc)
{
	RequiredState(csHandleValid);

	HDC tmpdc=::CreateCompatibleDC(m_Handle);
	if(tmpdc==NULL)
		return;

	HBITMAP oldmap=(HBITMAP)::SelectObject(tmpdc, bmp);

	::BitBlt(m_Handle, xdst, ydst, width, height, tmpdc, xsrc, ysrc, m_CopyMode);

	::SelectObject(tmpdc, oldmap);
	::DeleteDC(tmpdc);
}

void TCanvas::StretchBlt(int xdst, int ydst, int width, int height, HBITMAP bmp, int xsrc, int ysrc, int wsrc, int hsrc)
{
	RequiredState(csHandleValid);

	HDC tmpdc=::CreateCompatibleDC(m_Handle);
	if(tmpdc==NULL)
		return;

	HBITMAP oldmap=(HBITMAP)::SelectObject(tmpdc, bmp);

	::StretchBlt(m_Handle, xdst, ydst, width, height, tmpdc, xsrc, ysrc, wsrc, hsrc, m_CopyMode);

	::SelectObject(tmpdc, oldmap);
	::DeleteDC(tmpdc);
}

void TCanvas::StretchDraw(int xdst, int ydst, int width, int height, HBITMAP bmp)
{
	BITMAP info;
	if(GetObject(bmp, sizeof(info), &info)==0)
		return;

	StretchBlt(xdst, ydst, width, height, bmp, 0, 0, info.bmWidth, info.bmHeight);
}
