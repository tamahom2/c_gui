#include <stdbool.h> 
#include <string.h>
#include <assert.h>

#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_arc.h"
#include "ei_widget_frame.h"
#include "ei_application.h"
#include "ei_utils.h"

/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class: both the common attributes, that use
 *		\ref ei_widget_struct_size bytes, and the specific attributes of the class.
 *		After allocation, the function *must* initialize the memory to 0.
 *
 * @return			A block of memory big enough to represent a widget of this class,
 * 				with all bytes set to 0.
 */
ei_widget_t ei_frame_allocfunc(){
    return calloc(1,sizeof(ei_frame_t)+1);
}

/**
 * \brief	A function that releases the memory used by a widget before it is destroyed.
 *		The memory used for the \ref ei_widget_t structure itself must *not* be freed by
 *		the function. It frees the *fields* of the structure, if necessary.
 *		Can be set to NULL in \ref ei_frame_t if no memory is used by a class of widget.
 *
 * @param	widget		The widget which resources are to be freed.
 */
 void	ei_frame_releasefunc(ei_widget_t	widget){
        ei_frame_t *frame = (ei_frame_t *) widget;
        //if(frame->text_font!=NULL) hw_text_font_free(frame->text_font);
        if(frame->img!=NULL)hw_surface_free(frame->img);
        free(frame->text);
        //if(frame->img_rect!=NULL)free(frame->img_rect);
 }

/**
 * \brief	A function that draws a frame.
 * 		The function must also draw the children of the widget.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		A locked surface where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expreei_widgetclass_allocfunc_t *) ssed in the surface reference frame).
 */
void	ei_frame_drawfunc(ei_widget_t		widget,
						  ei_surface_t		surface,
						  ei_surface_t		pick_surface,
						  ei_rect_t*		clipper){
        if(clipper==NULL) return;
        ei_frame_t *frame = (ei_frame_t *) widget;
        //start by filling the surface
        if(frame->border_width==0) {
            ei_fill(surface, &frame->color, clipper);
            if(pick_surface!=NULL)
                ei_fill(pick_surface,&widget->pick_color,clipper);
        }else{
            int border = frame->border_width;
            ei_rect_t rect_big = ei_rect(ei_point(widget->screen_location.top_left.x,widget->screen_location.top_left.y), widget->screen_location.size);
            ei_rect_t rect_small = ei_rect(ei_point(widget->screen_location.top_left.x+border,widget->screen_location.top_left.y+border),
                                      ei_size(widget->screen_location.size.width-2*border,widget->screen_location.size.height-2*border));
            draw_button(surface,frame->color,rect_big,rect_small,0,0,frame->relief,clipper);
            if(pick_surface!=NULL)
                draw_button(pick_surface,widget->pick_color,rect_big,rect_small,0,0,frame->relief,clipper);
        }
        if(frame->text != NULL){
            //We should put the text
            ei_string_t text = ((ei_frame_t *) widget)->text;
            ei_font_t font = ((ei_frame_t *) widget)->text_font;
            ei_color_t color = ((ei_frame_t *) widget)->text_color;
            int width,height;
            hw_text_compute_size(text,font,&width,&height);
            ei_point_t  where_text = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width/2-width/2,
                                              widget->screen_location.top_left.y+widget->screen_location.size.height/2-height/2);
            int x1,x2,y1,y2;
            x1 = fmax(clipper->top_left.x,where_text.x);
            x2 = fmin(clipper->top_left.x+clipper->size.width,where_text.x+width);
            y1 = fmax(clipper->top_left.y,where_text.y);
            y2 = fmin(clipper->top_left.y+clipper->size.height,where_text.y+height);
            if(x1<x2 && y1<y2) {
                ei_rect_t intersection = ei_rect(ei_point(x1, y1), ei_size(x2 - x1, y2 - y1));
                ei_draw_text(surface, &where_text, text, font, color, &intersection);
            }
        }else if(frame->img != NULL)
        {
            //It's an image
            ei_surface_t img = ((ei_frame_t *) widget)->img;
            ei_rect_t rect_img = ((ei_frame_t *) widget)->img_rect;
            ei_size_t img_size = hw_surface_get_size(img);
            ei_point_t  where_img = ei_point(widget->screen_location.top_left.x,
                                              widget->screen_location.top_left.y);
            int x1,x2,y1,y2;
            x1 = fmax(clipper->top_left.x,where_img.x);
            x2 = fmin(clipper->top_left.x+clipper->size.width,where_img.x+img_size.width);
            y1 = fmax(clipper->top_left.y,where_img.y);
            y2 = fmin(clipper->top_left.y+clipper->size.height,where_img.y+img_size.height);
            if(x1<x2 && y1<y2) {
                ei_rect_t intersection = ei_rect(ei_point(x1, y1), ei_size(x2 - x1, y2 - y1));
                ei_rect_t src_rect = ei_rect(rect_img.top_left,intersection.size);
                hw_surface_lock(img);
                ei_copy_surface(surface,&intersection,img,&src_rect,true);
                hw_surface_unlock(img);
            }
        }
}



/**ei_frame_allocfunc
 * \brief	A function that sets the default values for a frame.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	ei_frame_setdefaultsfunc(ei_widget_t		widget){
    ((ei_frame_t *) widget)->color = ei_default_background_color;
    ((ei_frame_t *) widget)->border_width = 0;
    ((ei_frame_t *) widget)->relief = ei_relief_none;
    ((ei_frame_t *) widget)->text_font = ei_default_font;
    widget->content_rect = widget->screen_location;

}
