/****************************************************************************
 NetRider browser based on the WebKitNix webkit library. Copyright 2015 Georg Potthast
 
 The FLTK GUI consists of the class GUI which is a group for the menu bar,
 the tabs, the status bar and the browser window. The browser window is a 
 group for the icon bar and the view of the webpage. For each tab there is a
 browser window object and a WebKit object.
 There is a FLTK timeout loop which calls the timer callback. This trigges the
 glib loop.
 ****************************************************************************/
#include <ctype.h>
#include "netrider.h"

#include "xpm/menu.xpm"
#include "xpm/back.xpm"
#include "xpm/forward.xpm"
#include "xpm/stop.xpm"
#include "xpm/down.xpm"
#include "xpm/up.xpm"
#include "xpm/left.xpm" 
#include "xpm/right.xpm"
#include "xpm/go.xpm"
#include "xpm/searchtrans.xpm"
#include "xpm/reload.xpm" 
#include "xpm/bookmarks.xpm"
#include "xpm/netrider.xpm" 

/*******************************************************************************************
 * Utilities
*******************************************************************************************/
void write_to_statusbar(char* msg){
  	    GUI::self->stat_bar->value(msg);
	    GUI::self->stat_bar->redraw();
	    GUI::self->damage(FL_DAMAGE_CHILD);
	    Fl::flush();
}

std::string createStdStringFromWKString(WKStringRef wkStr)
{
    size_t wkStrSize = WKStringGetMaximumUTF8CStringSize(wkStr);
    std::string result;
    result.resize(wkStrSize + 1);
    size_t realSize = WKStringGetUTF8CString(wkStr, &result[0], result.length());
    if (realSize > 0)
        result.resize(realSize - 1);
    return result;
}

const char* get_url(){
    WKRetainPtr<WKURLRef> urlRef = adoptWK(WKPageCopyActiveURL(view[GUI::currenttab]->uiPage()));
    std::string url;
    static char buf[1024];
    if (urlRef) {
        WKRetainPtr<WKStringRef> urlStr = adoptWK(WKURLCopyString(urlRef.get()));
        url = createStdStringFromWKString(urlStr.get());
    }
    sprintf(buf,"%s",url.c_str());
    sprintf(GUI::self->browserwin[GUI::currenttab]->currenturl,"%s",buf); //save for cb_loc enter and print
    return (const char*)buf;
}    
 
void loadURL(const char* url)
{
  if (url==NULL) return;
    
    std::string fixedUrl(url);
    WKURLRef wkUrl = WKURLCreateWithUTF8CString(fixedUrl.c_str());
    WKPageLoadURL(view[GUI::currenttab]->uiPage(), wkUrl);
    WKRelease(wkUrl);
}

void getsource(WKStringRef sourcestring, WKErrorRef err_ref, void*){

  std::string messageString = createStdStringFromWKString(sourcestring);
  
  const char *f;
#if defined (__MINGW32__)	  
  f = fl_file_chooser(lt8("Save as ..."), "*.{html,htm,HTM}", NULL);
#else
  f = fl_file_chooser(lt8("Save as ..."), "*.{html,htm,HTM}", downloadsdir, 0);
#endif

  if (f) {
    std::ofstream ofs;
    ofs.open(f,std::ios::out | std::ios::trunc | std::ios::binary);
    ofs << messageString;
    ofs.close();
  }
  //std::cout << messageString;
}

void getSelectionOrContents(WKStringRef selectionstring, WKErrorRef, void*)
{
  std::string selectedtext = createStdStringFromWKString(selectionstring);
  //std::cout << selectedtext;
  Fl::copy(selectedtext.c_str(),strlen(selectedtext.c_str()),1);
}

void getsourcedata(WKDataRef sourcechars, WKErrorRef err_ref, void*){  
  
  const char *f;
#if defined (__MINGW32__)	  
  f = fl_file_chooser(lt8("Save as ..."), "*.{html,htm,HTM}", NULL);
#else
  f = fl_file_chooser(lt8("Save as ..."), "*.{html,htm,HTM}", downloadsdir, 0);
#endif

  if (f) {
    std::ofstream ofs;
    ofs.open(f,std::ios::out | std::ios::trunc | std::ios::binary);
    ofs.write((const char*)WKDataGetBytes(sourcechars),WKDataGetSize(sourcechars));
    ofs.close();
  }
}

void getsourceandshow(WKStringRef sourcestring, WKErrorRef err_ref, void*){

  std::string messageString = createStdStringFromWKString(sourcestring);

  Fl_Window *sourcewindow = new Fl_Window(GUI::self->x()+20,GUI::self->y()+20,GUI::self->w()-20,GUI::self->h()-20);
  Fl_Text_Buffer *buff = new Fl_Text_Buffer();
  Fl_Text_Editor *disp = new Fl_Text_Editor(10,10,GUI::self->w()-40,GUI::self->h()-40);
  disp->buffer(buff);
  sourcewindow->resizable(*disp);
  buff->text(messageString.c_str());
  sourcewindow->show();
}

/*******************************************************************************************
 * Callbacks
*******************************************************************************************/
/******************************************************************************************
WKViewClientV0 client;
*******************************************************************************************/
void drawnow(WebKit2* viewptr) { //called by viewNeedsDisplay
  GUI::self->browserwin[GUI::currenttab]->bview->dodraw(); //mygl->dodraw();  
}

void didChangeTooltip(WKViewRef, const WKStringRef tooltip, const void* clientInfo)
{
    std::string tooltipStr = createStdStringFromWKString(tooltip);
//FIXME   
    if (tooltipStr.empty()){
      write_to_statusbar((char*)"                                            ");
    } else {
      write_to_statusbar((char*)tooltipStr.c_str());
    }
}

/*******************************************************************************************
WKPageUIClientV2 uiClient;
*******************************************************************************************/
void runJavaScriptAlert(WKPageRef, WKStringRef message, WKFrameRef, const void*)
{
    std::string messageString = createStdStringFromWKString(message);
    fl_alert(messageString.c_str());
}

bool runJavaScriptConfirm(WKPageRef, WKStringRef message, WKFrameRef, const void*)
{
    std::string messageString = createStdStringFromWKString(message);
    if (fl_choice(messageString.c_str(), fl_no, fl_yes, NULL)) return 1;
    
    return 0; //fl_no
}

WKStringRef runJavaScriptPrompt(WKPageRef, WKStringRef message, WKStringRef defaultValue, WKFrameRef, const void*)
{
    std::string messageString = createStdStringFromWKString(message);
    std::string defaultString = createStdStringFromWKString(defaultValue);
    return WKStringCreateWithUTF8CString(fl_input(messageString.c_str(),defaultString.c_str()));
}

void mouseDidMoveOverElement(WKPageRef page, WKHitTestResultRef hitTestResult, WKEventModifiers modifiers, WKTypeRef userData, const void *clientInfo)
{
  WKRetainPtr<WKURLRef> urlRef = adoptWK(WKHitTestResultCopyAbsoluteLinkURL(hitTestResult));
    if (urlRef) {
        WKRetainPtr<WKStringRef> urlStr = adoptWK(WKURLCopyString(urlRef.get()));
	view[GUI::currenttab]->mouse_uri = createStdStringFromWKString(urlStr.get());
	//std::cout << view[GUI::currenttab]->mouse_uri << std::endl;
	//printf("%s\n",view[GUI::currenttab]->mouse_uri.c_str());
	write_to_statusbar((char*)view[GUI::currenttab]->mouse_uri.c_str());
	WKRetainPtr<WKStringRef> urltitle = adoptWK(WKHitTestResultCopyLinkLabel(hitTestResult));
	view[GUI::currenttab]->mouse_title = createStdStringFromWKString(urltitle.get());
	//std::cout << "Title:" << view[GUI::currenttab]->mouse_title << std::endl;
    } else {
      view[GUI::currenttab]->mouse_uri = "";
      view[GUI::currenttab]->mouse_title = "";
      write_to_statusbar((char*)"                                       ");
    }
}
void saveDataToFileInDownloadsFolder(WKPageRef page, WKStringRef suggestedFilename, WKStringRef mimeType, WKURLRef originatingURL, WKDataRef data, const void* clientInfo)
{
  printf("saveDataToFileInDownloadsFolder -");
    std::string url;
    static char buf[1024];
    if (originatingURL) {
        WKRetainPtr<WKStringRef> urlStr = adoptWK(WKURLCopyString(originatingURL));
        url = createStdStringFromWKString(urlStr.get());
    }
    std::cout << " URL:" << url << std::endl;
    //return ((const char*)url.c_str()); 
    //sprintf(buf,"%s",url.c_str());

}
/*******************************************************************************************
WKPageLoaderClientV3 loaderClient;
*******************************************************************************************/
void didReceiveTitleForFrame(WKPageRef, WKStringRef title, WKFrameRef frame, WKTypeRef, const void* clientInfo)
{
    std::string titleStr = createStdStringFromWKString(title);
    
    //GUI::self->browserwin[GUI::currenttab]->bview->top_window()->label(titleStr.c_str());
    GUI::self->label(titleStr.c_str());
    sprintf(GUI::self->browserwin[GUI::currenttab]->currenttitle,"%s",titleStr.c_str()); //save for print    
    
	  //update TAB title - will display after change of tabs
	  snprintf(GUI::self->browserwin[GUI::currenttab]->tabtitle,12,"%s",titleStr.c_str()); //clear  before loading new url
	  GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->label(GUI::self->browserwin[GUI::currenttab]->tabtitle);
	  GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->redraw_label(); //make the tabtitle shown
	  GUI::self->redraw();	  

	  //write into history
	  //check if already in history to avoid duplication
	  if (strcmp(GUI::self->URL[0].name,titleStr.c_str())!=0){ //if not equal then put into history array
	    //push entries down and write new entry into 0 item
	    for (int i = MAXHISTORY-1; i > 0; i--) {
	      sprintf(GUI::self->URL[i].name,"%s",GUI::self->URL[i - 1].name);
	      sprintf(GUI::self->URL[i].uri,"%s",GUI::self->URL[i - 1].uri);
	    }
	    sprintf(GUI::self->URL[0].name,"%s",titleStr.c_str()); //::view[GUI::currenttab]->getTitle());
	    sprintf(GUI::self->URL[0].uri,"%s",get_url());
	  }
 
}

void didStartProgress(WKPageRef, const void* clientInfo)
{
  write_to_statusbar((char*)"Trying to connect to server");
}

void didChangeProgress(WKPageRef page, const void* clientInfo)
{
  char msg[256];
    double value =  WKPageGetEstimatedProgress(page);
    sprintf(msg,"%.0f %c                   ",value*100,'%');
    write_to_statusbar(msg);
}

void didFinishDocumentLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void* clientInfo)
{ 
  //put URL into location field
  GUI::self->browserwin[GUI::currenttab]->tool->loc->value(get_url());  
}

void didFailProvisionalLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef, const void* clientInfo)
{
  static char buf[1024];
  int errorcode = WKErrorGetErrorCode(error);
  std::string errorString = createStdStringFromWKString(WKErrorCopyLocalizedDescription(error));
  
  sprintf(buf,"Failed to load page. Error code:%d, %s",errorcode,errorString.c_str());
  write_to_statusbar((char*)buf);
}

void didFailLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef userData, const void *clientInfo)
{
  static char buf[1024];
  int errorcode = WKErrorGetErrorCode(error);
  std::string errorString = createStdStringFromWKString(WKErrorCopyLocalizedDescription(error));
  
  sprintf(buf,"Failed to load page. Error code:%d, %s",errorcode,errorString.c_str());
  write_to_statusbar((char*)buf);
}

