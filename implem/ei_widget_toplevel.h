//
// Created by badaouii on 10/05/2023.
//

#ifndef EI_WIDGET_TOPLEVEL_H
#define EI_WIDGET_TOPLEVEL_H

#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_widget.h"
#include "ei_implementation.h"
/**
 * \brief	A structure that stores information about a toplevel.
 */
typedef struct ei_toplevel_t {
    ei_impl_widget_t		widget;
    ei_size_t		requested_size;
    ei_color_t		color;
    int		border_width;
    ei_string_t		title;
    ei_font_t  font;
    bool		closable;
    ei_axis_set_t		resizable;
    ei_size_ptr_t		min_size;
    int title_height;
} ei_toplevel_t;

/**
 * @brief	A function that is called in response to an event. This function
 *		is internal to the library. It implements the generic behavior of
 *		a widget (for example a button looks sunken when clicked)
 *toplevel
 * @param	widget		The widget for which the event was generated.
 * @param	event		The event containing all its parameters (type, etc.)
 *
 * @return			A boolean telling if the event was consumed by the callback or not.
 *				If true, the library does not try to call other callbacks for this
 *				event. If false, the library will call the next callback registered
 *				for this event, if any.
 *				Note: The callback may execute many operations and still return
 *				false, or return true without having done anything.
 */
bool ei_toplevel_handlefunc(ei_widget_t widget,struct ei_event_t*	event);

/**
 * \brief	A function that sets the default values for a toplevel.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	ei_toplevel_setdefaultsfunc(ei_widget_t		widget);


/**
 * \brief	A function that draws a toplevel.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		A locked surface where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expreei_widgetclass_allocfunc_t *) ssed in the surface reference).
 */
void	ei_toplevel_drawfunc(ei_widget_t		widget,
                           ei_surface_t		surface,
                           ei_surface_t		pick_surface,
                           ei_rect_t*		clipper);


/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class: both the common attributes, that use
 *		\ref ei_widget_struct_size bytes, and the specific attributes of the class.
 *		After allocation, the function *must* initialize the memory to 0.
 *
 * @return			A block of memory big enough to represent a widget of this class,
 * 				with all bytes set to 0.
 */
ei_widget_t ei_toplevel_allocfunc();

/**
 * \brief	A function that releases the memory used by a widget before it is destroyed.
 *		The memory used for the \ref ei_widget_t structure itself must *not* be freed by
 *		the function. It frees the *fields* of the structure, if necessary.
 *		Can be set to NULL in \ref ei_toplevel_t if no memory is used by a class of widget.
 *
 * @param	widget		The widget which resources are to be freed.
 */
void	ei_toplevel_releasefunc(ei_widget_t	widget);

/**
 * \brief 	A function that is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		The new location can be retrieved by \ref ei_widget_get_screen_location.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 */
void	ei_toplevel_geomnotifyfunc_t	(ei_widget_t	widget);


#endif
