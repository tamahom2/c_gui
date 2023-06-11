//
// Created by badaouii on 08/05/2023.
//

#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "hw_interface.h"
#include "ei_arc.h"
#include "ei_implementation.h"
#include "ei_widget_button.h"
#include "ei_widget_configure.h"
#include "ei_utils.h"
#include "ei_event.h"
#include "ei_application.h"

/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class: both the common attributes, that use
 *		\ref ei_widget_struct_size bytes, and the specific attributes of the class.
 *		After allocation, the function *must* initialize the memory to 0.
 *
 * @return			A block of memory big enough to represent a widget of this class,
 * 				with all bytes set to 0.
 */
ei_widget_t ei_button_allocfunc(){
    return calloc(1,sizeof(ei_button_t)+1);
}

/**
 * \brief	A function that releases the memory used by a widget before it is destroyed.
 *		The memory used for the \ref ei_widget_t structure itself must *not* be freed by
 *		the function. It frees the *fields* of the structure, if necessary.
 *		Can be set to NULL in \ref ei_button_t if no memory is used by a class of widget.
 *
 * @param	widget		The widget which resources are to be freed.
 */
void	ei_button_releasefunc(ei_widget_t	widget){
    ei_button_t *button = (ei_button_t *) widget;
    if(button->text!=NULL) free(button->text);
    //if(button->text_font!=NULL) hw_text_font_free(button->text_font);
    if(button->img!=NULL)hw_surface_free(button->img);
    //if(button->img_rect!=NULL)free(button->img_rect);
}

/**
 * \brief	A function that draws a button.
 * 		The function must also draw the children of the widget.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		A locked surface where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expreei_widgetclass_allocfunc_t *) ssed in the surface reference button).
 */
void	ei_button_drawfunc(ei_widget_t		widget,
                          ei_surface_t		surface,
                          ei_surface_t		pick_surface,
                          ei_rect_t*		clipper){
    //start by filling the surface
    if(clipper==NULL){
        return;
    }
    ei_button_t *button = (ei_button_t *) widget;
    int border = (button)->border_width;
    ei_rect_t rect_big = ei_rect(ei_point(widget->screen_location.top_left.x,widget->screen_location.top_left.y), widget->screen_location.size);
    ei_rect_t rect_small = ei_rect(ei_point(widget->screen_location.top_left.x+border,widget->screen_location.top_left.y+border),
                                   ei_size(widget->screen_location.size.width-2*border,widget->screen_location.size.height-2*border));
    draw_button(surface,
                ((ei_button_t *) widget)->color,
                rect_big,
                rect_small,
                ((ei_button_t *) widget)->corner_radius,
                ((ei_button_t *) widget)->corner_radius,
                ((ei_button_t *) widget)->relief,
                clipper);
    //Draw in picking surface
    if(pick_surface!=NULL)
        draw_button(pick_surface,
                    widget->pick_color,
                    rect_big,
                    rect_small,
                    ((ei_button_t *) widget)->corner_radius,
                    ((ei_button_t *) widget)->corner_radius,
                    ((ei_button_t *) widget)->relief,
                    clipper);
    if(button->text != NULL){
        //We should put the text
        ei_string_t text = ((ei_frame_t *) widget)->text;
        ei_font_t font = ((ei_frame_t *) widget)->text_font;
        ei_color_t color = ((ei_frame_t *) widget)->text_color;
        int width,height;
        hw_text_compute_size(text,font,&width,&height);
        int width_text_middle = rect_small.size.width-width;
        int height_text_middle = rect_small.size.height-height;
        ei_point_t where_text ;
        if(button->text_anchor == ei_anc_center){
            where_text = ei_point(rect_big.top_left.x+width_text_middle/2,rect_big.top_left.y+height_text_middle/2);
        }else if(button->text_anchor == ei_anc_west){
            where_text = ei_point(rect_big.top_left.x,rect_big.top_left.y+height_text_middle/2);
        }else if(button->text_anchor == ei_anc_southeast){
            where_text = ei_point(rect_big.top_left.x+width_text_middle,rect_big.top_left.y+height_text_middle);
        }else if(button->text_anchor == ei_anc_south){
            where_text = ei_point(rect_big.top_left.x+width_text_middle/2,rect_big.top_left.y+height_text_middle);
        }else if(button->text_anchor == ei_anc_southwest){
            where_text = ei_point(rect_big.top_left.x,rect_big.top_left.y+height_text_middle);
        }else if(button->text_anchor == ei_anc_east){
            where_text = ei_point(rect_big.top_left.x,rect_big.top_left.y+height_text_middle/2);
        }else if(button->text_anchor == ei_anc_northeast){
            where_text = ei_point(rect_big.top_left.x+width_text_middle,rect_big.top_left.y);
        }else if(button->text_anchor == ei_anc_northwest){
            where_text = ei_point(rect_big.top_left.x,rect_big.top_left.y);
        }else{
            where_text = ei_point(rect_big.top_left.x+width_text_middle/2,rect_big.top_left.y);
        }
        int x1,x2,y1,y2;
        x1 = fmax(clipper->top_left.x,where_text.x);
        x2 = fmin(clipper->top_left.x+clipper->size.width,where_text.x+width);
        y1 = fmax(clipper->top_left.y,where_text.y);
        y2 = fmin(clipper->top_left.y+clipper->size.height,where_text.y+height);
        if(x1<x2 && y1<y2){
            ei_rect_t intersection = ei_rect(ei_point(x1,y1), ei_size(x2-x1,y2-y1));
            ei_draw_text(surface,&where_text,text,font,color,&intersection);
        }
    }else if(button->img != NULL)
    {
        //It's an image
        ei_surface_t img = (button->img);
        ei_rect_t rect_img = button->img_rect;
        ei_size_t img_size = rect_img.size;
        ei_point_t  where_img ;
        if(button->img_anchor == ei_anc_center){
            where_img = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width/2-img_size.width/2,
                                 widget->screen_location.top_left.y+widget->screen_location.size.height/2-img_size.height/2);
        }else if(button->img_anchor == ei_anc_west){
            where_img = ei_point(widget->screen_location.top_left.x,
                                 widget->screen_location.top_left.y+widget->screen_location.size.width/2-img_size.height/2);
        }else if(button->img_anchor == ei_anc_southeast){
            where_img = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width-img_size.width,
                                 widget->screen_location.top_left.y+widget->screen_location.size.height-img_size.height);
        }else if(button->img_anchor == ei_anc_south){
            where_img = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width/2-img_size.width/2,
                                 widget->screen_location.top_left.y+widget->screen_location.size.height-img_size.height);
        }else if(button->img_anchor == ei_anc_southwest){
            where_img = ei_point(widget->screen_location.top_left.x,
                                 widget->screen_location.top_left.y+widget->screen_location.size.height-img_size.height);
        }else if(button->img_anchor == ei_anc_east){
            where_img = ei_point(widget->screen_location.top_left.x,
                                 widget->screen_location.top_left.y+widget->screen_location.size.height/2-img_size.height/2);
        }else if(button->img_anchor == ei_anc_northeast){
            where_img = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width-img_size.width,
                                 widget->screen_location.top_left.y);
        }else if(button->img_anchor == ei_anc_northwest){
            where_img = ei_point(widget->screen_location.top_left.x,
                                 widget->screen_location.top_left.y);
        }else{
            where_img = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width/2-img_size.width/2,
                                 widget->screen_location.top_left.y);
        }

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