/*******************************************************************************************
NIXViewClientV0 nixClient;
*******************************************************************************************/
void didCommitLoadForMainFrame(WKViewRef view, const void* clientInfo)
{
}
/*******************************************************************************************
WKContextDownloadClientV0 downloadclient
*******************************************************************************************/
void didStart(WKContextRef context, WKDownloadRef download, const void *clientInfo)
{
  write_to_statusbar((char*)"Starting file download"); //printf("didstart\n");
  GUI::self->browserwin[GUI::currenttab]->bytesdownloaded=0; //clear
  GUI::self->browserwin[GUI::currenttab]->download_ref=download;
}
void didReceiveResponse(WKContextRef context, WKDownloadRef download, WKURLResponseRef response, const void *clientInfo)
{
  write_to_statusbar((char*)"Contacted server"); //printf("didReceiveResponse\n");
}
void didReceiveData(WKContextRef context, WKDownloadRef download, uint64_t length, const void *clientInfo)
{
  static char buf[1024];
  GUI::self->browserwin[GUI::currenttab]->bytesdownloaded += length;
  
  //format string below: ' for thousands-separator .0 for printing integer numbers only
  sprintf(buf,"Downloading file: %s   %'.0f bytes",GUI::self->browserwin[GUI::currenttab]->downloadfilename,GUI::self->browserwin[GUI::currenttab]->bytesdownloaded);
  write_to_statusbar((char*)buf);
}
WKStringRef decideDestinationWithSuggestedFilename(WKContextRef context, WKDownloadRef download, WKStringRef filename, bool* allowOverwrite, const void *clientInfo)
{
  std::string filenameString = createStdStringFromWKString(filename);
  //std::cout << filenameString << std::endl;

  //*allowOverwrite=true; //will maybe work in 2015 version of webkit - new API proposed 
  
  sprintf(GUI::self->browserwin[GUI::currenttab]->downloadfilename,"%s",filenameString.c_str());
  
  static char buf[1024];
  sprintf(buf,"%s%s",downloadsdir,filenameString.c_str());
  unlink(buf); //delete existing file first - allowOverwrite=true;
  
  sprintf(buf,"%s%s%s","file://",downloadsdir,filenameString.c_str());    
  return WKStringCreateWithUTF8CString(buf);
}
void didFinish(WKContextRef context, WKDownloadRef download, const void *clientInfo)
{
  static char buf[1024];
  char msg[1024];
  int x;

  sprintf(buf,"Download completed: %s   %'.0f bytes downloaded",GUI::self->browserwin[GUI::currenttab]->downloadfilename,GUI::self->browserwin[GUI::currenttab]->bytesdownloaded);
  write_to_statusbar((char*)buf);
  
  sprintf(buf,"%s%s",downloadsdir,GUI::self->browserwin[GUI::currenttab]->downloadfilename);
  //returns 0 if first button selected - so start with NO
  if (fl_choice(lt8("Do you want to run/open the file %s?"), fl_no, fl_yes, NULL, GUI::self->browserwin[GUI::currenttab]->downloadfilename)){
	if (!strncmp(buf+strlen(buf)-5,".html",5)) {
	  cb_open(NULL,(void*)buf);
	} else {
#if defined (__MINGW32__) 
	  sprintf(msg,"%s",GUI::self->browserwin[GUI::currenttab]->downloadfilename); //use full path now - just enter the file for windows
#else
	  sprintf(msg,"xdg-open %s",buf); //use full path now
#endif
	  x=system (msg);
	}
   } //fl_choice
  
}
void didFail(WKContextRef context, WKDownloadRef download, WKErrorRef error, const void *clientInfo)
{
  static char buf[1024];

  int errorcode = WKErrorGetErrorCode(error);
  std::string errorString = createStdStringFromWKString(WKErrorCopyLocalizedDescription(error));
  if (errorcode == 401) errorString = "Cannot write file to disk!";
  //std::cout << errorString << std::endl;
  
  sprintf(buf,"Failed to download file: %s. Error code:%d, %s",GUI::self->browserwin[GUI::currenttab]->downloadfilename,errorcode,errorString.c_str());
  write_to_statusbar((char*)buf);
  
}
void didCancel(WKContextRef context, WKDownloadRef download, const void *clientInfo)
{
  static char buf[1024];

  sprintf(buf,"Downloading file: %s canceled.   %'.0f bytes downloaded",GUI::self->browserwin[GUI::currenttab]->downloadfilename,GUI::self->browserwin[GUI::currenttab]->bytesdownloaded);
  write_to_statusbar((char*)buf);
}
/*******************************************************************************************
WKPagePolicyClientV1 policyclient
*******************************************************************************************/
void decidePolicyForNavigationAction(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKFrameRef originatingFrame, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
  printf("NavigationAction\n");
  WKFramePolicyListenerUse(listener);
}
void decidePolicyForNewWindowAction(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKURLRequestRef request, WKStringRef frameName, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
  printf("NewWindow\n");
  WKFramePolicyListenerUse(listener);
}
void decidePolicyForResponse(WKPageRef page, WKFrameRef frame, WKURLResponseRef response, WKURLRequestRef request, bool canShowMIMEType, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
if (GUI::self->browserwin[GUI::currenttab]->savetargetas==1) {
   GUI::self->browserwin[GUI::currenttab]->savetargetas=0;
   WKFramePolicyListenerDownload(listener);
   return;
}

    // If the URL Response has "Content-Disposition: attachment;" header, then
    // we should download it.
    if (WKURLResponseIsAttachment(response)) {
        WKFramePolicyListenerDownload(listener);
        return;
    }

    WKRetainPtr<WKStringRef> mimeType = adoptWK(WKURLResponseCopyMIMEType(response));
    if (WKFrameIsMainFrame(frame)) {
        if (canShowMIMEType) {
            WKFramePolicyListenerUse(listener);
            return;
        }
        // If we can't use (show) it then we should download it.
        WKFramePolicyListenerDownload(listener);
        return;
    }

    // We should ignore downloadable top-level content for subframes, with an exception for text/xml and application/xml so we can still support Acid3 test.
    // It makes the browser intentionally behave differently when it comes to text(application)/xml content in subframes vs. mainframe.
    bool isXMLType = WKStringIsEqualToUTF8CString(mimeType.get(), "text/xml") || WKStringIsEqualToUTF8CString(mimeType.get(), "application/xml");
    if (!canShowMIMEType && !isXMLType) {
        WKFramePolicyListenerIgnore(listener);
        return;
    }
  WKFramePolicyListenerUse(listener);
  
}
/*******************************************************************************************
 * Scrollbars
 * 
   The Browser constructor defines the scrollbar_v and scrollbar_h with the width of zero
   and sets the scrollbarv_cb, scrollbarh_cb callbacks for these.
   The Browser::resize function will adjust to the right sizes using the 
   scrollbar_vw, scrollbar_hw size which are initially zero.
   The showscrollbars() function below will check if the size of the webpage requires scrollbars
   and sets the width to 15 if scrollbars shall show up.
 *
*******************************************************************************************/
void getmaxscroll(WKStringRef maxscrollstring, WKErrorRef err_ref, void*){

  std::string messageString = createStdStringFromWKString(maxscrollstring);
  //std::cout << "getmaxscroll-string:" << messageString << std::endl;

  char seps[] = ",";
  char *token;
  int scrollxmax,scrollymax,tokencount=0;

  token = strtok( &messageString[0], seps );
  while( token != NULL )
  {
    if (tokencount==0) {
      scrollxmax = atoi(token);
    } else if (tokencount==1) {
      scrollymax = atoi(token);
    }    
    tokencount++;
    token = strtok( NULL, seps );
  }
  GUI::self->browserwin[GUI::currenttab]->maxscrollx= scrollxmax;
  GUI::self->browserwin[GUI::currenttab]->maxscrolly= scrollymax;
}

void positionscrollbars(WKStringRef maxscrollstring, WKErrorRef err_ref, void*){
  std::string messageString = createStdStringFromWKString(maxscrollstring);

  char seps[] = ",";
  char *token;
  int scrollx,scrolly,scrollxmax,scrollymax,tokencount=0;

  token = strtok( &messageString[0], seps );
  while( token != NULL )
  {
    if (tokencount==0) {
      scrollx = atoi(token);
    } else if (tokencount==1) {
      scrolly = atoi(token);
    } else if (tokencount==2) {
      scrollxmax = atoi(token);
    } else if (tokencount==3) {
      scrollymax = atoi(token);
    }    
    tokencount++;
    token = strtok( NULL, seps );
  }
  
	HtmlBrowser *display=GUI::self->browserwin[GUI::currenttab];
	//horizontal bar
	if (scrollxmax>0) {
	  display->scrollbar_h->value(scrollx);
	  display->resize(display->x(),display->y(),display->w(),display->h()); //redraw with new scrollbar
	}
	//vertical bar
	if (scrollymax>0) {
	  display->scrollbar_v->value(scrolly);
	  display->resize(display->x(),display->y(),display->w(),display->h()); //redraw with new scrollbar
	}
}

void showscrollbars(WKStringRef maxscrollstring, WKErrorRef err_ref, void*){

  //set scrollbars as required
  
  std::string messageString = createStdStringFromWKString(maxscrollstring);

  char seps[] = ",";
  char *token;
  int scrollx,scrolly,scrollxmax,scrollymax,tokencount=0;

  token = strtok( &messageString[0], seps );
  while( token != NULL )
  {
    if (tokencount==0) {
      scrollx = atoi(token);
    } else if (tokencount==1) {
      scrolly = atoi(token);
    } else if (tokencount==2) {
      scrollxmax = atoi(token);
    } else if (tokencount==3) {
      scrollymax = atoi(token);
    }    
    tokencount++;
    token = strtok( NULL, seps );
  }

  //fill these for printing the page - testing
  currmaxscrollx=scrollxmax;
  currmaxscrolly=scrollymax;
  
	HtmlBrowser *display=GUI::self->browserwin[GUI::currenttab];

	//this is only called by resize, so took out the calls to resize
	//vertical bar
	if (scrollymax==0 && display->scrollbar_vw != 0){
	  display->scrollbar_vw = 0;
	  //display->resize(display->x(),display->y(),display->w(),display->h()); //redraw without scrollbar
	} else if (scrollymax>0) {
	  display->scrollbar_vw = 15;
	  display->scrollbar_v->bounds(0,scrollymax);
	  display->scrollbar_v->slider_size((float)display->bview->h()/((float)scrollymax+(float)display->bview->h()));
	  display->scrollbar_v->value(scrolly);
	  //display->resize(display->x(),display->y(),display->w(),display->h()); //redraw with new scrollbar
	}
	//horizontal bar
	if (scrollxmax==0 && display->scrollbar_hw != 0){ 
	  display->scrollbar_hw = 0;
	  //display->resize(display->x(),display->y(),display->w(),display->h()); //redraw without scrollbar
	} else if (scrollxmax>0) {
	  display->scrollbar_hw = 15;
	  display->scrollbar_h->bounds(0,scrollxmax);
	  display->scrollbar_h->slider_size((float)display->bview->w()/((float)scrollxmax+(float)display->bview->w()));
	  display->scrollbar_h->value(scrollx);
	  //display->resize(display->x(),display->y(),display->w(),display->h()); //redraw with new scrollbar
	}	
	//display->bview->take_focus(); //direct keyboard entries to browser window - e.g. Ctrl-F for find
}
/*******************************************************************************************
 Mouse events
*******************************************************************************************/

int MyGlWindow::handle(int e){
//FLTK is rather flaky here, you may not return 'e'

    int ret = Fl_Gl_Window::handle(e); //workaround for FLTK bug - see erco cheat page
    NIXMouseEvent ev;
    NIXWheelEvent evw;
    const float pixelsPerStep = 40.0f;

    //for mouse wheel scrolling
  const int scrollGranularity = 48;
  int offsetX = 0;
  int offsetY = 0;
  static float zfactor=1; //one is webkit default
  char msg[124]; //output to status bar
    

    switch(e) {
	case FL_PUSH:
	  if (Fl::event_button() == 3) { //right button opens context menu	    
	    context_menu_cb(Fl::event_x(),Fl::event_y());
	  } else  if (Fl::event_button() == 8) { //Button 4
	    GUI::self->browserwin[GUI::currenttab]->tool->cb_back(this,NULL);
	  } else  if (Fl::event_button() == 9) { //Button 5
	    GUI::self->browserwin[GUI::currenttab]->tool->cb_forward(this,NULL);
	  } else {
	    take_focus();
	    ev.type = kNIXInputEventTypeMouseDown;
            ev.button = convertXEventButtonToNativeMouseButton(Fl::event_buttons());
	    m_lastClickButton = convertXEventButtonToNativeMouseButton(Fl::event_buttons());
            ev.x = Fl::event_x();
            ev.y = Fl::event_y();
            ev.globalX = 0; //event.x_root;
            ev.globalY = 0; //event.y_root;
            ev.clickCount = 1; //m_clickCount;
            ev.modifiers = 0; //convertXEventModifiersToNativeModifiers(event.state);
            ev.timestamp = 0; //convertXEventTimeToNixTimestamp(event.time);
            NIXViewSendMouseEvent(view[GUI::currenttab]->uiView(), &ev);
            return 1;
	  }
	
	case FL_RELEASE:
        ev.type = kNIXInputEventTypeMouseUp;
        ev.button = convertXEventButtonToNativeMouseButton(Fl::event_buttons());
	m_lastClickButton=0; //just handle moving pressed button
        ev.x = Fl::event_x();
        ev.y = Fl::event_y();
        ev.globalX = 0; //event.x_root;
        ev.globalY = 0; //event.y_root;
        ev.clickCount = 1; //m_clickCount;
        ev.modifiers = 0; //convertXEventModifiersToNativeModifiers(event.state);
        ev.timestamp = 0; //convertXEventTimeToNixTimestamp(event.time);
        NIXViewSendMouseEvent(view[GUI::currenttab]->uiView(), &ev);
	return 1;
	
	case FL_MOVE:
	ev.type = kNIXInputEventTypeMouseMove;
	ev.button = m_lastClickButton;
        ev.x = Fl::event_x();
        ev.y = Fl::event_y();
        ev.globalX = 0; //event.x_root;
        ev.globalY = 0; //event.y_root;
        ev.clickCount = 1; //m_clickCount;
        ev.modifiers = 0; //convertXEventModifiersToNativeModifiers(event.state);
        ev.timestamp = 0; //convertXEventTimeToNixTimestamp(event.time);
        NIXViewSendMouseEvent(view[GUI::currenttab]->uiView(), &ev);

	    return 1;
	    
	case FL_MOUSEWHEEL: 
	  //zoom view
	  if (Fl::event_state() == FL_CTRL){
	    if (Fl::event_dy()>0) {
	      zfactor=zfactor-0.05; //down
	    } else if (Fl::event_dy()<0) {
	      zfactor=zfactor+0.05; //dup
	    }
	    WKPageSetPageZoomFactor(view[GUI::currenttab]->uiPage(), zfactor);
	    
	    //show result in status bar
	    sprintf(msg,lt8("Zoom set to: %.2f"),zfactor); //two digits behind comma
	    GUI::self->stat_bar->value(msg);
	    GUI::self->stat_bar->redraw();
	    GUI::self->damage(FL_DAMAGE_CHILD);
	    Fl::flush();

	    return 1;
	  }

	  
	  evw.type = kNIXInputEventTypeWheel;
	  evw.modifiers = 0; //convertXEventModifiersToNativeModifiers(event.state);
	  evw.timestamp = 0; //convertXEventTimeToNixTimestamp(event.time);
	  evw.x = Fl::event_x();
	  evw.y = Fl::event_y();
	  evw.globalX = 0; //event.x_root;
	  evw.globalY = 0; //event.y_root;
	  if (Fl::event_dy()>0) { //down
	      evw.delta = pixelsPerStep * -1;
	      evw.orientation = kNIXWheelEventOrientationVertical;
	  } else if (Fl::event_dy()<0) { //up
	      evw.delta = pixelsPerStep * 1;
	      evw.orientation = kNIXWheelEventOrientationVertical;
	  }
	  if (Fl::event_dx()>0) { //right
	      evw.delta = pixelsPerStep * -1;
	      evw.orientation = kNIXWheelEventOrientationHorizontal;
	  } else if (Fl::event_dx()<0) { //left
	      evw.delta = pixelsPerStep * -1;
	      evw.orientation = kNIXWheelEventOrientationHorizontal;
	  }
	  NIXViewSendWheelEvent(view[GUI::currenttab]->uiView(), &evw);

	  return 1;

	case FL_FOCUS: 
	        return(1); //or browser window looses focus!
        case FL_UNFOCUS: 
	  //clear_visible_focus();
                return(1); //or browser window looses focus!
	case FL_ENTER: 
	    //view[GUI::currenttab]->take_focus();//setFocus(true);
	  GUI::self->browserwin[GUI::currenttab]->bview->take_focus();
	  return 1;
	case FL_LEAVE:
	  //Fl::focus(GUI::self->browserwin[GUI::self->currenttab]->tool->loc);
	  //return 1;
	    break;
	  
	//case FL_KEYUP:
	case FL_SHORTCUT: //FLTK returns this instead of FL_KEYDOWN?
		return handle_key(e,Fl::event_key());	    
	default:
	  //printf("fl-%s\n",fl_eventnames[e]);
	  //fflush(stdout);
	  break;
	}
	//return Fl_Window::handle(e);
	return(ret);
}

/*******************************************************************************************
 * Keyboard events
*******************************************************************************************/

