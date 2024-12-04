#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "logging.h"

/**
 * @brief Clean and free memory for display and window
 * @param display: The X11 connection, pointer to a Display.
 * @param window: The window to destroy.
 */
void cleanup(Display* display, Window window){
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    log_msg("Successfully cleaned up window and display");
}


/**
 * @brief Main function to copy to clipboard.
 * It creates a connection to X Server, an invisible window, and
 * claims ownership of it with the provided text.
 * @param text: The text to be copied to the X11 clipboard
 */
void copy_to_clipboard(const char* text){
    Display* display;
    Window window;
    Atom clipboard, targets, utf_8_string;
    XEvent event;
    Bool served_request = False;

    int string_length = strlen(text);

    // Open connection with X Server
    display = XOpenDisplay(NULL);
    if (display == NULL){
        log_error("Unable to open connection with X Server");
        return;
    }
    log_msg("Successfully opened connection with X Server");

    // Create invisible window, as subwindow of default root window.
    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    log_msg("Successfully created a window");

    // Define clipboard atoms
    clipboard = XInternAtom(display, "CLIPBOARD", False);
    targets = XInternAtom(display, "TARGETS", False);
    utf_8_string = XInternAtom(display, "UTF8_STRING", False);
    log_msg("Created clipboard atoms");

    // Set data on clipboard
    XSetSelectionOwner(display, clipboard, window, CurrentTime);

    // Check ownership
    if (XGetSelectionOwner(display, clipboard) != window){
        log_error("Failed to set clipboard owner");
        cleanup(display, window);
        return;
    }
    log_msg("Successfully set clipboard ownership");

    // Loop until request is served
    while(!served_request){
        XNextEvent(display, &event);

        if (event.type == SelectionClear){
            log_error("Lost clipboard ownership. Exiting ...");
            break;
        }

        if (event.type != SelectionRequest) continue;

        // We are now only dealing with SelectionRequests
        XSelectionRequestEvent* req_ptr = (XSelectionRequestEvent*)&event;

        // initialize selection and populate it with req_ptr's contents
        XSelectionEvent select_event;

        select_event.type = SelectionNotify;
        select_event.display = req_ptr->display;
        select_event.requestor = req_ptr->requestor;
        select_event.selection = req_ptr->selection;
        select_event.target = req_ptr->target;
        select_event.time = req_ptr->time;

        if (req_ptr->target == targets){
            log_msg("Targets requested");
            Atom available_targets[2] = {targets, utf_8_string};

            // Change property with available targets
            XChangeProperty(req_ptr->display, req_ptr->requestor, req_ptr->property, XA_ATOM,
                            32, PropModeReplace, (unsigned char*)available_targets, 2);

            select_event.property = req_ptr->property;

        } else if (req_ptr->target == utf_8_string){
            log_msg("Requested string");

            // Respond with the clipboard content
            XChangeProperty(req_ptr->display, req_ptr->requestor, req_ptr->property, utf_8_string,
                            8, PropModeReplace, (unsigned char*)text, string_length);
            
            // Mark request as served to break the loop on the next iteration.
            select_event.property = req_ptr->property;
            served_request=True;
            log_msg("Text copied to clipboard");

        } else {
            log_error("Target is not on expected targets");
            select_event.property = None;
        }

        // Send the response
        XSendEvent(display, select_event.requestor, False, 0, (XEvent*)&select_event);
    }

    cleanup(display, window);
}
