#ifndef WX_IRRLICHTVIEWPORT
#define WX_IRRLICHTVIEWPORT

class wxCIViewportSettings;
class wxCIViewport;
class wxCIViewportManager;

typedef struct
{
	wxCIViewport *split1;
	wxCIViewport *split2;
} wxISplitInfo;

#include <wx/dynarray.h>
#include <wx/dc.h>
#include "wxIrrlicht.h"

WX_DECLARE_OBJARRAY(wxCIViewport *, wxIrrArrayOfViewportsPointers);

class wxCIViewportSettings
{
	friend class wxCIViewport;
	friend class wxCIViewportManager;

	public:
		wxCIViewportSettings();
		wxCIViewportSettings(const wxCIViewportSettings &src);
		~wxCIViewportSettings();

		wxCIViewportSettings &operator =(const wxCIViewportSettings &src);

		void SetDefaults(void);

		inline wxIColor GetColorBackground(bool bdefault=false) { return bdefault ? wxIColor(255, 192, 192, 192) : color_background; }
		inline wxIColor GetColorSelected(bool bdefault=false) { return bdefault ? wxIColor(255, 128, 128, 128) : color_selected; }
		inline wxIColor GetColorBorder(bool bdefault=false) { return bdefault ? wxIColor(255, 223, 226, 235) : color_border; }
		inline wxIColor GetColorText(bool bdefault=false) { return bdefault ? wxIColor(255, 0, 0, 0) : color_text; }
		inline wxIColor GetColorSelectedText(bool bdefault=false) { return bdefault ? wxIColor(255, 255, 255, 255) : color_selectedtext; }
		inline wxSize GetBorder(bool bdefault=false) { return bdefault ? wxSize(4,4) : m_border; }
		inline wxIGuiFont *GetFont(bool bdefault=false) { return bdefault ? NULL : font; }
//		inline wxString GetName(bool bdefault=false) { return bdefault ? wxEmptyString : sz_name; }
        inline wxString GetName(bool bdefault=false) { return bdefault ? wxString() : sz_name; }
		inline wxPoint GetNamePosition(bool bdefault=false) { return bdefault ? wxPoint( 5, 5 ) : m_namepos; }

	protected:
		bool b_active;
		bool b_selected;
		wxIColor color_background;
		wxIColor color_selected;
		wxIColor color_border;
		wxIColor color_text;
		wxIColor color_selectedtext;
		wxSize m_border;
		wxPoint m_namepos;
		wxIGuiFont *font;
		wxString sz_name;
};

class wxCIViewport
{
	friend class wxCIViewportManager;

	public:
		wxCIViewport();
		wxCIViewport(const wxCIViewport &src);
		~wxCIViewport();

		wxCIViewport &operator =(const wxCIViewport &src);

		void Draw(wxIVideoDriver *driver);
		bool Contains(wxPoint pos);
		wxIrrOrientation BorderContains(wxPoint pos);
		bool CanBeShown(void);

		inline wxSize GetSize(void) { return m_size; }
		inline int GetWidth(void) { return m_size.GetWidth(); }
		inline int GetHeight(void) { return m_size.GetHeight(); }
		inline wxIColor GetBackgroundlColor(void) { return m_settings.color_background; }
		inline wxIColor GetSelectedColor(void) { return m_settings.color_selected; }
		inline wxIColor GetBorderColor(void) { return m_settings.color_border; }
		inline wxSize GetBorderWidth(void) { return m_settings.m_border; }
		inline wxString GetName(void) { return m_settings.sz_name; }
		inline wxPoint GetNamePosition(void) { return m_settings.m_namepos; }
		inline wxIGuiFont *GetFont(void) { return m_settings.font; }
		inline wxIColor GetTextColor(void) { return m_settings.color_text; }
		inline wxIColor GetSelectedTextColor(void) { return m_settings.color_selectedtext; }
		inline float GetRatio(void) { return (float)GetWidth()/(float)GetHeight(); }
		wxIRectInteger GetArea(bool bmargin=false);

		inline void SetBackgroundColor(wxIColor color) { m_settings.color_background = color; }
		inline void SetSelectedColor(wxIColor color) { m_settings.color_selected = color; }
		inline void SetBorderColor(wxIColor color) { m_settings.color_border = color; }
		inline void SetBorderWidth(wxSize borderwidth) { m_settings.m_border = borderwidth; }
		inline void SetName(wxString name) { m_settings.sz_name = name; }
		inline void SetNamePosition(wxPoint pos) { m_settings.m_namepos = pos; }
		inline void SetFont(wxIGuiFont *fnt) { m_settings.font = fnt; }
		inline void SetTextColor(wxIColor color) { m_settings.color_text = color; }
		inline void SetSelectedTextColor(wxIColor color) { m_settings.color_selectedtext = color; }