int MyGlWindow::handle_key(int e,int key)
{

    NIXKeyEvent nixEvent;
    memset(&nixEvent, 0, sizeof(NIXKeyEvent));
    nixEvent.type = kNIXInputEventTypeKeyDown;
    nixEvent.modifiers = convertXEventModifiersToNativeModifiers(Fl::event_state());
    nixEvent.timestamp = 0; //convertXEventTimeToNixTimestamp(event->time);
    nixEvent.shouldUseUpperCase = 0; //useUpperCase;
    if (isupper(Fl::event_text()[0]) && isalpha(Fl::event_text()[0])){
      //printf("upper\n");
      nixEvent.shouldUseUpperCase = 1;}
    nixEvent.isKeypad = 0; //isKeypadKeysym(symbol);
    nixEvent.key = convertXKeySymToNativeKeycode(Fl::event_key()); //need this for the switch below

    //printf("key:%d,nixEvent.key:%d,keysym:%d,text:%s,len:%d\n",key,nixEvent.key,Fl::event_key(), Fl::event_text(),Fl::event_length());

    //keys+ctrl
    if (nixEvent.modifiers & kNIXInputEventModifiersControlKey) {
      
      if (key=='s') {WKPageStopLoading(view[GUI::currenttab]->uiPage()); return 1;}

      if (key=='r') {WKPageReload(view[GUI::currenttab]->uiPage()); return 1;}

      if (key=='f') { cb_find(this,NULL); return 1;}

      if (key=='t') { //open new tab
	GUI::self->browsertabs->value(GUI::self->browsertabs->BrowserGroup[GUI::self->lasttab]);//GUI::self->lasttab
	GUI::self->browsertabs->TabCallback(GUI::self->browsertabs,NULL);    
	//GUI::self->redraw();
	return 1;
      } //key=t

      if (key=='w') { //close current tab      
        if (GUI::currenttab==0) return 1;
	if (GUI::currenttab==GUI::self->lasttab) return 1; //don't delete the "+"
	int ctab = GUI::currenttab; //to delete after change to first tab
	if (fl_choice(lt8("Do you want to close the current Tab?"), fl_no, fl_yes, NULL, NULL)){
	  GUI::self->browsertabs->value(GUI::self->browsertabs->BrowserGroup[0]); //jump to first tab before deleting
	  GUI::currenttab=0; //or crash  
	  delete GUI::self->browsertabs->BrowserGroup[ctab];
	  GUI::self->redraw();
	  //the focus is on tab one then - select next tab before ctrl-w
	  Fl::focus(GUI::self->browsertabs->BrowserGroup[0]);
	  //view[GUI::currenttab]->setFocus(true);
	  GUI::self->browserwin[GUI::currenttab]->bview->take_focus();
	  return 1;
	}
      } //key=w      
    } //CTRL	

    
    if (nixEvent.modifiers & kNIXInputEventModifiersAltKey) {
      //go back if ALT-Cursor left
      if (key==FL_Left) { 
      if (WKPageCanGoBack(view[GUI::currenttab]->uiPage()))
	   WKPageGoBack(view[GUI::currenttab]->uiPage());
	  return 1; 
      } else if (key==FL_Right) { 
        if (WKPageCanGoForward(view[GUI::currenttab]->uiPage()))
		WKPageGoForward(view[GUI::currenttab]->uiPage());
	return 1; 
      }
     } //ALT
    
#if testsoup
SoupSession *session;
SoupMessage *msg;
#endif

WKSize size;
WKSize visibleContentsSize;
WKPoint position;
std::string url;
    
    switch (nixEvent.key) {
      case kNIXKeyEventKey_F1:
	fl_message("NixFLTKWebLauncher Help Page\n\n"
	    "F1 - this help screen\n"
	    "F2 - enter URL\n"
	    "F3 - bookmark www.google.com\n"
	    "F4 - bookmark www.wikipedia.com\n"  //for mouse wheel scrolling
	    "F5 - redraw screen\n"
	    "Alt-Cursor left - go back one web page\n"
	    "Alt-Cursor right - go forward one web page\n"
	);
	return 1;
      case kNIXKeyEventKey_F2:
	const char* newurl;
	newurl = fl_input("URL:","http://");
	//if (newurl != NULL) //done in function now
	loadURL(newurl);
	return 1;
      case kNIXKeyEventKey_F3:
	loadURL("http://www.google.com");
	return 1;
      case kNIXKeyEventKey_F4:
	loadURL("http://www.wikipedia.com");
	return 1;
      case kNIXKeyEventKey_F5:
	GUI::self->browserwin[GUI::currenttab]->resize(0,20,577,719); //resize to page size
	//WKPageReload(view[GUI::currenttab]->uiPage());
#if 0
	size = WKViewGetSize(view[GUI::currenttab]->uiView());
	printf("size.width=%d,size.height=%d\n",size.width,size.height);
	
	visibleContentsSize = NIXViewVisibleContentsSize(view[GUI::currenttab]->uiView());
	printf("vsize.width=%d,vsize.height=%d\n",visibleContentsSize.width,visibleContentsSize.height);
	
	position = WKViewGetContentPosition(view[GUI::currenttab]->uiView());
	printf("pos.x=%d,pos.y=%d\n",position.x,position.y);
	//scrollBy(1,1);
#endif
	
        return 1;
	case kNIXKeyEventKey_F6:
	  
	  //WKPageGetMaxScrollForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), 
		//		   (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), getmaxscroll);
	  return 1;
	case kNIXKeyEventKey_F7: 

	  //   WKPageGetScrollPosForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), 
	//			   (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), getmaxscroll);
	//WKPageGetScrollPosForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), 
	//	  (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), showscrollbars); 

	  
	  return 1;
	case kNIXKeyEventKey_F8:
#if testsoup	  
	  session = soup_session_sync_new();
	  msg = soup_message_new ("GET", "http://www.distasis.com/note.html");
	  soup_session_send_message (session, msg);
	  fwrite (msg->response_body->data,1,msg->response_body->length,stdout);
#endif	  
	  return 1;
      
	default:
#if 1
	  //if enter key check if something entered into URL input field. If yes, call cb_loc for that
	  if (Fl::event_key() == FL_Enter){
      
	    if (strcmp(GUI::self->browserwin[GUI::currenttab]->currenturl,GUI::self->browserwin[GUI::currenttab]->tool->loc->value())!=0){	
	      //printf("unequal\n");
	      GUI::self->browserwin[GUI::currenttab]->tool->cb_loc(GUI::self->browserwin[GUI::currenttab]->tool->loc,NULL);        
	      return 1;
	    }else if (strlen(GUI::self->browserwin[GUI::currenttab]->tool->search->value())!=0){
	      GUI::self->browserwin[GUI::currenttab]->tool->cb_search(GUI::self->browserwin[GUI::currenttab]->tool->search,NULL);        
	      return 1;
	    } //strcmp
	  } 
#endif 
	if (nixEvent.key > 16700000 & nixEvent.key <30000000){ //try to grab control and cursor keys - above are UTF8 keys
	  NIXViewSendKeyEvent(view[GUI::currenttab]->uiView(), &nixEvent);

	  if ( nixEvent.key == kNIXKeyEventKey_Home | nixEvent.key == kNIXKeyEventKey_End | nixEvent.key == kNIXKeyEventKey_PageUp | nixEvent.key == kNIXKeyEventKey_PageDown ){
	      WKPageGetScrollPosForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), 
		  (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), positionscrollbars);
	  }
	  return 1; //pass to URL input widget - important to trigger with enter key!
	}
	break;
    }
    
    //to send shifted number keys and other shifted non-alpha to webkit
    if (Fl::event_length() == 1){
      nixEvent.key = convertXKeySymToNativeKeycode(Fl::event_text()[0]);
    } else {
      nixEvent.key = convertXKeySymToNativeKeycode(Fl::event_key());
      nixEvent.text = Fl::event_text(); //this makes UTF8 chars working
    }
    NIXViewSendKeyEvent(view[GUI::currenttab]->uiView(), &nixEvent);
    
    return 1;
}

/*******************************************************************************************
WebKit2 members - functions
*******************************************************************************************/
WebKit2::WebKit2(const std::vector<std::string>& urls)
    : m_initialUrls(urls)
{
    initUi();
  
}

WebKit2::~WebKit2()
{
    WKRelease(m_uiView);
    WKRelease(m_uiContext);
}

void WebKit2::initUi()
{
    m_uiContext = WKContextCreate();

    WKStringRef wkStr = WKStringCreateWithUTF8CString("Browser");
    m_uiPageGroup = WKPageGroupCreateWithIdentifier(wkStr);
    WKRelease(wkStr);

    m_uiView = WKViewCreate(m_uiContext, m_uiPageGroup);

    WKViewClientV0 client;
    std::memset(&client, 0, sizeof(WKViewClientV0));
    client.base.version = 0;
    client.base.clientInfo = this;
    client.viewNeedsDisplay = [](WKViewRef, WKRect, const void* client) {
	drawnow((WebKit2*)client);
    };
    client.webProcessCrashed = [](WKViewRef, WKURLRef, const void*) {
        puts("UI Webprocess crashed!\n -> Please set environment variables!");
    };
    client.didChangeTooltip = didChangeTooltip;
    WKViewSetViewClient(m_uiView, &client.base);

    NIXViewClientV0 nixClient;
    std::memset(&nixClient, 0, sizeof(nixClient));
    nixClient.base.version = 0;
    nixClient.base.clientInfo = GUI::self->browserwin[GUI::currenttab]->bview; //mygl;
    nixClient.didCommitLoadForMainFrame = didCommitLoadForMainFrame;
#if 0
    nixClient.setCursor = [](WKViewRef, unsigned shape, const void* window) {
        ((DesktopWindow*)window)->setMouseCursor(shape);
    };
#endif    
    NIXViewSetNixViewClient(m_uiView, &nixClient.base);

    m_uiPage = WKViewGetPage(m_uiView);

    WKPageUIClientV2 uiClient;
    memset(&uiClient, 0, sizeof(uiClient));
    uiClient.base.version = 2;
    uiClient.base.clientInfo = this;
    uiClient.runJavaScriptAlert = runJavaScriptAlert;
    uiClient.runJavaScriptConfirm = runJavaScriptConfirm;
    uiClient.runJavaScriptPrompt = runJavaScriptPrompt;
    uiClient.mouseDidMoveOverElement = mouseDidMoveOverElement; 
#if 0    
    uiClient.saveDataToFileInDownloadsFolder = saveDataToFileInDownloadsFolder;
#endif    
    WKPageSetPageUIClient(m_uiPage, &uiClient.base);
    
    WKPageLoaderClientV3 loaderClient;
    memset(&loaderClient, 0, sizeof(WKPageLoaderClientV3));
    loaderClient.base.version = 3;
    loaderClient.base.clientInfo = this;
    loaderClient.didReceiveTitleForFrame = didReceiveTitleForFrame;
    loaderClient.didStartProgress = didStartProgress;
    loaderClient.didChangeProgress = didChangeProgress;
    loaderClient.didFinishDocumentLoadForFrame = didFinishDocumentLoadForFrame;
    loaderClient.didFailProvisionalLoadWithErrorForFrame = didFailProvisionalLoadWithErrorForFrame;
    loaderClient.didFailLoadWithErrorForFrame = didFailLoadWithErrorForFrame;
    WKPageSetPageLoaderClient(m_uiPage, &loaderClient.base);
    
    WKContextDownloadClientV0 downloadclient;
    memset(&downloadclient, 0, sizeof(WKContextDownloadClientV0));
    downloadclient.base.version = 0;
    downloadclient.base.clientInfo = this;
    downloadclient.didStart = didStart;
    downloadclient.didReceiveResponse = didReceiveResponse;
    downloadclient.didReceiveData = didReceiveData;
    downloadclient.decideDestinationWithSuggestedFilename = decideDestinationWithSuggestedFilename;
    downloadclient.didFinish = didFinish;
    downloadclient.didFail = didFail;
    downloadclient.didCancel = didCancel;
    WKContextSetDownloadClient(m_uiContext, &downloadclient.base);
    
    WKPagePolicyClientV1 policyclient;
    memset(&policyclient, 0, sizeof(WKPagePolicyClientV1));
    policyclient.base.version = 1;
    policyclient.base.clientInfo = this;
    //policyclient.decidePolicyForNewWindowAction = decidePolicyForNewWindowAction;
    //policyclient.decidePolicyForNavigationAction = decidePolicyForNavigationAction;
    policyclient.decidePolicyForResponse = decidePolicyForResponse;
    WKPageSetPagePolicyClient(m_uiPage, &policyclient.base);

    WKViewInitialize(m_uiView);
    WKViewSetIsFocused(m_uiView, true);
    WKViewSetIsVisible(m_uiView, true);
    WKViewSetSize(m_uiView,WKSizeMake(GUI::self->browserwin[GUI::currenttab]->bview->w(),GUI::self->browserwin[GUI::currenttab]->bview->h())); //(mygl->w(),mygl->h()));

    WKURLRef wkUrl;
    if (m_initialUrls.empty())
        wkUrl = WKURLCreateWithUTF8CString("http://www.google.com");
    else {
        const std::string& url = m_initialUrls[0];
	checkurl((char*) url.c_str());
        wkUrl = WKURLCreateWithUTF8CString(url.c_str());
    }
    //only for first tab
    if (GUI::self->currenttab == 0) WKPageLoadURL(WKViewGetPage(m_uiView), wkUrl);
    WKRelease(wkUrl);

    wkStr = WKStringCreateWithUTF8CString("Content");
    m_contentPageGroup = WKPageGroupCreateWithIdentifier(wkStr);
    WKRelease(wkStr);
    WKPreferencesRef webPreferences = WKPageGroupGetPreferences(m_contentPageGroup);

    //WKPreferencesSetWebAudioEnabled(webPreferences, true);
    //WKPreferencesSetWebGLEnabled(webPreferences, true);
    WKPreferencesSetDeveloperExtrasEnabled(webPreferences, true);
  
    WKPreferencesSetFrameFlatteningEnabled(webPreferences, true);
    WKPreferencesSetOfflineWebApplicationCacheEnabled(webPreferences, true);
    WKPreferencesSetInteractiveFormValidationEnabled(webPreferences, true);
    
    WKPreferencesSetWebSecurityEnabled(webPreferences, false);
}


/*************************************************************************
* Timer callback
**************************************************************************/
static void Timer_CB(void *userdata) {
        g_main_context_iteration(0,0);
	//don't let these widgets eat keys like cursor, space, enter
	GUI::self->browserwin[GUI::self->currenttab]->tool->clear_visible_focus();
	GUI::self->browserwin[GUI::currenttab]->clear_visible_focus();
	//printf("t "); fflush(stdout);
        //Fl::repeat_timeout(1.0/44.0, Timer_CB, userdata);       // 24fps
	Fl::repeat_timeout(0.01, Timer_CB, userdata);  
}

/*************************************************************************
* Navigation
**************************************************************************/

void checkurl(char *url)
{
//add "http://" if not entered into URL
  
        char tmpurl[512];
        sprintf(tmpurl,"%s",url);

        if (!strncmp(url,"ftp://",6)) return; //ok
        if (!strncmp(url,"FTP://",6)) return; //ok

        if (!strncmp(url,"http://",7)) return; //ok
        if (!strncmp(url,"HTTP://",7)) return; //ok

        //above checks :// to so check here
        if (!strncmp(url,"https://",8)) return; //ok
        if (!strncmp(url,"HTTPS://",8)) return; //ok

        //add http:// now
        sprintf(url,"http://%s",tmpurl);

        return; //URL is passed as pointer and modified
}

