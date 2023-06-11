

#ifndef EI_WIDGET_FRAME_H
#define EI_WIDGET_FRAME_H

#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_widget.h"
#include "ei_implementation.h"
/**
 * \brief	A structure that stores information about a frame.
 */
typedef struct ei_frame_t {
    ei_impl_widget_t		widget;
    ei_color_t	color;
    int		border_width;
    ei_relief_t		relief;
    ei_string_t		text;
    ei_font_t		text_font;
    ei_color_t		text_color;
    ei_anchor_t		text_anchor;
    ei_surface_t	img;
    ei_rect_t	img_rect;
    ei_anchor_t		img_anchor;

} ei_frame_t;


/**
 * \brief	A function that sets the default values for a frame.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	ei_frame_setdefaultsfunc(ei_widget_t		widget);


/**
 * \brief	A function that draws a frame.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		A locked surface where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expreei_widgetclass_allocfunc_t *) ssed in the surface reference button).
 */
void	ei_frame_drawfunc(ei_widget_t		widget,
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
ei_widget_t ei_frame_allocfunc();

/**
 * \brief	A function that releases the memory used by a widget before it is destroyed.
 *		The memory used for the \ref ei_widget_t structure itself must *not* be freed by
 *		the function. It frees the *fields* of the structure, if necessary.
 *		Can be set to NULL in \ref ei_button_t if no memory is used by a class of widget.
 *
 * @param	widget		The widget which resources are to be freed.
 */
void	ei_frame_releasefunc(ei_widget_t	widget);
#endif