	protected:
		inline wxPoint GetPos(void) { return m_pos; }
		void Resize(wxSize size);
		void Layout(void);

	private:
		void RemoveChildsSafe(void);

	protected:
		wxPoint m_pos;
		wxSize m_size;
		wxIrrOrientation n_split;		// the orientation the viewport has been splited
		wxCIViewport *p_split1;
		wxCIViewport *p_split2;
		float f_percentw;				// the percentage of the total parent width (Range from 0.0 - 1.0 )
		float f_percenth;				// the percentage of the total parent height (Range from 0.0 - 1.0 )
		wxCIViewportSettings m_settings;
		wxICamera *p_cam;
};

class wxCIViewportManager
{
	public:
		wxCIViewportManager();
		~wxCIViewportManager();

		virtual void OnSize(wxSizeEvent &event);
		virtual void OnMouseMove(wxMouseEvent &event);
		virtual void OnMouseLeftDown(wxMouseEvent &event);
		virtual void OnMouseLeftUp(wxMouseEvent &event);
		virtual void OnPaint(wxIVideoDriver *driver, wxISceneManager *mgr, wxICamera *cam, wxCIViewport *viewport=NULL);

		inline bool NeedRefresh(void) { return b_refresh; }
		wxISplitInfo SplitViewport(wxCIViewport *viewport, wxIrrOrientation split, float percent=0.5f);
		void RemoveViewport(wxCIViewport *viewport);
		void SelectViewport(wxCIViewport *selected);
		wxCIViewport *Contains(wxPoint pos);

		unsigned int GetViewportCount(bool bsplited=false);
		wxCIViewport *GetViewport(int index, wxCIViewport *viewport=NULL);
		inline wxCIViewport *GetActiveViewport(void) { return m_active; }
		inline wxCIViewport *GetSelectedViewport(void) { return m_selected; }
		inline wxIRectInteger GetClientArea(void) { return m_root->GetArea(); }
		wxIColor GetViewportBackgroundColor(wxCIViewport *viewport);
		wxIColor GetViewportSelectedColor(wxCIViewport *viewport);
		wxIColor GetViewportBorderColor(wxCIViewport *viewport);
		wxSize GetViewportBorderWidth(wxCIViewport *viewport);
		wxString GetViewportName(wxCIViewport *viewport);
		wxIGuiFont *GetViewportFont(wxCIViewport *viewport);
		wxIColor GetViewportTextColor(wxCIViewport *viewport);
		wxIColor GetViewportSelectedTextColor(wxCIViewport *viewport);

		void SetViewportBackgroundColor(wxCIViewport *viewport, wxIColor color);
		void SetViewportSelectedColor(wxCIViewport *viewport, wxIColor color);
		void SetViewportBorderColor(wxCIViewport *viewport, wxIColor color);
		void SetViewportBorderWidth(wxCIViewport *viewport, wxSize borderwidth);
		void SetViewportName(wxCIViewport *viewport, wxString name);
		void SetViewportFont(wxCIViewport *viewport, wxIGuiFont *fnt);
		void SetViewportTextColor(wxCIViewport *viewport, wxIColor color);
		void SetViewportSelectedTextColor(wxCIViewport *viewport, wxIColor color);
		void SetFont(wxIGuiFont *fnt);

	protected:
		void Resize(wxSize size, wxCIViewport *viewport=NULL);
		void Layout(wxCIViewport *viewport=NULL );
		void SetActiveViewport(wxCIViewport *active);

	private:
		void Push(wxCIViewport *viewport);
		wxCIViewport *Pop(void);
		inline bool IsStackEmpty(void) { return (m_vstack.GetCount()==0); }
		inline void ClearStack(void) { m_vstack.Clear(); }
		wxCIViewport *GetParent(wxCIViewport *viewport);
		void SetCursor(void);

	protected:
		wxCIViewport *m_root;
		wxCIViewport *m_active;
		wxCIViewport *m_selected;
		wxCIViewport *m_grabed;
		wxIrrArrayOfViewportsPointers m_vstack;

	private:
		bool b_refresh;
		wxPoint m_point;
		wxSize m_size;
		bool b_allowroodel;
		wxIrrOrientation grabdir;
		wxIGuiFont *font;
};

#endif