HtmlNavigation::HtmlNavigation(HtmlBrowser *e, int X, int Y, int W, int H) : 
	Fl_Group(X, Y, W, H)
{
	int i;
	box(FL_UP_BOX);
        Y += 2;
        H -= 4;
        W = H;
        X += 1;

	browser = e;
	menu = new Fl_Button(X, Y, W, H); X += W + 1;
        back = new Fl_Button(X, Y, W, H); X += W + 1;
        forward = new Fl_Button(X, Y, W, H); X += W + 1;
        stop = new Fl_Button(X, Y, W, H); X += W + 1;
	reload = new Fl_Button(X, Y, W, H); X += W + 1;

	loc = new Fl_Input(X, Y, w() - (X - x()) - 278, H); X += w() - (X - x()) - 278 +1;
	
	go = new Fl_Button(X, Y, W, H); X += W;
        search = new Fl_Input(X, Y, w() - (X - x()) - 58, H); //X += 142 +1;
	searchbutton = new Fl_Button(w()-H -H -2, Y, W, H);
	bookmarks = new Fl_Button(w()-H-1, Y, W, H);
        end();
	    resizable(loc);

	p_menu = new Fl_Pixmap(menu_xpm);
        p_back = new Fl_Pixmap(back_xpm);
        p_forward = new Fl_Pixmap(forward_xpm);
        p_stop = new Fl_Pixmap(stop_xpm);
	p_reload = new Fl_Pixmap(reload_xpm);
	p_go = new Fl_Pixmap(go_xpm);
	p_search = new Fl_Pixmap(searchtrans_xpm);
	p_bookmarks = new Fl_Pixmap(bookmarks_xpm);

 
	p_menu->label(menu);
        p_back->label(back);
        p_forward->label(forward);
        p_stop->label(stop);
	p_reload->label(reload);	
        p_go->label(go);
	p_search->label(searchbutton);
	p_bookmarks->label(bookmarks);
        
	menu->callback(::cb_menu);
	//menu->tooltip(lt8("Open menu"));
	menu->tooltip(("Open menu"));
	back->callback(cb_back);
	back->tooltip(("Move back one page"));
	forward->callback(cb_forward);
	forward->tooltip(("Move forward one page"));
	stop->callback(cb_stop);
	stop->tooltip(("Stop loading page"));
	reload->callback(cb_reload);
	reload->tooltip(("Reload page"));
	
	loc->callback(cb_loc);
	
	search->callback(cb_search); //if enter key pressed
	
	go->tooltip(("Start loading URL"));
	go->callback(cb_loc,(void*)e);

	searchbutton->tooltip(("Search with Google"));
	searchbutton->callback(cb_search);

	
	bookmarks->tooltip(("Show bookmarks"));
	bookmarks->callback(::cb_bookmarks,(void*)1); //indicate short window
	

	
	for (i = 0; i < 10; i++) {
		history[i] = NULL;
	}
}


HtmlNavigation::~HtmlNavigation()
{
}


void HtmlNavigation::cb_loc(Fl_Widget *w, void *data)
{
  char buf[1024];
  const char* url = buf;

  sprintf(buf,"%s",GUI::self->browserwin[GUI::currenttab]->tool->loc->value());

  if (strlen(buf)<3) return; //must be invalid
  if (strcmp(buf,"http://")==0) return;
  checkurl((char*) buf);
  WKPageStopLoading(view[GUI::currenttab]->uiPage());
  
  snprintf(GUI::self->browserwin[GUI::currenttab]->tabtitle,12,"%s",lt8("Loading...  ")); //clear before loading new url
  GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->label(GUI::self->browserwin[GUI::currenttab]->tabtitle);
  GUI::self->redraw();
  ::loadURL(url);
}

void HtmlNavigation::cb_search(Fl_Widget *w, void *data)
{
  char buf[1024];
  const char* url = buf;
  int x;

  sprintf(buf,"http://www.google.com/search?q=%s",GUI::self->browserwin[GUI::currenttab]->tool->search->value());
  for(x = 0; x < strlen(buf); x++){
    if (buf[x]==' ')buf[x] = '+'; //replace any blank with '+' for Google query
  } 
  snprintf(GUI::self->browserwin[GUI::currenttab]->tabtitle,12,"%s",lt8("Loading...  ")); //clear  before loading new url
  GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->label(GUI::self->browserwin[GUI::currenttab]->tabtitle);
  GUI::self->redraw();
  ::loadURL(url);
}


void HtmlNavigation::cb_back(Fl_Widget *w, void *data)
{
    if (WKPageCanGoBack(view[GUI::currenttab]->uiPage()))
		WKPageGoBack(view[GUI::currenttab]->uiPage());
}

void HtmlNavigation::cb_forward(Fl_Widget *w, void *data)
{
     if (WKPageCanGoForward(view[GUI::currenttab]->uiPage()))
		WKPageGoForward(view[GUI::currenttab]->uiPage());
}

void HtmlNavigation::cb_stop(Fl_Widget *w, void *data)
{
  WKPageStopLoading(view[GUI::currenttab]->uiPage()); 
}

void HtmlNavigation::cb_reload(Fl_Widget *w, void *data)
{
  WKPageReload(view[GUI::currenttab]->uiPage());
}

/*************************************************************************
* OpenGL Window
**************************************************************************/

void MyGlWindow::dodraw() {
  HtmlBrowser *display=GUI::self->browserwin[GUI::currenttab];
  display->resize(display->x(),display->y(),display->w(),display->h()); //redraw needed to display page - add scrollbars here!
  /*
  if (strlen(GUI::self->browserwin[GUI::currenttab]->tool->loc->value()) > 8) { //do not make IPC call if no web page loaded - crash!!
	WKPageGetScrollPosForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), 
		  (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), showscrollbars); 
  }
  */
  draw();
}

void MyGlWindow::draw() {
  WKSize size = WKSizeMake(w(),h());
  WKViewSetSize(view[GUI::currenttab]->uiView(), size);

	glLoadIdentity();
        glViewport(0,0,w(),h());
        glOrtho(-w(),w(),-h(),h(),-1,1);

	//glViewport(0,0,w(),h());
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    WKViewPaintToCurrentGLContext(view[GUI::currenttab]->uiView());

    redraw();
}
    
void MyGlWindow::resize(int X,int Y,int W,int H) {
        Fl_Gl_Window::resize(X,Y,W,H);
	
	WKSize size = WKSizeMake(w(),h());
	WKViewSetSize(view[GUI::currenttab]->uiView(), size);

        glLoadIdentity();
        glViewport(0,0,W,H);
        glOrtho(-W,W,-H,H,-1,1);

	//glViewport(0,0,w(),h());
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	WKViewPaintToCurrentGLContext(view[GUI::currenttab]->uiView());

	WKPageGetScrollPosForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), 
	  (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), showscrollbars); 

	redraw();
}

/*************************************************************************
* Browser Group
**************************************************************************/

HtmlBrowser::HtmlBrowser(int X, int Y, int W, int H) : Fl_Group(X, Y, W, H)
{
  	scrollbar_vw=0;
	scrollbar_hw=0;
	savetargetas=0;

	tool = new HtmlNavigation(this, X, Y, W, 33);
	tool->end();

	bview = new MyGlWindow(X, Y+35, W -scrollbar_vw, H-35 -scrollbar_hw); //resize below will determine this!
	bview->end();
	
	scrollbar_v = new Fl_Scrollbar(X+W -scrollbar_vw,Y+35,scrollbar_vw,H-35 -scrollbar_hw);
	scrollbar_v->type(FL_VERTICAL);
	scrollbar_v->slider_size(.04);
	scrollbar_v->callback(scrollbarv_cb);
	scrollbar_h = new Fl_Scrollbar(X,Y+35+H-34 -scrollbar_hw,W -scrollbar_vw,scrollbar_hw);
	scrollbar_h->type(FL_HORIZONTAL);
	scrollbar_h->slider_size(.03);
	scrollbar_h->callback(scrollbarh_cb);
}

HtmlBrowser::~HtmlBrowser()
{
}

void HtmlBrowser::resize(int X, int Y, int W, int H)
{
	Fl_Group::resize(X, Y, W, H);
	tool->resize(X, Y, W, 33);
	bview->resize(X, Y + 35, W -scrollbar_vw, H-35 -scrollbar_hw);
	scrollbar_v->resize(X+W -scrollbar_vw,Y+35,scrollbar_vw,H-35);
	scrollbar_h->resize(X,Y+35+H-34 -scrollbar_hw,W -scrollbar_vw,scrollbar_hw);
	if (findwindow) //position find window just above status bar
	    findwindow->resize(GUI::self->x(),GUI::self->y()+GUI::self->h()-40, 280, 20); 
}

/*************************************************************************
* BrowserTabs class methods
**************************************************************************/

BrowserTabs::BrowserTabs(int X, int Y, int W, int H) : Fl_Tabs(X, Y, W, H)
{
}

BrowserTabs::~BrowserTabs()
{
}

int BrowserTabs::handle(int e)
{
  if ( e == FL_KEYBOARD && ( Fl::event_key() == FL_Left || Fl::event_key() == FL_Right || Fl::event_key() == 32 || Fl::event_key() == FL_Enter ) ) {    
    return GUI::self->browserwin[GUI::currenttab]->bview->handle_key(e,Fl::event_key());
  }
  return Fl_Tabs::handle(e); 
 
}

/*************************************************************************
* BrowserTabs implementation
**************************************************************************/
//at the beginning the currenttab is zero and the lastab ("+") is one

// Construct a new tab with contents
int BrowserTabs::ConstructTab(Fl_Widget *w, const char* text) {
    Fl_Tabs *browsertabs = (Fl_Tabs*)w;
    Fl_Group *o;
    
    GUI::self->lasttab++;
    int currentnr = GUI::self->lasttab;
    
    browsertabs->begin(); //or no new tab generated
    BrowserGroup[currentnr] = o = new Fl_Group(0,browsertabs->y()+20,browsertabs->w(),browsertabs->h()-20,text);
	GUI::self->browserwin[currentnr] = new HtmlBrowser(o->x(),o->y(),o->w(),o->h());
	GUI::self->browserwin[currentnr]->end();
	
    o->resizable(GUI::self->browserwin[currentnr]);
    o->end();
    browsertabs->when(FL_WHEN_NOT_CHANGED|FL_WHEN_CHANGED); 
    browsertabs->end();
    return currentnr;
}

// Callback for whenever a tab is clicked
void BrowserTabs::TabCallback(Fl_Widget *w, void*) {
  BrowserTabs *tabs = (BrowserTabs*)w;          // tabs widget
  Fl_Group *tab = (Fl_Group*)tabs->value();     // group clicked
  int ctab;
  
  // Pushed on the "+" tab?
  if ( strcmp(tab->label(), "+") == 0 ) {
    if (GUI::self->lasttab >= MAXVIEWS) return;
    
    // Adjust the "+" tab group to become a new container like the others..
    tab->label(lt8("New Tab     ")); //12 chars wide
    ctab = tabs->ConstructTab(tabs,"+") - 1;  //lasttab is two first since firsttab+"+" tab present

    //make new webview object now
    GUI::self->currenttab=ctab; //need this in WebKit2 init()
    view[ctab] = new WebKit2(args);
    
    //open as blank now //::view[ctab]->loadURL(defaulturl);
    Fl::focus(GUI::self->browserwin[GUI::self->currenttab]->tool->loc);
    GUI::self->browserwin[GUI::self->currenttab]->tool->loc->insert("http://"); 

    
  } else if (Fl::event_button() == FL_RIGHT_MOUSE) {    
    //delete tab - cannot delete TAB that has the focus if not FLTK 1.3.3!
    if (tabs->BrowserGroup[0] == tab) return;
    
    //returns 0 if first button selected - so start with NO
    if (fl_choice(lt8("Do you want to close the '%s' Tab?"), fl_no, fl_yes, NULL, tab->label())){ 
        tabs->value(tabs->BrowserGroup[0]); //jump to first tab before deleting
	GUI::currenttab=0; //or crash
	//find tab in array
	for (int i=0;i<MAXVIEWS;i++){ 
	  if (tab==tabs->BrowserGroup[i]){
	  pixelbuffer[i]=NULL; //stop timeout callback
	  break;      
	  } //if tab
	}  //for
        delete tab; //sic!!
        //view[GUI::currenttab]->setFocus(true);
        GUI::self->browserwin[GUI::currenttab]->bview->take_focus();
    }
  } else {
  //find tab in array
    for (int i=0;i<MAXVIEWS;i++){ 
      if (tab==tabs->BrowserGroup[i]){
	GUI::currenttab=i;

	std::string titleStr = createStdStringFromWKString(WKPageCopyTitle(view[GUI::currenttab]->uiPage()));
	//sprintf(GUI::self->bookmarks[0].name,"%s",titleStr.c_str());
	GUI::self->label(titleStr.c_str());	
	GUI::self->browserwin[GUI::currenttab]->tool->loc->value(get_url());   
	break;      
      } //if tab
    }  //for

  } //if/else
  GUI::self->redraw();   
}

void openURL_in_newtab(Fl_Widget *w, void* url){
      GUI::self->browsertabs->value(GUI::self->browsertabs->BrowserGroup[GUI::self->lasttab]);//GUI::self->lasttab
      GUI::self->browsertabs->TabCallback(GUI::self->browsertabs,NULL);    

      snprintf(GUI::self->browserwin[GUI::currenttab]->tabtitle,12,"%s",lt8("Loading...  ")); //clear
      GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->label(GUI::self->browserwin[GUI::currenttab]->tabtitle);
      GUI::self->redraw();
      ::loadURL((const char*)url);
}

/*************************************************************************
* GUI and Tabs
**************************************************************************/

GUI *GUI::self = NULL;

GUI::GUI(int X, int Y ,int W, int H) : Fl_Window(X, Y, W, H)
{
        mnu = NULL;
        mnu_bar = NULL;
        for (int i=0; i<=(MAXVIEWS-1); i++){ //-1 to avoid aggressive opts warning
            browserwin[i] = NULL;
        }
        
       	for (int i = 0; i < MAXHISTORY; i++) {
		URL[i].name[0]='\0'; 
		URL[i].uri[0]='\0';
	}
        
       	for (int i = 0; i < MAXBOOKMARKS; i++) {
		bookmarks[i].name[0]='\0'; 
		bookmarks[i].uri[0]='\0';
	}

	stat_bar = NULL;
	self = this;
	//findwindow=NULL;
	historywindow=NULL;
	bookmarkswindow=NULL;
}

GUI::~GUI()
{
	delete(mnu_bar);
	delete(mnu);
	for (int i=0; i<=(MAXVIEWS-1); i++){
	   if (browserwin[i] != NULL) delete(browserwin[i]);
	}
	delete(stat_bar);
	self = NULL;
}

