 
#ifndef netrider_h
#define netrider_h

#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/Fl_Double_Window.H>

#include <FL/Enumerations.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/gl.h> 
#include <glib.h>

#include <WebKit2/WKContext.h>
#include <WebKit2/WKType.h>
#include <WebKit2/WKURL.h>
#include <WebKit2/WKPage.h>
#include "WebKit2/WKString.h"
#include "WebKit2/WKPreferencesRef.h"
#include "WebKit2/WKPreferencesPrivate.h"
#include "WebKit2/WKRetainPtr.h"
#include "WebKit2/WKData.h"
#include "WebKit2/WKHitTestResult.h"
#include "WebKit2/WKPageUIClient.h"
#include "WebKit2/WKFramePolicyListener.h"
#include "WebKit2/WKURLResponse.h"
//#include <WebCore/HTTPStatusCodes.h>
#include "WebKit2/WKFrame.h"
#include "WebKit2/WKError.h"
#include "WebKit2/WKDownload.h"

#if 0
#include "WebKit2/WKArray.h"
#include "WebKit2/WKAuthenticationChallenge.h"
#include "WebKit2/WKAuthenticationDecisionListener.h"
#include "WebKit2/WKContextNix.h"
#include "WebKit2/WKCredential.h"
#include "WebKit2/WKCredentialTypes.h"
#include "WebKit2/WKErrorNix.h"
#include "WebKit2/WKFrame.h"
#include "WebKit2/WKPageNix.h"
#include "WebKit2/WKProtectionSpace.h"
#endif

#include<stdio.h>
#include<stdlib.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#include <unistd.h>
#include <sys/stat.h>

#include <FL/Fl_Menu.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Scrollbar.H>

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/names.h>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Printer.H>
//#include <FL/Fl_Paged_Device.H>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include <NIXView.h>
#include "NIXEvents.h"
#include "inpututils.h"

#define testsoup 0
#if testsoup
  #include <libsoup/soup.h> //for file transfer
#endif

//#include "Fl_I18n.H"
#define lt8 locale->translate

#define webkitsupport 1
#define MAXVIEWS 600
#define defaulturl "http://www.google.com"
#define MAXHISTORY 100
#define MAXBOOKMARKS 512
#define defaultWindowWidth 1024 // 800
#define defaultWindowHeight 768 //600

/*************************************************************************
* Globals
**************************************************************************/

std::vector<std::string> args;
char *requesturl[2];

void* pixelbuffer[MAXVIEWS];
void* xferpixelbuffer;
void* dummypixelbuffer;

Fl_Window *findwindow;

int activetransfers=0;
int prefs_written=0;
char* homedir;
char downloadsdir[128];
char downloadsdir_ftp[128];
char netriderdirpath[FL_PATH_MAX]; 

const int windowWidth = defaultWindowWidth; //1024;
const int windowHeight = defaultWindowHeight; //768;

const int WKwindowWidth =Fl::w(); //800; //1024;
const int WKwindowHeight =Fl::h(); //600; //768;

int currmaxscrolly;
int currmaxscrollx;

typedef struct url_text
{
  char name[1024];
  char uri[1024];
}url_text;

#include "Fl_I18n.cxx"
Fl_I18n *locale;

//function declarations
void readPrefs(); 
void writePrefs(); 
void checkurl(char *url);
void showscrollbars(WKStringRef maxscrollstring, WKErrorRef err_ref, void*);
/*************************************************************************
* WebKit2 interface
**************************************************************************/
class WebKit2 
{
public:
    WebKit2(const std::vector<std::string>& urls);
    ~WebKit2();

    WKViewRef uiView() { return m_uiView; }
    WKPageRef uiPage() { return m_uiPage; }
    WKPageGroupRef contentPageGroup() { return m_contentPageGroup; }
    void scheduleUpdateDisplay();
    std::string mouse_uri;
    std::string mouse_title;

private:
    WKViewRef m_uiView;
    WKPageRef m_uiPage;

    WKContextRef m_uiContext;
    WKPageGroupRef m_uiPageGroup;
    WKPageGroupRef m_contentPageGroup;
    
    char* downloadname;

    const std::vector<std::string>& m_initialUrls;

    void initUi();

};

//Globals
WebKit2* view[MAXVIEWS];
WebKit2* xferview;

/*******************************************************************************************
 * MyGlWindow class definition
*******************************************************************************************/

class MyGlWindow : public Fl_Gl_Window {
    //OpenGL window: (w,h) is upper right, (-w,-h) is lower left, (0,0) is center
    void draw();
    
public:
    int handle_key(int e,int key);
    // CONSTRUCTOR
    MyGlWindow(int X,int Y,int W,int H,const char*L=0) : Fl_Gl_Window(X,Y,W,H,L) {
    };
    void resize(int X,int Y,int W,int H);
    void dodraw();
    int handle(int e);
};

/*************************************************************************
* Tooltip window
**************************************************************************/
#if 0
class TipWin : public Fl_Menu_Window {
public:
    char tip[1024];
    TipWin():Fl_Menu_Window(1,1) {      // will autosize
        //strcpy(tip, "X.XX");
        set_override();
        end();
    }
    void draw() {
        draw_box(FL_BORDER_BOX, 0, 0, w(), h(), Fl_Color(175));
        fl_color(FL_BLACK);
        fl_font(labelfont(), labelsize());
        fl_draw(tip, 3, 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP));
    }
};
#endif
/*************************************************************************
* Navigation
**************************************************************************/
	
class HtmlBrowser;

