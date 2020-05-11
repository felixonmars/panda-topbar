#ifndef LXQTXFITMAN_H
#define LXQTXFITMAN_H

#include <X11/Xlib.h>

/**
 * @file xfitman.h
 * @author Christopher "VdoP" Regali
 * @brief handles all of our xlib-calls.
 */

/**
 * @brief manages the Xlib apicalls
 */
class XfitMan
{
public:
    XfitMan();
    ~XfitMan();

    static Atom atom(const char* atomName);

    void moveWindow(Window _win, int _x, int _y) const;
    void raiseWindow(Window _wid) const;
    void resizeWindow(Window _wid, int _width, int _height) const;
    void closeWindow(Window _wid) const;

    bool getClientIcon(Window _wid, QPixmap& _pixreturn) const;
    bool getClientIcon(Window _wid, QIcon *icon) const;
    void setIconGeometry(Window _wid, QRect* rect = 0) const;

    QString getWindowTitle(Window _wid) const;
    QString getApplicationName(Window _wid) const;

    int clientMessage(Window _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;

private:

    /** \warning Do not forget to XFree(result) after data are processed!
    */
    bool getWindowProperty(Window window,
                           Atom atom,               // property
                           Atom reqType,            // req_type
                           unsigned long* resultLen,// nitems_return
                           unsigned char** result   // prop_return
                          ) const;

    /** \warning Do not forget to XFree(result) after data are processed!
    */
    bool getRootWindowProperty(Atom atom,               // property
                               Atom reqType,            // req_type
                               unsigned long* resultLen,// nitems_return
                               unsigned char** result   // prop_return
                              ) const;


    Window  root; //the actual root window on the used screen
};


const XfitMan& xfitMan();

#endif