void GUI::create_layout()
{
	begin();
	browsertabs = new BrowserTabs(0,0,defaultWindowWidth+1,defaultWindowHeight-19);

	//define first group here to allow to set resizable(0)
	Fl_Group* o = new Fl_Group(0,browsertabs->y()+20,browsertabs->w(),browsertabs->h()-20,"New Tab    "); //lt8 will crash here!
	browsertabs->BrowserGroup[0] = o;
	browserwin[0] = new HtmlBrowser(o->x(),o->y(),o->w(),o->h());
	resizable(browserwin[0]);
	browserwin[0]->end();
	o->end();

	GUI::self->lasttab = browsertabs->ConstructTab(browsertabs,"+"); 

	browsertabs->resizable(o);
	browsertabs->callback(browsertabs->TabCallback);
	browsertabs->end();

/*************************************************************************
* GUI layout continued - statusbar part with scroll buttons
**************************************************************************/
	//stat_bar = new Fl_Output(0, defaultWindowHeight-19, defaultWindowWidth+1-150-5, 20); //171
	stat_bar = new Fl_Output(0, defaultWindowHeight-19, defaultWindowWidth, 20); //171
	stat_bar->color(FL_GRAY);  
	end();
}

/*************************************************************************
* Callbacks sections now
**************************************************************************/

/*************************************************************************
* GUI callbacks
**************************************************************************/

void scrollbarv_cb(Fl_Widget *w, void *data)
{
  Fl_Scrollbar* sb = (Fl_Scrollbar*)w;
  WKPoint position;
  position.x = -1; //-1 = keep position
  position.y = sb->value();
  WKPageSetScrollPosForFrame(view[GUI::currenttab]->uiPage(),position);
}

void scrollbarh_cb(Fl_Widget *w, void *data)
{
  Fl_Scrollbar* sb = (Fl_Scrollbar*)w;
  WKPoint position;
  position.x = sb->value();
  position.y = -1; //-1 = keep position
  WKPageSetScrollPosForFrame(view[GUI::currenttab]->uiPage(),position);
}

void download_file(Fl_Widget *w, void* url){
  char msg[1024];
  char msgtmp[1024];
  char pathbuf[1024];
  int d; //relative position of slash before filename
  char * pch; //position of slash before filename
  
	snprintf(msg,256,"Save %s as file...",(char*)url);
	char *f;
          sprintf(msgtmp,"%s",""); //clear
	  sprintf(msgtmp,"%s",(char*)url);
	  pch=strrchr(msgtmp,'/');
	  if (pch==0) {
	    d = 0; 
	  } else {
	    d = pch - msgtmp; 
	    if (d<0) d=0;
	  }
#if defined (__MINGW32__)	  
	f = fl_file_chooser(msg, "*", msgtmp+d+1);
#else
	sprintf(pathbuf,"%s%s",downloadsdir,msgtmp+d+1);
	f = fl_file_chooser(msg, "*",pathbuf);
#endif
	if (f) {
	  GUI::self->browserwin[GUI::currenttab]->savetargetas=1;
	  loadURL((const char*)url);
	} 
}

void copy_to_clipboard(Fl_Widget *w, void* text){
  Fl::copy((char*)text,strlen((char*)text),1);
}

void cb_selectall(Fl_Widget *w, void* d){
  WKPageExecuteCommand(view[GUI::currenttab]->uiPage(),WKStringCreateWithUTF8CString("SelectAll"));
}

void context_menu_cb(int x, int y){
  //make hit test to determine if click on link
  //open different context menus if yes or no
  char *theURI;
  char URI[1024];
  char URItitle[1024];
  char title_url[2048];
  
  sprintf(URI,"%s",view[GUI::currenttab]->mouse_uri.c_str());
  theURI=URI;
  sprintf(URItitle,"%s",view[GUI::currenttab]->mouse_title.c_str());
  sprintf(title_url,"%s,%s",view[GUI::currenttab]->mouse_uri.c_str(),view[GUI::currenttab]->mouse_title.c_str());
   
  Fl_Menu_Item menuitems1[] = {
      { lt8(" Open link in new tab "), 0, (Fl_Callback*)openURL_in_newtab,(void*) theURI, 0, 0, 0, 17},
      { lt8(" Add link to bookmarks "), 0, (Fl_Callback*)cb_add_bookmark, (void*)title_url, 0, 0, 0, 17},
      { lt8(" Save target as... "), 0, (Fl_Callback*)download_file,(void*)theURI, 0, 0, 0, 17},
      { lt8(" Copy link to clipboard "), 0, (Fl_Callback*)copy_to_clipboard,(void*)theURI, 0, 0, 0, 17},
      { lt8(" Copy link text "), 0, (Fl_Callback*)copy_to_clipboard,(void*)URItitle, 0, 0, 0, 17},
      { 0 }
      };    
  
  Fl_Menu_Item menuitems2[] = {
      { lt8(" Back "), 0, (Fl_Callback*)GUI::self->browserwin[GUI::currenttab]->tool->cb_back, 0, 0, 0, 0, 17},
      { lt8(" Forward "), 0, (Fl_Callback*)GUI::self->browserwin[GUI::currenttab]->tool->cb_forward, 0, 0, 0, 0, 17},
      { lt8(" Reload "), 0, (Fl_Callback*)GUI::self->browserwin[GUI::currenttab]->tool->cb_reload, 0, 0, 0, 0, 17},
      { lt8(" Stop "), 0, (Fl_Callback*)GUI::self->browserwin[GUI::currenttab]->tool->cb_stop, 0, FL_MENU_DIVIDER, 0, 0, 17},
      { lt8(" Add bookmark "), 0, (Fl_Callback*)cb_add_bookmark, 0, 0, 0, 0, 17},
      { lt8(" Save page as.. "), 0, (Fl_Callback*)cb_save_as, 0, 0, 0, 0, 17},
//      { lt8(" Print to PS "), 0, (Fl_Callback*)cb_print, 0, FL_MENU_DIVIDER, 0, 0, 17},
//      { lt8(" Select all "), 0, (Fl_Callback*)cb_selectall, 0, 0, 0, 0, 17},
//      { lt8(" Copy selected "), 0, (Fl_Callback*)cb_copy, 0, 0, 0, 0, 17},
      { lt8(" Select all and copy "), 0, (Fl_Callback*)cb_copy, 0, 0, 0, 0, 17},
      { lt8(" Show source "), 0, (Fl_Callback*)cb_showSource, 0, 0, 0, 0, 17},
      { 0 }
      };
  

  //make dummybutton to allow to set attributes for the popup menu - see last parameter
  //allow for two different menu colors if needed
  Fl_Menu_Button* dummybutton1 = new Fl_Menu_Button(1,1, 1, 1);
  dummybutton1->color(FL_WHITE);
  dummybutton1->box(FL_SHADOW_BOX);
  Fl_Menu_Button* dummybutton2 = new Fl_Menu_Button(1,1, 1, 1);
  dummybutton2->color(FL_WHITE);
  dummybutton2->box(FL_SHADOW_BOX);

  const Fl_Menu_Item *m;
  if (strlen(theURI)!=0){
    GUI::self->stat_bar->value(theURI);
    GUI::self->stat_bar->redraw();
    GUI::self->damage(FL_DAMAGE_CHILD);
    Fl::flush();
    m = menuitems1->popup(x, y, 0, &menuitems1[0], dummybutton1); 
#if 1 
    GUI::self->stat_bar->value("                    ");
    GUI::self->stat_bar->redraw();
    GUI::self->damage(FL_DAMAGE_CHILD);
    Fl::flush();
#endif      
  } else {  
    m = menuitems2->popup(x, y, 0, &menuitems2[0], dummybutton2); 
  } //if  
  
  if ( m ) m->do_callback(NULL, m->user_data());

  return;
}

/*************************************************************************
* Menu callbacks
**************************************************************************/

/* Callback invoked when menubutton pushed */
void cb_menu(Fl_Widget *w, void*) {

  static int en=0,de=1; //avoid error in gcc 4.9.2

  Fl_Menu_Item menuitems[] = {
    { lt8(" &File "), 0, 0, 0, FL_SUBMENU, 0, 0, 17 },
      { lt8(" Open "), 0, (Fl_Callback*)cb_open, 0, 0, 0, 0, 17},
      { lt8(" Save "), 0, (Fl_Callback*)cb_save_as, 0, 0, 0, 0, 17},
      { " Print to PS ", 0, (Fl_Callback*)cb_print, (void*)GUI::self->browserwin[GUI::currenttab], 0, 0, 0, 17},
      { lt8(" Add bookmark "), 0, (Fl_Callback*)cb_add_bookmark, 0, 0, 0, 0, 17},
      { lt8(" Set as startpage "), 0, (Fl_Callback*)cb_set_startpage, 0, 0, 0, 0, 17},      
      { lt8(" Language "), 0, 0, 0, FL_SUBMENU, 0, 0, 17 },
	{ " English ", 0, (Fl_Callback*)cb_set_language, (void*)&en, 0, 0, 0, 17},
	{ " Deutsch ", 0, (Fl_Callback*)cb_set_language, (void*)&de, 0, 0, 0, 17},
      { 0 },
      { lt8(" Stop file transfer "), 0, (Fl_Callback*)cb_stop_xfer, 0, 0, 0, 0, 17},
      { lt8(" E&xit "), 0, (Fl_Callback*)cb_exit, 0, 0, 0, 0, 17},
      { 0 },
    { lt8(" Edit "), 0, 0, 0, FL_SUBMENU, 0, 0, 17},
//      { lt8(" Copy selected "), 0, (Fl_Callback*)cb_copy, 0, 0, 0, 0, 17},
      { lt8(" Select all and copy "), 0, (Fl_Callback*)cb_copy, 0, 0, 0, 0, 17},
      { lt8(" Find text "), 0, (Fl_Callback*)cb_find, 0, 0, 0, 0, 17},
      { lt8(" Show source "), 0, (Fl_Callback*)cb_showSource, 0, 0, 0, 0, 17},
      { 0 },
    { lt8(" &History "), 0, (Fl_Callback*)cb_history, 0, 0, 0, 0, 17},
    { lt8(" &Bookmarks "), 0, (Fl_Callback*)cb_bookmarks, 0, 0, 0, 0, 17},    
      
    { lt8(" Help "), 0, 0, 0, FL_SUBMENU, 0, 0, 17},
      { lt8(" About "), 0, (Fl_Callback*)cb_about, 0, 0, 0, 0, 17},
      { lt8(" Online Help "), 0, (Fl_Callback*)cb_help, 0, 0, 0, 0, 17},
      { 0 },	
{ 0 }
};

  //make dummybutton to allow to set attributes for the popup menu - see last parameter
  Fl_Menu_Button* dummybutton = new Fl_Menu_Button(1,1, 1, 1);
  dummybutton->color(FL_WHITE);
  dummybutton->box(FL_SHADOW_BOX);
  //dummybutton->box(FL_BORDER_BOX);

  //const Fl_Menu_Item *m = menuitems->popup(GUI::self->stat_bar->w(), GUI::self->h()-145, 0, 0, 0); //adjust height if menue increases
  const Fl_Menu_Item *m = menuitems->popup(GUI::self->browserwin[GUI::currenttab]->bview->x()+2,GUI::self->browserwin[GUI::currenttab]->bview->y()+2, 0, 0, dummybutton); 
  if ( m ) m->do_callback(w, m->user_data());
  return;
}

/*************************************************************************
* File menu 
**************************************************************************/

void cb_open(Fl_Widget*, void *d)
{
  const char *f;
  //static char fpath[1024]; //old version
  char fpath[1024];
  static char file_url[1024];

  if (d != NULL){
    sprintf(fpath,"%s",(char*)d);
    f = fpath; //(char*)d; //"ftpdir.htm";
  } else {
    sprintf(fpath,"%s",""); //clear
    f = fl_file_chooser(lt8("Open ..."), "*.{html,htm,HTM}", downloadsdir, 0);
  }

if (f) {
  sprintf(file_url,"file://%s",f);
  printf("file_url:%s",file_url);
  loadURL(file_url);
}

} //end cb_open


void cb_save_as(Fl_Widget*, void *d)
{
  //will call getsource callback which does the rest
  WKPageGetSourceForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), getsource);
}