/**
 * \brief	A function that sets the default values for a button.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	ei_button_setdefaultsfunc(ei_widget_t		widget){
    ((ei_button_t *) widget)->color = ei_default_background_color;
    ((ei_button_t *) widget)->border_width = 0;
    ((ei_button_t *) widget)->relief = ei_relief_none;
    ((ei_button_t *) widget)->corner_radius = k_default_button_corner_radius;
    ((ei_button_t *) widget)->callback = NULL;
    ((ei_button_t *) widget)->user_param = NULL;
    widget->content_rect = widget->screen_location;
}

/**
 * @brief	A function that is called in response to an event. This function
 *		is internal to the library. It implements the generic behavior of
 *		a widget (for example a button looks sunken when clicked)
 *button
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
bool ei_button_handlefunc(ei_widget_t widget,struct ei_event_t*	event){
    if(event->type ==ei_ev_mouse_buttondown){
        ((ei_button_t *) widget)->relief = ei_relief_sunken;
        if(((ei_button_t *) widget)->callback != NULL){
            ((ei_button_t *) widget)->callback(widget,event,((ei_button_t *) widget)->user_param);
        }
        ei_app_invalidate_rect(&widget->screen_location);
        return true;
    }
    if(event->type == ei_ev_mouse_move){
        if(((ei_button_t *) widget)->callback != NULL){
            ((ei_button_t *) widget)->callback(widget,event,((ei_button_t *) widget)->user_param);
        }
        ei_app_invalidate_rect(&widget->screen_location);
        return true;
    }
    if(event->type == ei_ev_mouse_buttonup){
        ((ei_button_t *) widget)->relief = ei_relief_raised;
        ei_app_invalidate_rect(&widget->screen_location);
        ei_event_set_active_widget(ei_app_root_widget());
        return true;
    }
    return false;
}