class HtmlNavigation : public Fl_Group {

private:
	//int handle_key(int e,int key);

public:
	char *history[10];
	HtmlBrowser *browser;
	Fl_Button *menu;
	Fl_Button *forward;
	Fl_Button *back;
	Fl_Button *stop;
	Fl_Button *reload;
	Fl_Input *loc;
	Fl_Button *go;
	Fl_Input *search;
	Fl_Button *searchbutton;
	Fl_Button *bookmarks;

	Fl_Pixmap *p_menu;
        Fl_Pixmap *p_back;
        Fl_Pixmap *p_forward;
        Fl_Pixmap *p_stop;
	Fl_Pixmap *p_reload;
        Fl_Pixmap *p_go;        
	Fl_Pixmap *p_search;
	Fl_Pixmap *p_bookmarks;

	HtmlNavigation(HtmlBrowser *e, int X, int Y, int W, int H);
	~HtmlNavigation(void);
	//int handle(int e); 
	
	static void cb_back(Fl_Widget*, void*);
	static void cb_forward(Fl_Widget*, void*);
	static void cb_stop(Fl_Widget*, void*);
	static void cb_reload(Fl_Widget*, void*);
	static void cb_loc(Fl_Widget*, void*);
	static void cb_search(Fl_Widget*, void*);
	//static void cb_bookmarks(Fl_Widget*, void*);

};

/*************************************************************************
* Browser Group - consists of toolbar and web window
**************************************************************************/

class HtmlBrowser : public Fl_Group {
public:
	//HtmlView *bview;
	MyGlWindow* bview;
	HtmlNavigation *tool;
	Fl_Scrollbar *scrollbar_v;
	int scrollbar_vw;
	Fl_Scrollbar *scrollbar_h;
	int scrollbar_hw;
	int maxscrollx,maxscrolly;
	
	char tabtitle[128]; 
	char downloadfilename[1024];
	float bytesdownloaded;
	WKDownloadRef download_ref;
	bool savetargetas;
	char currenturl[1024];
	char currenttitle[1024];

	HtmlBrowser(int X, int Y, int W, int H);
	~HtmlBrowser(void);
	void resize(int X, int Y, int W, int H); 
};


/*************************************************************************
* BrowserTabs class derived from Fl_Tabs for future expansion
**************************************************************************/

class BrowserTabs : public Fl_Tabs {
public:
	BrowserTabs(int X, int Y, int W, int H);
	~BrowserTabs(void);
	Fl_Group *BrowserGroup[MAXVIEWS]; //in case same label
	Fl_Group *firsttab;
	
	int ConstructTab(Fl_Widget *w, const char* text);
	static void TabCallback(Fl_Widget *w, void*);
	int handle(int e);   
};

/*************************************************************************
* GUI and callbacks
**************************************************************************/
void scroll_callback(Fl_Widget* w, void* data);
void scrollbarv_cb(Fl_Widget* w, void* data);
void scrollbarh_cb(Fl_Widget* w, void* data);

void cb_open(Fl_Widget*, void*);
void cb_save_as(Fl_Widget*, void*);
void cb_print(Fl_Widget*, void*);
void cb_set_startpage(Fl_Widget*, void*);
void cb_set_language(Fl_Widget*, void* v);
void cb_stop_xfer(Fl_Widget*, void*);
void cb_copy(Fl_Widget*, void*);
void cb_find(Fl_Widget*, void*);
void cb_showSource(Fl_Widget*, void*);
void cb_history(Fl_Widget*, void*);
void cb_add_bookmark(Fl_Widget*, void*);
void cb_bookmarks(Fl_Widget*, void*);

void cb_exit(Fl_Widget*, void*);
void cb_about(Fl_Widget*, void*);
void cb_help(Fl_Widget*, void*);
void cb_menu(Fl_Widget*, void*);
void cb_findnext(Fl_Widget* w, void*);
void context_menu_cb(int x, int y);
int make_ftpdir_page(char* ftpurl);

class GUI : public Fl_Window {
public:
	Fl_Menu_Item *mnu;
	Fl_Menu_Bar *mnu_bar;
	HtmlBrowser *browserwin[MAXVIEWS];
	Fl_Output *stat_bar;
	BrowserTabs *browsertabs;
	static GUI *self;

    Fl_Pixmap *p_right;
    Fl_Pixmap *p_left;
    Fl_Pixmap *p_down;
    Fl_Pixmap *p_up;
    
    static int lasttab;
    static int currenttab;
    
    url_text URL[MAXHISTORY]; //e.g. 25 structures
    url_text bookmarks[MAXBOOKMARKS];
    
    //Fl_Window *findwindow;
    Fl_Window *historywindow;
    Fl_Window *bookmarkswindow;
    char findtext[512];
    char startpage[1024];

	GUI(int X, int Y, int W, int H);
	~GUI();
	void create_layout(void);
	void create_menu(void);
	//int handle(int e);
	//void resize(int x, int y, int w, int h);
};

//declarations
    int GUI::lasttab=0;
    int GUI::currenttab=0;


const char blankpage[] = ""
"<html>\n"
" <head>\n"
" <title>NetRider Welcome page</title>\n"
" </head>\n"
" <body bgcolor=aqua text=purple>\n"
" <H1 align=center>Welcome to NetRider</H1>"

//" <br />\n"
//" <a href=\"http://www.google.com\" target=\"_blank\">Google</a>\n"
//" <br />\n"
" </body>\n"
"</html>\n";


#endif //netrider.h