//this version works somewhat
void cb_print(Fl_Widget*, void *d)
{
  char mainTitle[1024];
  char mainURL[1024];
  int i = 1; //count pages
  char msg[1024];
  WKPoint sposition; //scrollbar_v
  
  //////::view[GUI::currenttab]->printactive=1;
  HtmlBrowser *display=GUI::self->browserwin[GUI::currenttab]; //to shorten code
  //HtmlBrowser *display=(HtmlBrowser*)d; 

  // save the original viewport size
  int save_x,save_y,save_w,save_h;
  int save_gx,save_gy,save_gw,save_gh;
  save_x=display->x();
  save_y=display->y(); 
  save_w=display->w(); 
  save_h=display->h();
#if 1
  save_gx=GUI::self->x(); 
  save_gy=GUI::self->y();
  save_gw=GUI::self->w(); 
  save_gh=GUI::self->h();
  //printf("save_x:%d,save_y:%d,save_w:%d,save_h:%d\nsave_gx:%d,save_gy:%d,save_gw:%d,save_gh:%d\nWKwindowWidth:%d, WKwindowHeight:%d\n",save_x,save_y,save_w,save_h,save_gx,save_gy,save_gw,save_gh,WKwindowWidth, WKwindowHeight);
#endif

#if 1
  //printf("scrollmax:%d\n",currmaxscrolly); //display->scrollbar_v->maximum());
  display->resize(0,20,577,719); //resize to page size
  g_main_context_iteration(0,0);
  display->bview->dodraw();
  g_main_context_iteration(0,0);
  //printf("scrollmax:%d\n",currmaxscrolly); //display->scrollbar_v->maximum());
  fl_alert("Sorry, printing is still an alpha version.\n\nPlease select output file now.");
#endif
  
   GUI::self->browserwin[GUI::currenttab]->bview->dodraw();
   g_main_context_iteration(0,0);

   // get the page URL and title
   snprintf(mainTitle,1024,"%s",GUI::self->browserwin[GUI::currenttab]->currenttitle);
   snprintf(mainURL,1024,"%s",GUI::self->browserwin[GUI::currenttab]->currenturl);
	    
   // get the date and time of printing
   char printDate[32];  // make it relatively large to be on the safe side
   time_t now = time(NULL);
   struct tm *lt = localtime(&now);
   strftime(printDate, sizeof(printDate), "%I:%M%p", lt); //strftime (buffer,80,"Now it's %I:%M%p.",timeinfo);
   
   // get the canvas size(total page size) and number of pages
   int wView, hView, hScroll, vScroll;   
   int wCanvas, hCanvas;

  hScroll=currmaxscrollx; // GUI::self->browserwin[GUI::currenttab]->maxscrollx;
  vScroll=currmaxscrolly; // GUI::self->browserwin[GUI::currenttab]->maxscrolly;
  //hScroll=GUI::self->browserwin[GUI::currenttab]->maxscrollx;
  //vScroll=GUI::self->browserwin[GUI::currenttab]->maxscrolly;
   //printf("Scrollsize: hScroll = %d, vScroll = %d\n", hScroll, vScroll);
   
   wView=display->w();
   hView=display->h();
   //printf("Visible: wView = %d, hView = %d\n", wView, hView);

   //total size is scroll + visible
   wCanvas = hScroll+wView;
   hCanvas = vScroll+hView;
   printf("Total: wCanvas = %d, hCanvas = %d\n", wCanvas, hCanvas);

   fl_font(FL_HELVETICA, 12);  // reset the font for page number etc to calculate margins
      int padding = 4, margin = fl_height() + padding; //fl_height() is the space between lines required for the title line
      printf("margin:%d,fl_height:%d\n",margin,fl_height());
      
   Fl_Printer *p = new Fl_Printer(); 
   
      int wPage, hPage;  // get the printable page size
      if (p->printable_rect(&wPage, &hPage)) {
         printf("Could not determine printable area\n");
         p->end_job();
         delete p;
         return;
      }
      
      // This is to avoid problems if p->printable_rect returns a
      // negative or zero size (for example, if you're printing to
      // a file and you don't have an actual printer installed).
      wPage = (wPage > 0) ? wPage : 577;
      hPage = (hPage > 0) ? hPage : 719;
      hPage -= 2 * margin;  // leave margins at the top and bottom
      //printf("Printable rect minus margins: wPage = %d, hPage = %d\n", wPage, hPage);
   
   if (!p->start_job(1)) {
      // resize the viewport to the printable page size
      float scalefactor=float(wPage)/float(wCanvas);
      scalefactor=1; //test
      
      int left,top,right,bottom,ow,oh;
      p->margins(&left,&top,&right,&bottom); //read margins
      p->origin(&ow,&oh); //read origin values
      //printf("left%d,top%d,right%d,bottom%d,ow:%d,oh:%d,wPage:%d,hPage:%d\n",left,top,right,bottom,ow,oh,wPage,hPage);
      
#if 0
      p->scale(scalefactor,scalefactor);
      p->printable_rect(&wPage, &hPage);
      //p->origin(0,676); // + (hPage*scalefactor)/2);
      //p->origin(0,640);
      p->origin(0,0);
      
      //scaling increases the printable rect since more pixels fit on the page
      //printf("Printable rect after scale: wPage = %d, hPage = %d, Scalefactor:%f\n", wPage, hPage,scalefactor);
      //::view[GUI::currenttab]->setPageZoomFactor(scalefactor); 
      p->margins(&left,&top,&right,&bottom);
      p->origin(&ow,&oh);
      //printf("left%d,top%d,right%d,bottom%d,ow:%d,oh:%d\n",left,top,right,bottom,ow,oh);
#endif

      // get the total number of pages
      int numPages = (int)ceil((double)hCanvas / (double)hPage);
      
      int offset = 0;      
      //display->bview->make_current();

      while (offset < hCanvas) {
      //printf("Printing page %d\n", i);
      /* //will write into postscript file!
      sprintf(msg,"Writing page %d                   ",i);      
      GUI::self->stat_bar->value(msg);
      GUI::self->stat_bar->redraw();
      GUI::self->damage(FL_DAMAGE_CHILD);
      Fl::flush();
      */
         char pageNum[16];
         //snprintf(pageNum, sizeof(pageNum), "Page %d of %d", i, numPages);
	 snprintf(pageNum, sizeof(pageNum), "Page %d", i);

         p->start_page();

	 GUI::self->browserwin[GUI::currenttab]->bview->dodraw();
	 g_main_context_iteration(0,0);
	 
         fl_color(FL_BLACK);
         fl_font(FL_HELVETICA, 12);  // reset the font to calculate margins

         // Title in the top left
         fl_draw(mainTitle, 0, 0);
         // URL in the bottom left
         fl_draw(mainURL, 0, hPage + margin);
         // Page number in the top right
         fl_draw(pageNum, wPage - (int)fl_width(pageNum), 0);
         // Date in the bottom right
         fl_draw(printDate, wPage - (int)fl_width(printDate), hPage + margin); 
	 
	 p->print_widget(GUI::self->browserwin[GUI::currenttab]->bview,0,0);

         p->end_page();

         offset += 719 - 2*margin -8; //719; //WKwindowHeight-74; //::view[GUI::currenttab]->currh; //hPage;
  
	 sposition.x = -1; //-1 = keep position
	 sposition.y = offset; //advance by one page
	 WKPageSetScrollPosForFrame(view[GUI::currenttab]->uiPage(),sposition);
	 //g_main_context_iteration(0,0);

         //printf("offset:%d,hCanvas:%d,Page:%d\n",offset,hCanvas,i);
         i++;
	 //if (i==5) break;
	 //break;
      } //while
      p->end_job();
   }
#if 1
   // restore the original viewport size
   GUI::self->resize(save_gx, save_gy,save_gw,save_gh);
   display->resize(save_x, save_y, save_w,save_h);
   GUI::self->damage();
   Fl::flush();

   //GUI::self->resize(0,0,save_w,save_h);
   // FIXME: remember the user's scroll position
   //->scrollTo(0, 0);

      sprintf(msg,"%d pages written                   ",i);      
      GUI::self->stat_bar->value(msg);
      GUI::self->stat_bar->redraw();
      GUI::self->damage(FL_DAMAGE_CHILD);
      Fl::flush();
#endif
   delete p;
   //////::view[GUI::currenttab]->printactive=0;
}


//save current page as startpage
void cb_set_startpage(Fl_Widget*, void*)
{
    sprintf(GUI::self->startpage,"%s",get_url());
}

void cb_set_language(Fl_Widget*, void* v)
{
  //printf("value:%d\n",*((int*)v));
  
  switch (*((int*)v)) { //for gcc 4.9.2
    case 0:
      sprintf(Fl_I18n::current_language,"%s",""); //english
      break;      
    case 1:
      sprintf(Fl_I18n::current_language,"%s","german");
      break;            
    default:
      sprintf(Fl_I18n::current_language,"%s",""); //english
      break;
    }
    locale->reload("", "netrider","");
}

void cb_stop_xfer(Fl_Widget*, void*)
{
  WKDownloadCancel(GUI::self->browserwin[GUI::currenttab]->download_ref);
}

void cb_exit(Fl_Widget*, void*)
{
	writePrefs();
	prefs_written=1; //don't run prefs again by atexit()
	
	exit(0);
}

/*************************************************************************
* Edit menu - find and show source
**************************************************************************/

void cb_copy(Fl_Widget*, void *d)
{
  //Rest handled in getSelectionOrContents callback
  WKPageGetSelectionOrContentsAsString(view[GUI::currenttab]->uiPage(), (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), getSelectionOrContents);
}


void cb_findtext(Fl_Widget* w, void* d){
  if (!d) return;
  Fl_Input *in = (Fl_Input*)d;
    strncpy(GUI::self->findtext,in->value(),512);
    if (GUI::self->findtext[0]=='\0') return; //empty input field entered
    int foptions = 0b11010001;
    WKPageFindString(view[GUI::currenttab]->uiPage(),WKStringCreateWithUTF8CString(GUI::self->findtext),foptions,0);
}

void cb_findnext(Fl_Widget* w, void*){ 
    int foptions = 0b11010001;
    WKPageFindString(view[GUI::currenttab]->uiPage(),WKStringCreateWithUTF8CString(GUI::self->findtext),foptions,0);
}

void cb_findprev(Fl_Widget* w, void*){
    int foptions = 0b11010001;
    foptions = foptions | 1<<3;
    WKPageFindString(view[GUI::currenttab]->uiPage(),WKStringCreateWithUTF8CString(GUI::self->findtext),foptions,0);
}

void cb_findclose(Fl_Widget* w, void *d){
  Fl_Window *fw = (Fl_Window*)d;
  fw->hide(); 
}


void cb_find(Fl_Widget* w, void* d)
{
    findwindow = new Fl_Window(GUI::self->x(),GUI::self->y()+GUI::self->h()-40, 280, 20);
    Fl_Input *findtext = new Fl_Input(0, 0,  160, 20);
    Fl_Button *findnext = new Fl_Button(160, 0, 40, 20);
    Fl_Button *findprev = new Fl_Button(200, 0, 40, 20);
    Fl_Button *findquit = new Fl_Button(240, 0, 40, 20);
    findwindow->end();

  findnext->label(lt8("next"));
  findprev->label(lt8("back"));
  findquit->label(lt8("close"));

  findtext->callback(cb_findtext,(void*)findtext); 
  findtext->when(FL_WHEN_ENTER_KEY || FL_WHEN_RELEASE);
  findnext->callback(cb_findnext);
  //findnext->tooltip("Find next occurence");
  findprev->callback(cb_findprev);
  //findprev->tooltip("Find previous occurence");
  findquit->callback(cb_findclose,findwindow);
    
  findwindow->clear_border();	
  findwindow->show();
}

void cb_showSource(Fl_Widget*, void*)
{
 WKPageGetSourceForFrame(view[GUI::currenttab]->uiPage(), WKPageGetMainFrame(view[GUI::currenttab]->uiPage()), (void*) WKPageGetContext(view[GUI::currenttab]->uiPage()), getsourceandshow);
}

/*************************************************************************
* History
**************************************************************************/

void HistoryCallback(Fl_Widget *w, void *data) {
  Fl_Browser *fbrow = (Fl_Browser*)w;
  int index = fbrow->value();            // get index of selected item
  if (index==0) return;                  // valid item?
  char pagename[1024];
  char uri[1024];
  char * tabpos;  //now double-line graphics char: 204decimal or 314 octal character instead!
  char buf[1024];
  const char* url = buf;

  tabpos=strchr((char*)fbrow->text(index),'\314');
  strcpy(uri,tabpos+1); //first text after tab -> uri, then before->pagename
  strncpy(pagename,fbrow->text(index),tabpos - fbrow->text(index));
  pagename[tabpos - fbrow->text(index)] = '\0';
  fbrow->parent()->hide();
  
  sprintf(buf,"%s",uri);
  checkurl((char*) buf);  
  GUI::self->browserwin[GUI::currenttab]->tool->loc->value(buf); //put in URL location field
  
  WKPageStopLoading(view[GUI::currenttab]->uiPage());
  snprintf(GUI::self->browserwin[GUI::currenttab]->tabtitle,12,"%s",lt8("Loading...  ")); //clear before loading new url
  GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->label(GUI::self->browserwin[GUI::currenttab]->tabtitle);
  GUI::self->redraw();
  ::loadURL(url); //=buf see above 
}

void cb_history(Fl_Widget*, void*)
{
  char buffer[2048];

  GUI::self->historywindow = new Fl_Window(GUI::self->x()+20,GUI::self->y()+20,680,GUI::self->h()-20);
  GUI::self->historywindow->label(lt8("History"));
  
  Fl_Browser *b = new Fl_Browser(10,10,GUI::self->historywindow->w()-20,GUI::self->historywindow->h()-20);
  static int widths[] = { 110*6, 30*6, 0 };  // widths for each column 8 pixel per char
  b->column_widths(widths);
  b->column_char('\314');   // tab as column delimiters  //now double-line graphics char: 204decimal or 314 octal character instead!
  b->type(FL_HOLD_BROWSER);  //single selection
  GUI::self->historywindow->resizable(b);
  b->callback(HistoryCallback);
  
  
  for (int i = 0; i < MAXHISTORY; i++) {
    sprintf(buffer,"%s\314%s",GUI::self->URL[i].name,GUI::self->URL[i].uri);
    if (strlen(buffer)>1){
      b->add(buffer);
      //count++;
    } else {
      break; 
    }
  }

  int maxheigth=GUI::self->h()-20;
  if (maxheigth > (38+b->size()*14)){
      GUI::self->historywindow->resize(GUI::self->x()+20,GUI::self->y()+20,680,38+(b->size())*14+30); //no space for buttons  
    } else {  
      b->resize(10,10, GUI::self->historywindow->w()-20, GUI::self->historywindow->h()-20); //no space for buttons
  }
  
  GUI::self->historywindow->resizable(b);
  GUI::self->historywindow->end();
  GUI::self->historywindow->show();
}

/*************************************************************************
* Bookmarks
**************************************************************************/

void cb_add_bookmark(Fl_Widget*, void* title_url)
{  
  char *theURI;
  char *pch;
  int d;
	//write into bookmark list
	//push entries down and write new entry into 0 item
	for (int i = MAXBOOKMARKS-1; i > 0; i--) {
	  sprintf(GUI::self->bookmarks[i].name,"%s",GUI::self->bookmarks[i - 1].name);
	  sprintf(GUI::self->bookmarks[i].uri,"%s",GUI::self->bookmarks[i - 1].uri);
	}
	if (title_url == NULL){
	  std::string titleStr = createStdStringFromWKString(WKPageCopyTitle(view[GUI::currenttab]->uiPage()));
	  sprintf(GUI::self->bookmarks[0].name,"%s",titleStr.c_str());
	  sprintf(GUI::self->bookmarks[0].uri,"%s",get_url());
	} else {
	  //find comma in title_url
	  pch=strchr((char*)title_url,',');
	  if (pch==0) {
	    return; 
	  } else {
	    d = pch - (char*)title_url; 
	    if (d<0) d=0;
	  }
	  snprintf(GUI::self->bookmarks[0].uri,d+1,"%s",(char*)title_url);
	  sprintf(GUI::self->bookmarks[0].name,"%s",(char*)title_url+d+1);		  
	}
}

void cb_edittext_book(Fl_Widget* w, void* d)
{
  Fl_Window *b = (Fl_Window*)w;
  
  char buffer[2048];
  
  struct inputobjects{
    Fl_Input* name;
    Fl_Input* url;
    Fl_Browser* fbrow;
  };
  inputobjects *inputs = (inputobjects*)d;

  int index=inputs->fbrow->value(); //get selected line

  //modify bookmarks array entry - this is zero based while browser is one based
  sprintf(GUI::self->bookmarks[index-1].name,"%s",inputs->name->value());
  sprintf(GUI::self->bookmarks[index-1].uri,"%s",inputs->url->value());
  sprintf(buffer,"%s\314%s",GUI::self->bookmarks[index-1].name,GUI::self->bookmarks[index-1].uri);
  inputs->fbrow->text(index,buffer);
  
  b->parent()->hide();   
}

void cb_editquit_book(Fl_Widget* w, void*)
{
  Fl_Window *b = (Fl_Window*)w;
  b->parent()->hide();  
}

