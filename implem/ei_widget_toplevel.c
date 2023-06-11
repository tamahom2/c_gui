//
// Created by badaouii on 10/05/2023.
//

#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_draw.h"
#include "ei_arc.h"
#include "ei_application.h"
#include "ei_widget_toplevel.h"
#include "ei_arc.h"
#include "ei_utils.h"
#include "ei_widget_configure.h"
#include "ei_event.h"

bool *DRAWN_CLOSE = NULL;
bool *DRAWN_RESIZABLE = NULL;
ei_point_t OLD_POSITION;
/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class: both the common attributes, that use
 *		\ref ei_widget_struct_size bytes, and the specific attributes of the class.
 *		After allocation, the function *must* initialize the memory to 0.
 *
 * @return			A block of memory big enough to represent a widget of this class,
 * 				with all bytes set to 0.
 */
ei_widget_t ei_toplevel_allocfunc(){
    return calloc(1,sizeof(ei_toplevel_t)+1);
}

/*
 * button_close
 *  \brief close the toplevel and it's children
 *	Callback called when a user clicks on the button close.
 */
void close_toplevel(ei_widget_t widget, ei_event_t* event, ei_user_param_t user_param)
{
    printf("Destroying the toplevel\n");
    ei_widget_destroy(widget->parent);
}

/*
 * resize --
 *
 *	Callback called when a user clicks on the button close.
 */
void resize_toplevel(ei_widget_t widget, ei_event_t* event, ei_user_param_t user_param)
{
    if(event->type == ei_ev_mouse_buttondown) {
        OLD_POSITION = event->param.mouse.where;
    }
    if(event->type == ei_ev_mouse_move){
        ei_point_t new_position  = event->param.mouse.where;
        int new_width = widget->parent->screen_location.size.width;
        int new_height = widget->parent->screen_location.size.height;
        if(((ei_toplevel_t *) widget->parent)->resizable == ei_axis_both || ((ei_toplevel_t *) widget->parent)->resizable == ei_axis_x) {
            new_width += new_position.x - OLD_POSITION.x;
        }
        if(((ei_toplevel_t *) widget->parent)->resizable == ei_axis_both || ((ei_toplevel_t *) widget->parent)->resizable == ei_axis_y) {
            new_height += new_position.y - OLD_POSITION.y;
        }
        ei_place(widget->parent,NULL,NULL,NULL, &(int){new_width},&(int){new_height},NULL,NULL,NULL,NULL);
        OLD_POSITION = new_position;
    }
    if(event->type == ei_ev_mouse_buttonup){
        ei_event_set_active_widget(widget->parent->parent);
    }
}

/**
 * \brief	A function that releases the memory used by a widget before it is destroyed.
 *		The memory used for the \ref ei_widget_t structure itself must *not* be freed by
 *		the function. It frees the *fields* of the structure, if necessary.
 *		Can be set to NULL in \ref ei_toplevel_t if no memory is used by a class of widget.
 *
 * @param	widget		The widget which resources are to be freed.
 */
void	ei_toplevel_releasefunc(ei_widget_t	widget){
    ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
    if(toplevel->title!= NULL) free(toplevel->title);
    //if(toplevel->min_size!=NULL) free(toplevel->min_size);
}

/**
 * \brief	A function that draws a toplevel.
 * 		The function must also draw the children of the widget.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		A locked surface where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expreei_widgetclass_allocfunc_t *) ssed in the surface reference toplevel).
 */
