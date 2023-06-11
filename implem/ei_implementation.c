/**
 * @file	ei_implementation.c
 *
 * @brief 	Private definitions.
 * 
 */
#include <math.h>
#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_implementation.h"
#include "ei_application.h"
#include "ei_widget_frame.h"

/**
 * \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
 * 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
 * 		The surface parameter provides the channel order.
 *
 * @param	surface		The surface where to store this pixel, provides the channels order.
 * @param	color		The color to convert.
 *
 * @return 			The 32 bit integer corresponding to the color. The alpha component
 *				of the color is ignored in the case of surfaces that don't have an
 *				alpha channel.
 */
uint32_t ei_impl_map_rgba(ei_surface_t surface, ei_color_t color){
    /**
     * We start by getting the indexes of each color
    */
    int ir,ig,ib,ia;
    hw_surface_get_channel_indices(surface,&ir,&ig,&ib,&ia);
    /**
     * we get the colors
    */
    uint8_t r,g,b,a;
    r = color.red;
    g = color.green;
    b = color.blue;
    a = color.alpha;
    /**
     * Calculate the value
    */

    uint32_t c = (r<<(ir*8)) + (g<<(8*ig)) + (b<<(8*ib)) + (a<<(8*ia));
    return c;

}

void ei_impl_widget_draw_children      (ei_widget_t		widget,
                                        ei_surface_t		surface,
                                        ei_surface_t		pick_surface,
                                        ei_rect_t*		clipper){
    if(clipper==NULL){
        return;
    }
    if(widget==ei_app_root_widget()){
        widget->wclass->drawfunc(widget, surface, pick_surface, clipper);
    }
    ei_widget_t cur_child = widget->children_head;
    if(cur_child==NULL){
        return;
    }
    ei_rect_t *inter_clipper = NULL;
    int x1,x2,y1,y2;
    while(cur_child!=NULL){
        inter_clipper = NULL;
        if(cur_child->screen_location.size.width <= 0 || cur_child->screen_location.size.height <= 0) {
            cur_child = cur_child->next_sibling;
            continue;
        }
        //Get intersection of clipper and the clipper frame
        x1 = fmax(clipper->top_left.x,cur_child->screen_location.top_left.x);
        x2 = fmin(clipper->top_left.x+clipper->size.width,cur_child->screen_location.top_left.x+cur_child->screen_location.size.width);
        y1 = fmax(clipper->top_left.y,cur_child->screen_location.top_left.y);
        y2 = fmin(clipper->top_left.y+clipper->size.height,cur_child->screen_location.top_left.y+cur_child->screen_location.size.height);
        if(x1<x2 && y1<y2) {
            inter_clipper = calloc(1,sizeof(ei_rect_t));
            *inter_clipper = (ei_rect_t) ei_rect(ei_point(x1, y1), ei_size(x2 - x1, y2 - y1));
        }
        /*We should add the case if it's inside the toplevel to change the clipper*/
        cur_child->wclass->drawfunc(cur_child,surface,pick_surface,inter_clipper);
        cur_child = cur_child->next_sibling;
    }
    cur_child = widget->children_head;

    while(cur_child != NULL){
        //Get intersection of clipper and the clipper frame
        inter_clipper=NULL;
        if(cur_child->screen_location.size.width <= 0 || cur_child->screen_location.size.height <= 0) {
            cur_child = cur_child->next_sibling;
            continue;
        }
        if(!strcmp(cur_child->wclass->name,"toplevel")){
            x1 = fmax(clipper->top_left.x, cur_child->screen_location.top_left.x);
            x2 = fmin(clipper->top_left.x + clipper->size.width,
                      cur_child->screen_location.top_left.x + cur_child->screen_location.size.width);
            y1 = fmax(clipper->top_left.y, cur_child->screen_location.top_left.y);
            y2 = fmin(clipper->top_left.y + clipper->size.height,
                      cur_child->screen_location.top_left.y + cur_child->screen_location.size.height);
        }else {
            if(cur_child->content_rect.size.height < 0) cur_child->content_rect.size.height = 0;
            if(cur_child->content_rect.size.width < 0) cur_child->content_rect.size.width = 0;

            x1 = fmax(clipper->top_left.x, cur_child->content_rect.top_left.x);
            x2 = fmin(clipper->top_left.x + clipper->size.width,
                      cur_child->content_rect.top_left.x + cur_child->content_rect.size.width);
            y1 = fmax(clipper->top_left.y, cur_child->content_rect.top_left.y);
            y2 = fmin(clipper->top_left.y + clipper->size.height,
                      cur_child->content_rect.top_left.y + cur_child->content_rect.size.height);
        }
        if(x1<x2 && y1<y2) {
            inter_clipper = calloc(1,sizeof(ei_rect_t));
            *inter_clipper = (ei_rect_t) ei_rect(ei_point(x1, y1), ei_size(x2 - x1, y2 - y1));
        }
        ei_impl_widget_draw_children(cur_child,surface,pick_surface,inter_clipper);
        cur_child = cur_child->next_sibling;
    }
    free(inter_clipper);
}