void cb_edit_bookmark(Fl_Widget*, void* d)
{
  char pagename[1024];
  char pageuri[1024];
  char * tabpos;  //now double-line graphics char: 204decimal or 314 octal character instead!

  struct inputobjects{
    Fl_Input* name;
    Fl_Input* url;
    Fl_Browser* fbrow;
  };
  inputobjects *inputs = new inputobjects;
  
  Fl_Browser *fbrow = (Fl_Browser*)d;
  int index=fbrow->value(); //browser index is one based

  if ((char*)fbrow->text(index)) { //empty entries are NULL
      tabpos=strchr((char*)fbrow->text(index),'\314');
      strcpy(pageuri,tabpos+1); //first text after tab -> uri, then before->pagename
      strncpy(pagename,fbrow->text(index),tabpos - fbrow->text(index));
      pagename[tabpos - fbrow->text(index)] = '\0';
  
      Fl_Window *editwindow = new Fl_Window(fbrow->x()+20,fbrow->y()+20, 580, 160);
	Fl_Input *editname = new Fl_Input(90, 10,  460, 30,"Name: ");
	editname->value(pagename);
	Fl_Input *editurl = new Fl_Input(90, 60,  460, 30, "URL:  ");
	editurl->value(pageuri);
	Fl_Button *editok = new Fl_Button(90, 110, 60, 30, lt8("Save"));
	Fl_Button *editquit = new Fl_Button(180, 110, 60, 30, lt8("Close"));
      editwindow->end();

      inputs->name=editname;
      inputs->url=editurl;
      inputs->fbrow=fbrow;  
      editok->callback(cb_edittext_book,(void*)inputs); 
      editquit->callback(cb_editquit_book);

      editwindow->show();
  } //empty

}

void cb_sort_bookmark(Fl_Widget*, void* d)
{ 
  Fl_Browser *b = (Fl_Browser*)d;
  
    for ( int t=1; t<=b->size(); t++ ) {
        for ( int r=t+1; r<=b->size(); r++ ) {
            if ( strcmp(b->text(t), b->text(r)) > 0 ) {
                b->swap(t,r);
            }
        } //for r
    } //for t

  //now write sorted items into bookmarks array
  char pagename[1024];
  char pageuri[1024];
  char * tabpos;  //now double-line graphics char: 204decimal or 314 octal character instead!
  
  for (int i = 1; i < MAXBOOKMARKS; i++) {
      //read out uri and name from browser entry
    if ((char*)b->text(i)) { //empty entries are NULL
      tabpos=strchr((char*)b->text(i),'\314');
      if (tabpos) {
	strcpy(pageuri,tabpos+1); //first text after tab -> uri, then before->pagename
	strncpy(pagename,b->text(i),tabpos - b->text(i));
	pagename[tabpos - b->text(i)] = '\0';

	sprintf(GUI::self->bookmarks[i-1].name,"%s",pagename);
	sprintf(GUI::self->bookmarks[i-1].uri,"%s",pageuri);
      }
    } //empty entries
  }

}

void cb_delete_bookmark(Fl_Widget*, void* d)
{  
  Fl_Browser *fbrow = (Fl_Browser*)d;
  int index=fbrow->value()-1; //browser index is one based
  //delete line from browser
  fbrow->remove(index+1); //since -1 above

  //delete line from bookmarks array now
  //push entries up starting with deleted entry and clear last entry
  for (int i = index; i < MAXBOOKMARKS-1; i++) {
    sprintf(GUI::self->bookmarks[i].name,"%s",GUI::self->bookmarks[i + 1].name);
    sprintf(GUI::self->bookmarks[i].uri,"%s",GUI::self->bookmarks[i + 1].uri);
  }
  sprintf(GUI::self->bookmarks[MAXBOOKMARKS].name,"%s","");
  sprintf(GUI::self->bookmarks[MAXBOOKMARKS].uri,"%s","");

}

void cb_close_bookmark(Fl_Widget*, void* d)
{ 
  Fl_Window *b = (Fl_Window*)d;
  b->hide();
}

void bookmarks_cb(Fl_Widget* w, void* d)
{
  Fl_Browser *fbrow;
  if (d==NULL){ //bookmarkwindow as shortwindow
    fbrow = (Fl_Browser*)w;
  } else { //bookmarkwindow with buttons
    fbrow = (Fl_Browser*)d;
  }
  int index = fbrow->value();            // get index of selected item
  if (index==0) return;                  // valid item?
  char pagename[1024];
  char uri[1024];
  char * tabpos; //now double-line graphics char: 204decimal or 314 octal character instead!
  char buf[1024];
  const char* url = buf;

  tabpos=strchr((char*)fbrow->text(index),'\314');
  strcpy(uri,tabpos+1); //first text after tab -> uri, then before->pagename
  strncpy(pagename,fbrow->text(index),tabpos - fbrow->text(index));
  pagename[tabpos - fbrow->text(index)] = '\0';
  fbrow->parent()->hide();
  
  sprintf(buf,"%s",uri);
  checkurl((char*) buf);
  GUI::self->browserwin[GUI::currenttab]->tool->loc->value(buf); //put in URL location field
  
  //////::view[GUI::currenttab]->stopLoading();
  WKPageStopLoading(view[GUI::currenttab]->uiPage());
  
  snprintf(GUI::self->browserwin[GUI::currenttab]->tabtitle,12,"%s",lt8("Loading...  ")); //clear
  GUI::self->browsertabs->BrowserGroup[GUI::currenttab]->label(GUI::self->browserwin[GUI::currenttab]->tabtitle);
  GUI::self->redraw();
  ::loadURL(url); //=buf see above
}

void cb_bookmarks(Fl_Widget*, void* d)
{
  int shortwin=(int)((size_t)d); //passing address location one
  int bstart=10;
  int maxheigth=0;
  
  char buffer[2048];

  GUI::self->bookmarkswindow = new Fl_Window(GUI::self->x()+20,GUI::self->y()+20,680,GUI::self->h()-20);
  GUI::self->bookmarkswindow->label(lt8("Bookmarks"));
  
  Fl_Browser *b = new Fl_Browser(10,10,GUI::self->bookmarkswindow->w()-20,GUI::self->bookmarkswindow->h()-60);
  static int widths[] = { 110*6, 0 };  // widths for each column 8 pixel per char
  b->column_widths(widths);
  b->column_char('\314');   // tab as column delimiters  //now double-line graphics char: 204decimal or 314 octal character instead!
  b->type(FL_HOLD_BROWSER);  //single selection
  GUI::self->bookmarkswindow->resizable(b);
  if (shortwin==1){
    b->callback(bookmarks_cb);  
    b->resize(10,10,GUI::self->bookmarkswindow->w()-20,GUI::self->bookmarkswindow->h()-20);
  }
  b->end();
  
  for (int i = 0; i < MAXBOOKMARKS; i++) {
    sprintf(buffer,"%s\314%s",GUI::self->bookmarks[i].name,GUI::self->bookmarks[i].uri);
    if (strlen(buffer)>1){
      b->add(buffer); //printf("%s\n",buffer);
    } else {
      break;
    }
  }
  
  maxheigth=GUI::self->h()-20;
  if (shortwin==0) {
    if (maxheigth > (80+18+b->size()*14)){
      GUI::self->bookmarkswindow->resize(GUI::self->x()+20,GUI::self->y()+20,680,80+18+(b->size())*14); //with space for buttons  
      b->resize(10,10,GUI::self->bookmarkswindow->w()-20,GUI::self->bookmarkswindow->h()-60);
    }        

    bstart=GUI::self->bookmarkswindow->w()-500;
    Fl_Button *loadbook = new Fl_Button(bstart,GUI::self->bookmarkswindow->h()-40, 80, 30);
    loadbook->label(lt8("Load"));
    loadbook->callback(bookmarks_cb,(void*)b);
    Fl_Button *editbook = new Fl_Button(bstart+100,GUI::self->bookmarkswindow->h()-40, 80, 30);
    editbook->label(lt8("Edit"));
    editbook->callback(cb_edit_bookmark,(void*)b);
    Fl_Button *sortbook = new Fl_Button(bstart+200,GUI::self->bookmarkswindow->h()-40, 80, 30);
    sortbook->label(lt8("Sort"));
    sortbook->callback(cb_sort_bookmark,(void*)b);
    Fl_Button *deletebook = new Fl_Button(bstart+300,GUI::self->bookmarkswindow->h()-40, 80, 30);
    deletebook->label(lt8("Delete"));
    deletebook->callback(cb_delete_bookmark,(void*)b);
    Fl_Button *closebook = new Fl_Button(bstart+400,GUI::self->bookmarkswindow->h()-40, 80, 30);
    closebook->label(lt8("Close"));
    closebook->callback(cb_close_bookmark,(void*)GUI::self->bookmarkswindow);

  } else { //if called from bookmarks icon at top right corner
    if (maxheigth > (38+b->size()*14)){
      GUI::self->bookmarkswindow->resize(GUI::self->x()+20,GUI::self->y()+20,680,38+(b->size())*14); //no space for buttons  
    } else {  
      b->resize(10,10,GUI::self->bookmarkswindow->w()-20,GUI::self->bookmarkswindow->h()-20); //no space for buttons
    }
  }
  GUI::self->bookmarkswindow->resizable(b);
  GUI::self->bookmarkswindow->end();
  GUI::self->bookmarkswindow->show();
}

/*************************************************************************
* Help menu
**************************************************************************/

void cb_about(Fl_Widget*, void*)
{
    fl_message("  NetRider - Copyright (c) 2015 Georg Potthast\n\n"
	       "  This software has a BSD license\n"
    );
}

void cb_help(Fl_Widget*, void*)
{
	fl_message("NetRider Help Page\n\n"
	    "Scroll and Zoom Display:\n"
	    "Mouse wheel up/down/left/right scrolls the display\n"
	    "Ctrl-Mouse wheel up/down zooms/shrinks the display\n"
	    "Ctrl-Home - scroll the display to TOP, Ctrl-End - scroll the display to END\n"
	    "PageUP/PageDOWN - scroll the display one page up or down\n\n"
	    "Navigation:\n"
	    "Select the text in the URL input field using a double click!\n"
	    "Mouse Button 4 - go back one web page\n"
	    "Mouse Button 5 - go forward one web page\n"
	    "Ctrl-S - stop loading, Ctrl-R - reload web page\n\n"
#if 0	    
	    "Editing: - Paste not implemented yet!\n"
	    "Ctrl-A - select all, Ctrl-X cut, Ctrl-C copy, Ctrl-P select paragraph\n"
	    "Ctrl-D - select sentence, Ctrl-L select line, Ctrl-W select word\n"
	    "Cursor keys + Shift:select + Alt:move word + Alt-Shift:select word\n"
	    "Backspace + Shift:delete forward + Alt:delete word + Alt-Shift:delete word forward\n"
#endif
	    "\nCtrl-T - open new tab, Ctrl-W - close current tab - set focus to this tab before entering\n"
	    "Close tabs with right mouse click - the current tab cannot be closed with FLTK 1.3.2\n"
	    "\nThe first tab cannot be closed\n"	    
	    //"\nSettings will be saved when the program is terminated selecting Exit from the menu\n"
	    //"\n\nTodo:\nFile download, plugins, cookies, print etc.\n"
	);
}

int make_ftpdir_page(char* ftpurl){

  if (ftpurl==NULL) return 0;
  
  static int activeflag;
  if (activeflag==0){
    activeflag=1;
  } else {
    return 0;
  }
  
int strcopylen;
char * strpos;
char * strposb;
FILE *inputfile,*outputfile;
char dirbuf[512];
char linebuf[512];
char tmpbuf[512];
char namebuf[512];
char sizebuf[512];
char datebuf[512];
char yearbuf[512];
char timebuf[512];


#if defined (__MINGW32__)	  
	inputfile = fopen("ftpdir","rb");
#else
	inputfile = fopen(downloadsdir_ftp,"rb");
#endif

if (inputfile == NULL) {
	printf("Could not open file ftpdir");
	printf("\n%s or %s\n","ftpdir",downloadsdir_ftp);
	return 1;
}

#if defined (__MINGW32__)	  
	outputfile = fopen("ftpdir.htm","wt");
#else
	sprintf(tmpbuf,"%sftpdir.htm",downloadsdir);
	outputfile = fopen(tmpbuf,"wt");
#endif

//get current year
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

/* header lines */
fprintf(outputfile,"<HTML><HEAD><TITLE>");
//get directory for title - first line of file shall contain that
sprintf(dirbuf,"%s",ftpurl); //"ftp://ftp.delorie.com/pub/djgpp/");

fprintf(outputfile,"%s",dirbuf);
fprintf(outputfile,"</TITLE></HEAD><BODY BGCOLOR=white><P><H2>Index of ");
fprintf(outputfile,"%s",dirbuf);
fprintf(outputfile,"</H2><P><P>");

/*change to higher directory*/
/*remove last directory from dirbuf put result in tmpbuf - use linebuf as tmp storage */
	strncpy(linebuf,dirbuf,510); //do not modify dirbuf
	linebuf[strlen(linebuf)-1]='\0'; //remove trailing "/" 
	//printf("linebuf3: >%s<\n",linebuf);
	strpos=strrchr(linebuf,'/')+1; //search for new last '/'
	if ((strpos-linebuf) > 13) { //don't allow dir names below two characters
	memmove (tmpbuf,linebuf,strpos-linebuf); //copy path up to last '/'
	tmpbuf[strpos-linebuf]='\0';
	} else {
    strcpy(tmpbuf,dirbuf); //do not change directory, apparently top dir reached
	}
fprintf(outputfile,"<li>&nbsp;&nbsp;<A HREF=\"%s\">Change to higher directory</A></li><P>",tmpbuf);
fprintf(outputfile,"<table>"); //start lines with files and directories

	/* read to end-of-line */
while (fgets(linebuf,500,inputfile)){
	if (strlen(linebuf)<35) continue; //no valid line
	linebuf[strlen(linebuf)-1]='\0'; //remove trailing "/n" 

	sprintf(timebuf,"00:00");  //default
	
	if (linebuf[0] == 'd' || linebuf[0] == 'l'){
		fprintf(outputfile,"<tr><td>DIR:</td><td><A HREF=\"%s",dirbuf);
	} else {
		fprintf(outputfile,"<tr><td>File:</td><td><A HREF=\"%s",dirbuf);
	}
	
	strpos=strchr(linebuf,' '); //past file access info
	while(*++strpos ==' '){} //skip varying number of blanks
	strpos=strchr(strpos,' '); //past file type info
	while(*++strpos ==' '){} //skip varying number of blanks
	strpos=strchr(strpos,' '); //past numbers 1
	while(*++strpos ==' '){} //skip varying number of blanks
	strpos=strchr(strpos,' '); //past numbers 2
	while(*++strpos ==' '){} //skip varying number of blanks
	strposb=strchr(strpos,' '); //past file size

	snprintf(sizebuf,(strposb-strpos+1),"%s",strpos);	
	strpos=strposb;

	while(*++strpos ==' '){} //skip varying number of blanks
	strposb=strchr(strpos,' '); //past month
	while(*++strposb ==' '){} //skip varying number of blanks
	strposb=strchr(strposb,' '); //past day
	snprintf(datebuf,(strposb-strpos+1),"%s",strpos);		
	strpos=strposb;

	while(*++strpos ==' '){} //skip varying number of blanks
	strposb=strchr(strpos,' '); //past year
	snprintf(yearbuf,(strposb-strpos+1),"%s",strpos);		
	strpos=strposb;
	if (strchr(yearbuf,':')){ //does year contain time?
	  sprintf(timebuf,"%s",yearbuf);
	  sprintf(yearbuf,"%d",1900+timeinfo->tm_year);
	}
	
	while(*++strpos ==' '){} //skip varying number of blanks
	//strpos on name now
	sprintf(namebuf,"%s",strpos);
	if (linebuf[0] == 'l'){
	  strposb=strchr(strpos,'>'); //skip after and including ->
	  snprintf(namebuf,(strposb-2-strpos+1),"%s",strpos);	
	}
	
	if (linebuf[0] == 'd' || linebuf[0] == 'l'){ //handle directories
		/* add / to filename if directory */
		fprintf(outputfile,"%s/\">%s</A></td>",namebuf,namebuf); //write filename
	} else {
		fprintf(outputfile,"%s\">%s</A></td>",namebuf,namebuf); //write filename
	} //end directory

	fprintf(outputfile,"<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Bytes:</td><td>%s</td>",sizebuf);
	fprintf(outputfile,"<td>&nbsp;&nbsp;&nbsp;%s</td><td>&nbsp;&nbsp;&nbsp;%s</td><td>&nbsp;&nbsp;&nbsp;%s</td></tr>\n",datebuf,yearbuf,timebuf);
}
fprintf(outputfile,"</table></BODY></HTML>");

fclose(inputfile);
fclose(outputfile);
activeflag=0;
return 1;
}