void	ei_toplevel_drawfunc(ei_widget_t		widget,
                          ei_surface_t		surface,
                          ei_surface_t		pick_surface,
                          ei_rect_t*		clipper){
        //lets start by doing the toplevel frame
        if(clipper==NULL) return;
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
        int width = 0,height = 0;
        if(toplevel->title!=NULL){
            hw_text_compute_size(toplevel->title,toplevel->font,&width,&height);
        }

        ei_point_t *pts = malloc(185*sizeof(ei_point_t));
        ei_point_t center1 = ei_point(widget->screen_location.top_left.x+height,widget->screen_location.top_left.y+height);
        arc(pts,90,center1,height,180,270);
        ei_point_t center2 = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width-height,widget->screen_location.top_left.y+height);
        arc(pts+90,90,center2,height,270,360);
        pts[180] = ei_point(widget->screen_location.top_left.x+widget->screen_location.size.width,widget->screen_location.top_left.y+widget->screen_location.size.height);
        pts[181] = ei_point(widget->screen_location.top_left.x,widget->screen_location.top_left.y+widget->screen_location.size.height);
        pts[182] = pts[0];
        ei_color_t color_tinted = {toplevel->color.red*0.75,toplevel->color.green*0.75,toplevel->color.blue*0.75,toplevel->color.alpha};

        ei_draw_polygon(surface,pts,183,color_tinted,clipper);
        if(pick_surface!=NULL)
            ei_draw_polygon(pick_surface,pts,183,widget->pick_color,clipper);
        free(pts);
        ei_point_t pts1 = ei_point(widget->screen_location.top_left.x+toplevel->border_width,widget->screen_location.top_left.y+height+toplevel->border_width);
        ei_rect_t clipper_frame = ei_rect(pts1, ei_size(widget->screen_location.size.width-2*toplevel->border_width,widget->screen_location.size.height-height-2*toplevel->border_width));
        int x1,x2,y1,y2;
        //Get intersection of clipper and the clipper frame
        x1 = fmax(clipper->top_left.x,clipper_frame.top_left.x);
        x2 = fmin(clipper->top_left.x+clipper->size.width,clipper_frame.top_left.x+clipper_frame.size.width);
        y1 = fmax(clipper->top_left.y,clipper_frame.top_left.y);
        y2 = fmin(clipper->top_left.y+clipper->size.height,clipper_frame.top_left.y+clipper_frame.size.height);
        if(x1<x2 && y1<y2){
            ei_rect_t intersection = ei_rect(ei_point(x1,y1), ei_size(x2-x1,y2-y1));
            ei_fill(surface,&toplevel->color,&intersection);
        }
        /*if(!SET_CONTENT_RECT[widget->pick_id]) {
            ei_rect_t content= ei_rect(ei_point(widget->screen_location.top_left.x + toplevel->border_width,
                                                 widget->screen_location.top_left.y + 10+toplevel->border_width),
                                        ei_size(widget->screen_location.size.width - 2 * toplevel->border_width,
                                                   widget->screen_location.size.height - 10));
            *widget->content_rect = content;
            SET_CONTENT_RECT[widget->pick_id] = true;
        }*/
        if(toplevel->title!=NULL) {
            ei_string_t title = toplevel->title;
            ei_font_t font = toplevel->font;
            ei_color_t color = {0,0,0,255};
            ei_point_t where_text = ei_point(widget->screen_location.top_left.x+height,widget->screen_location.top_left.y);
            //Get intersection of clipper and the clipper frame
            x1 = fmax(clipper->top_left.x,where_text.x);
            x2 = fmin(clipper->top_left.x+clipper->size.width,where_text.x+width);
            y1 = fmax(clipper->top_left.y,where_text.y);
            y2 = fmin(clipper->top_left.y+clipper->size.height,where_text.y+height);
            if(x1<x2 && y1<y2){
                ei_rect_t intersection = ei_rect(ei_point(x1,y1), ei_size(x2-x1,y2-y1));
                ei_draw_text(surface,&where_text,title,font,color,&intersection);
            }
        }
        //draw the close button
        if(toplevel->closable && !DRAWN_CLOSE[widget->pick_id]){
            ei_widget_t button_close = ei_widget_create("button",widget,NULL,NULL);
            ei_button_configure(button_close,&(ei_size_t){20,20},
                            &(ei_color_t){0xff, 0, 0, 0xff},
                            NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                            &(ei_callback_t){close_toplevel},NULL);
            ei_place(button_close,&(ei_anchor_t){ei_anc_northwest},&(int){6},&(int){-toplevel->title_height+6},NULL,NULL,&(float){0},&(float){0},NULL,NULL);
            DRAWN_CLOSE[widget->pick_id] = true;
        }
        //Draw the resize button
        if(toplevel->resizable != ei_axis_none && !DRAWN_RESIZABLE[widget->pick_id]){
            ei_widget_t button_resize = ei_widget_create("button",widget,NULL,NULL);
            ei_button_configure(button_resize,&(ei_size_t){10,10},
                                &color_tinted,
                                &(int){0},&(int){0},NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                                &(ei_callback_t){resize_toplevel},NULL);
            ei_place(button_resize,&(ei_anchor_t){ei_anc_southeast},&(int){0},&(int){0},NULL,NULL,&(float){1},&(float){1},NULL,NULL);
            DRAWN_RESIZABLE[widget->pick_id] = true;
        }
}



/**ei_toplevel_allocfunc
 * \brief	A function that sets the default values for a toplevel.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	ei_toplevel_setdefaultsfunc(ei_widget_t		widget){
    widget->requested_size = ei_size(320,240);
    ((ei_toplevel_t *) widget)->color = ei_default_background_color;
    ((ei_toplevel_t *) widget)->border_width = 4;
    ((ei_toplevel_t *) widget)->font = ei_default_font;
    ((ei_toplevel_t *) widget)->closable = true;
    ((ei_toplevel_t *) widget)->resizable = ei_axis_both;
    ((ei_toplevel_t *) widget)->min_size = calloc(1,sizeof(ei_size_ptr_t));
    ((ei_toplevel_t *) widget)->min_size->height = 120;
    ((ei_toplevel_t *) widget)->min_size->width = 160;
    ((ei_toplevel_t *) widget)->title_height = 0;
    DRAWN_CLOSE = calloc(1000,sizeof(bool));
    DRAWN_RESIZABLE = calloc(1000,sizeof(bool));
}

/**
 * \brief 	A function that is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		The new location can be retrieved by \ref ei_widget_get_screen_location.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 */
void	ei_toplevel_geomnotifyfunc_t	(ei_widget_t	widget){
    if(widget->screen_location.size.height<((ei_toplevel_t *) widget)->min_size->height){
        ei_place(widget,NULL,NULL,NULL,NULL,&((ei_toplevel_t *) widget)->min_size->height, NULL ,NULL,NULL,NULL);
    }
    if(widget->screen_location.size.width<((ei_toplevel_t *) widget)->min_size->width){
        ei_place(widget,NULL,NULL,NULL,&((ei_toplevel_t *) widget)->min_size->width,NULL, NULL ,NULL,NULL,NULL);
    }
    //ei_place_xy(widget,widget->screen_location.top_left.x,widget->screen_location.top_left.y);
}


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
bool ei_toplevel_handlefunc(ei_widget_t widget,struct ei_event_t*	event){
    if (event->type == ei_ev_mouse_buttondown) {
            OLD_POSITION = event->param.mouse.where;
            return true;
        }
    else if (event->type == ei_ev_mouse_move) {
            ei_point_t new_position = event->param.mouse.where;
            int x_deviation = new_position.x - OLD_POSITION.x;
            int y_deviation = new_position.y - OLD_POSITION.y;
            ei_place_xy(widget, widget->screen_location.top_left.x + x_deviation,
                        widget->screen_location.top_left.y + y_deviation);
            OLD_POSITION = new_position;
        }
    else if (event->type == ei_ev_mouse_buttonup) {
            ei_event_set_active_widget(widget->parent);
        }
    return false;
}