void setWindowIcon(Fl_Window *window, Fl_Pixmap *icon)
{
#if !defined (__MINGW32__) //uses X11 Pixmap below
    Fl_Window *dummyWindow = new Fl_Window(50, 50);
    dummyWindow->show();        /* needed to define a graphics context */
    dummyWindow->make_current();
    int maxDim = icon->w();
    if(icon->h() > maxDim)
        maxDim = icon->h();
    Pixmap pixmap = fl_create_offscreen(maxDim, maxDim);
    fl_begin_offscreen(pixmap);
    fl_color(FL_BACKGROUND_COLOR);
    fl_rectf(0, 0, maxDim, maxDim);
    icon->draw((maxDim - icon->w())/2, (maxDim - icon->h())/2);
    fl_end_offscreen();
    dummyWindow->hide();
    delete dummyWindow;
    window->icon((char *)pixmap);
#endif    
}

void main_exit_handler(Fl_Widget *, void *) {
  //can add additional work here
  while (Fl::first_window()){
    Fl::first_window()->hide();
  }

  writePrefs();
  return;
}
/*************************************************************************
* Main()
**************************************************************************/

int main(int argc, char **argv) //, char **environ)
{

	//Fl::scheme("gleam");
	//Fl::scheme("OXY");
	Fl::scheme("gtk+");

#if defined (__MINGW32__) 	
//will result in: c:/Documents and Settings/<user name here>/Application Data//netrider/
//Windows interprets the "//" as one "/" so can leave it like that
Fl_Preferences nprefs( Fl_Preferences::USER, "", "netrider" );
#else
//will result in: /home/<user name here>/Application Data//netrider/
Fl_Preferences nprefs( getenv("HOME"), "", "netrider" );
#endif
//char netriderdirpath[FL_PATH_MAX];
nprefs.getUserdataPath( netriderdirpath, FL_PATH_MAX );
//printf("netriderdirpath:%s\n",netriderdirpath);
	
	
#if !defined (__MINGW32__) 	
	setlocale(LC_ALL, ""); //to allow apostrophe in printf
	homedir=getenv("HOME");

//netriderdirpath has / at end!
	sprintf(downloadsdir,"%sdownloads/",netriderdirpath);
	sprintf(downloadsdir_ftp,"%sdownloads/ftpdir",netriderdirpath); //including file name

	//check if netrider directories exist in netriderdirpath
	//mkdir will just return error if it already exists
	//with read/write/search permissions for owner and group, and with read/search permissions for others.
	char mkdirbuffer[FL_PATH_MAX];
	sprintf(mkdirbuffer,"%sgerman.lproj",netriderdirpath);
	mkdir(mkdirbuffer,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	sprintf(mkdirbuffer,"%ssettings",netriderdirpath);
	mkdir(mkdirbuffer,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	sprintf(mkdirbuffer,"%sdoc",netriderdirpath);
	mkdir(mkdirbuffer,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	sprintf(mkdirbuffer,"%sdownloads",netriderdirpath);
	mkdir(mkdirbuffer,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
  struct stat   buffer;
  //copy german.lproj/GUI.strings from opt to netriderdirpath
  sprintf(mkdirbuffer,"%sgerman.lproj/GUI.strings",netriderdirpath);
  if (stat (mkdirbuffer, &buffer) != 0){
    if (stat ("/opt/netrider/german.lproj/GUI.strings", &buffer) == 0){ // present ?
         std::ifstream  src("/opt/netrider/german.lproj/GUI.strings", std::ios::binary);
         std::ofstream  dst(mkdirbuffer,  std::ios::binary);
         dst << src.rdbuf();    
         src.close();
         dst.close();  
    }
  }
  //copy default netrider.pref from opt to netriderdirpath
  sprintf(mkdirbuffer,"%ssettings/netrider.prefs",netriderdirpath);
  if (stat (mkdirbuffer, &buffer) != 0){
    if (stat ("/opt/netrider/settings/netrider.prefs", &buffer) == 0){ // present?
         std::ifstream  src("/opt/netrider/settings/netrider.prefs", std::ios::binary);
         std::ofstream  dst(mkdirbuffer,  std::ios::binary);
         dst << src.rdbuf();    
         src.close();
         dst.close();  
    }
  }

#else //defined (__MINGW32__)
//netriderdirpath has / at end!
	sprintf(downloadsdir,"%sdownloads/",netriderdirpath);
	sprintf(downloadsdir_ftp,"%sdownloads/ftpdir",netriderdirpath); //including file name

	//check if netrider directories exist in netriderdirpath
	//mkdir will just return error if it already exists
	//with read/write/search permissions for owner and group, and with read/search permissions for others.
	char mkdirbuffer[FL_PATH_MAX];
	sprintf(mkdirbuffer,"%sgerman.lproj",netriderdirpath);
	mkdir(mkdirbuffer);
	sprintf(mkdirbuffer,"%ssettings",netriderdirpath);
	mkdir(mkdirbuffer);
	sprintf(mkdirbuffer,"%sdoc",netriderdirpath);
	mkdir(mkdirbuffer);
	sprintf(mkdirbuffer,"%sdownloads",netriderdirpath);
	mkdir(mkdirbuffer);

  struct stat   buffer;
  //copy german.lproj/GUI.strings from current directory to netriderdirpath
  sprintf(mkdirbuffer,"%sgerman.lproj/GUI.strings",netriderdirpath);
  if (stat (mkdirbuffer, &buffer) != 0){ //not present	     
    if (stat ("./netrider/german.lproj/GUI.strings", &buffer) == 0){ // present?	     
         std::ifstream  src("./netrider/german.lproj/GUI.strings", std::ios::binary);
         std::ofstream  dst(mkdirbuffer,  std::ios::binary);
         dst << src.rdbuf();  
         src.close();
         dst.close();  
    }  
  }
  //copy default netrider.pref from current directory to netriderdirpath
  sprintf(mkdirbuffer,"%ssettings/netrider.prefs",netriderdirpath);
  if (stat (mkdirbuffer, &buffer) != 0){ //not present
    if (stat ("./netrider/settings/netrider.prefs", &buffer) == 0){ // present?
         std::ifstream  src("./netrider/settings/netrider.prefs", std::ios::binary);
         std::ofstream  dst(mkdirbuffer,  std::ios::binary);
         dst << src.rdbuf();    
         src.close();
         dst.close();  
    }
  }

#endif  


	int index = 0;
	GUI *gui;

	Fl::args(argc, argv, index);
	
	//atexit(writePrefs);
	//atexit(main_exit_handler);

	
	// create the user interface
	//gui = new GUI(50,50,440, 190);
	gui = new GUI(50,50, defaultWindowWidth, defaultWindowHeight); //just won't work??

	Fl::default_atclose(gui,NULL);
        gui->callback(main_exit_handler);

#if defined (__MINGW32__)	
  gui->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));  
#else 
  static Fl_Pixmap  netrider_pixmap(netrider_xpm);
  setWindowIcon(gui,&netrider_pixmap);
#endif  

    	gui->create_layout();

	// show the interface
	gui->resize(gui->x(), gui->y(), windowWidth, windowHeight);
	gui->label("NetRider 0.94            ");
	gui->show(argc, argv);

/**************************************************************************/
  char buf[1024]={defaulturl}; //www.yahoo.com
  const char* url;

  Fl::add_timeout(0.25,Timer_CB,(void*)0);

  readPrefs();

  locale = new Fl_I18n("", "netrider",""); // allocate a global locale object
  fl_yes=lt8("Yes");
  fl_no=lt8("No");
  fl_close=lt8("Close");
  fl_cancel=lt8("Cancel");
#if 0
printf("language:%s\n",language);
   int code=0;
   //char *message = locale->translate("Alert code %d");
   char *message = lt8("Alert code %d");
   fl_alert(message, code);
   free(message);
#endif	
//fl_alert(netriderdirpath, 0); //test

  sprintf(buf,"%s",GUI::self->startpage); //set startpage  
  if (argc > 1) {
    url = argv[1];
    sprintf(buf,"%s",url);
  }
  checkurl((char*) buf);

  //sprintf(GUI::self->startpage,"%s",buf); //set startpage
  GUI::self->browserwin[GUI::currenttab]->tool->loc->value(buf); //put in URL location field

  //std::vector<std::string> args;
  std::copy(&argv[1], &argv[argc], std::back_inserter(args));
  view[0] = new WebKit2(args);

  Fl::add_timeout(0.01, Timer_CB, (void*)view[0]);       // wait 1 secs before animation begins

  return(Fl::run());
}

/*************************************************************************
* save and retrieve configuration data in netrider.prefs file
**************************************************************************/

void readPrefs() {
    
  int boolValue;
  int intValue;
  char buffer[1024];
  char *flexbuffer=NULL;
  double doubleValue;
  int i, n, j;
#if 0
  const char *envhome = "HOME";
  char currdir[ FL_PATH_MAX ] = ".";
  char *homedir;
  homedir = getenv(envhome);
  
  if (homedir!=NULL) sprintf(currdir,"%s",homedir); //subfolders do not work
  
  Fl_Preferences netrider( currdir, "NetRider", "netrider" );
#endif  

  //leave company field blank
  //will result in: c:/Documents and Settings/<user name here>/Application Data//netrider/settings/netrider.pref
  Fl_Preferences netrider( netriderdirpath, "", "settings/netrider" );

#if 0 
  char path[ FL_PATH_MAX ];
  netrider.getUserdataPath( path, sizeof(path) );
  printf("%s\n",path);
#endif
   Fl_Preferences settings( netrider, "settings" );
      settings.get( "language", buffer, "",128);
      //printf("language-get:%s\n",buffer);
      //strcpy(language,buffer); 
      strcpy(Fl_I18n::current_language,buffer);
      settings.get( "startpage", buffer, "www.google.com",1023); //set google as default
      if (strlen(buffer)==0) sprintf(buffer,"www.google.com"); //if above does not work
      strcpy(GUI::self->startpage,buffer); //buf=url

  Fl_Preferences history( netrider, "history" );
    {
      history.get( "historyitemcount", n, 0 );
      j=0;
      for ( i=0; i<n; i+=2 ) { //step by two!
        if (i>0) j=i/2;
        history.get( Fl_Preferences::Name( i ), flexbuffer, "" );
        sprintf(GUI::self->URL[j].name,"%s",flexbuffer);
        history.get( Fl_Preferences::Name( i+1 ), flexbuffer, "" );
        sprintf(GUI::self->URL[j].uri,"%s",flexbuffer);
      } //for
    }

  Fl_Preferences bookmarks( netrider, "bookmarks" );
    {
      j=0;
      bookmarks.get( "bookmarksitemcount", n, 0 );
      for ( i=0; i<n; i+=2 ){ //step by two!
        if (i>0) j=i/2;
        bookmarks.get( Fl_Preferences::Name( i ), flexbuffer, "" );
        sprintf(GUI::self->bookmarks[j].name,"%s",flexbuffer);
        bookmarks.get( Fl_Preferences::Name( i+1 ), flexbuffer, "" );
        sprintf(GUI::self->bookmarks[j].uri,"%s",flexbuffer);
      }  //for
    } 
  if ( flexbuffer ) free( flexbuffer );      
}
/**************************************************************************/
void writePrefs() {

  if (prefs_written) return; //already done in exit callback
  
  int i, n, j;
  char thecount[10];
  char itemnr[10];
#if 0    
  const char *envhome = "HOME";
  char currdir[ FL_PATH_MAX ] = ".";
  char *homedir;
  homedir = getenv(envhome);
    
  if (homedir!=NULL) sprintf(currdir,"%s",homedir); //subfolders do not work

  Fl_Preferences netrider( currdir, "NetRider", "netrider" );  
#endif

  //leave company field blank
  //will result in: c:/Documents and Settings/<user name here>/Application Data//netrider/settings/netrider.pref
  Fl_Preferences netrider( netriderdirpath, "", "settings/netrider" );

  Fl_Preferences settings( netrider, "settings" );
      //printf("language-set:%s\n",language);
      settings.set( "language", Fl_I18n::current_language); //language);
      settings.flush();
      settings.set( "startpage", GUI::self->startpage);
      settings.flush();

  Fl_Preferences history( netrider, "history" );
    {
      //first count historyitems
      for (n=0;n<MAXHISTORY;n++){
	if (GUI::self->URL[n].uri[0]=='\0') break;  
      }
      sprintf(thecount,"%i",n*2); //url and tile both count here
      history.set( "historyitemcount", thecount);  

      i=j=0;
      //while (GUI::self->URL[j].uri[0]!='\0') {
      //for (n=0;n<thecount;n++){
      while (j < n) {
	//if (GUI::self->URL[j].uri[0]=='\0') break;  
	sprintf(itemnr,"%i",i);
	history.set( itemnr, GUI::self->URL[j].name);
	sprintf(itemnr,"%i",i+1);
        history.set( itemnr, GUI::self->URL[j].uri);
        i+=2; //step by two!
        if (i>0) j=i/2;
      }
    }
      settings.flush();
  
  Fl_Preferences bookmarks( netrider, "bookmarks" );
    {
      i=j=0;
      while (GUI::self->bookmarks[j].uri[0]!='\0') {
	sprintf(itemnr,"%i",i);
	bookmarks.set( itemnr, GUI::self->bookmarks[j].name);
	sprintf(itemnr,"%i",i+1);
        bookmarks.set( itemnr, GUI::self->bookmarks[j].uri);
	i+=2; //step by two!
        if (i>0) j=i/2;
      }
    }
      sprintf(thecount,"%i",i);
      bookmarks.set( "bookmarksitemcount", thecount);   

}
