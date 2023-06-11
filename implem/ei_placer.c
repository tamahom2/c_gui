

#include "ei_types.h"
#include "ei_widget.h"
#include "ei_implementation.h"
#include "ei_widget_button.h"
#include "ei_widget_toplevel.h"
#include "ei_utils.h"
#include "ei_application.h"

/**
 * \brief	Update the placement of the widget and its children
 *
 * @param	widget		The widget to update from screen.
 */
void ei_impl_placer_run(ei_widget_t widget){
    if(widget->parent==NULL) return;
    ei_app_invalidate_rect(&widget->screen_location);
    int old_x = widget->screen_location.top_left.x,old_y = widget->screen_location.top_left.y,old_width = widget->screen_location.size.width,old_height = widget->screen_location.size.height;
    if (widget->placer_params->rel_height != 0) {
            widget->screen_location.size.height = widget->parent->content_rect.size.height * (widget->placer_params->rel_height)+widget->placer_params->height;
    } else if(widget->placer_params->height != 0){
            widget->screen_location.size.height = widget->placer_params->height;
    }
    if (widget->placer_params->rel_width != 0) {
            widget->screen_location.size.width = widget->parent->content_rect.size.width * (widget->placer_params->rel_width)+widget->placer_params->width;
    } else if(widget->placer_params->width != 0){
            widget->screen_location.size.width = widget->placer_params->width;
    }

    if(widget->placer_params->anchor==ei_anc_center){
        //Center
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x -
                widget->screen_location.size.width/2;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y -
                widget->screen_location.size.height/2;
    }else if(widget->placer_params->anchor==ei_anc_east){
        //E
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y -
                widget->screen_location.size.height/2;
    }else if(widget->placer_params->anchor==ei_anc_north){
        //N
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x -
                widget->screen_location.size.width/2;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y;
    }else if(widget->placer_params->anchor==ei_anc_northeast){
        //NE
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x -
                widget->screen_location.size.width;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y;

    }else if(widget->placer_params->anchor==ei_anc_northwest){
        //NW
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y;
    }else if(widget->placer_params->anchor==ei_anc_south){
        //S
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x -
                widget->screen_location.size.width/2;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y -
                widget->screen_location.size.height;
    }else if(widget->placer_params->anchor==ei_anc_southeast){
        //SE
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x-
                widget->screen_location.size.width;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y-
                widget->screen_location.size.height;

    }else if(widget->placer_params->anchor==ei_anc_southwest){
        //SW
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height)*widget->placer_params->rel_y +
                widget->placer_params->abs_y -
                widget->screen_location.size.height;
    }else{
        //WEST
        widget->screen_location.top_left.x =
                (widget->parent->content_rect.size.width)*widget->placer_params->rel_x +
                widget->placer_params->abs_x;
        widget->screen_location.top_left.y =
                (widget->parent->content_rect.size.height) * widget->placer_params->rel_y +
                widget->placer_params->abs_y -
                widget->screen_location.size.height/2;
    }

    widget->screen_location.top_left.x += widget->parent->content_rect.top_left.x;
    widget->screen_location.top_left.y += widget->parent->content_rect.top_left.y;
    /*if(!strcmp(widget->parent->wclass->name,"toplevel")){
        widget->screen_location.top_left.y += 10;
    }*/

    if((old_x != widget->screen_location.top_left.x ||
       old_y != widget->screen_location.top_left.y ||
       old_width != widget->screen_location.size.width ||
       old_height != widget->screen_location.size.height) ){
        widget->content_rect.top_left = widget->screen_location.top_left;
        if(widget->screen_location.size.width<0){
            widget->screen_location.size.width = 0;
        }
        if(widget->screen_location.size.height<0){
            widget->screen_location.size.height = 0;
        }
        if(!strcmp(widget->wclass->name,"toplevel")){
            widget->content_rect.top_left.y += ((ei_toplevel_t *) widget)->title_height;
            widget->content_rect.top_left.x += ((ei_toplevel_t *) widget)->border_width;
            widget->content_rect.size.width += widget->screen_location.size.width - old_width;
            widget->content_rect.size.height += widget->screen_location.size.height - old_height;
        }else{
            widget->content_rect.size = widget->screen_location.size;
        }
        ei_rect_t inter = widget->screen_location;
        if(widget->screen_location.top_left.x<0){
            inter.top_left.x =0;
        }
        if(widget->screen_location.top_left.y<0){
            inter.top_left.y = 0;
        }
        ei_app_invalidate_rect(&inter);

        if(widget->wclass->geomnotifyfunc!=NULL) widget->wclass->geomnotifyfunc(widget);
        ei_widget_t visiting = widget->children_head;
        while(visiting!=NULL){
            ei_impl_placer_run(visiting);
            visiting = visiting->next_sibling;
        }
    }


}

/**
 * \brief	Configures the geometry of a widget using the "placer" geometry manager.
 *
 *		The placer computes a widget's geometry relative to its parent *content_rect*.
 *
 * 		If the widget was already managed by the "placer", then this calls simply updates
 *		the placer parameters: arguments that are not NULL replace previous values.
 *
 * 		When the arguments are passed as NULL, the placer uses default values (detailed in
 *		the argument descriptions below). If no size is provided (either absolute or
 *		relative), then either the requested size of the widget is used if one was provided,
 *		or the default size is used.
 *
 * @param	widget		The widget to place.
 * @param	anchor		How to anchor the widget to the position defined by the placer
 *				(defaults to ei_anc_northwest).
 * @param	x		The absolute x position of the widget (defaults to 0).
 * @param	y		The absolute y position of the widget (defaults to 0).
 * @param	width		The absolute width for the widget (defaults to the requested width or
 * 				the default width of the widget if rel_width is NULL, or 0 otherwise).
 * @param	height		The absolute height for the widget (defaults to the requested height or
 *				the default height of the widget if rel_height is NULL, or 0 otherwise).
 * @param	rel_x		The relative x position of the widget: 0.0 corresponds to the left
 *				side of the master, 1.0 to the right side (defaults to 0.0).
 * @param	rel_y		The relative y position of the widget: 0.0 corresponds to the top
 *				side of the master, 1.0 to the bottom side (defaults to 0.0).
 * @param	rel_width	The relative width of the widget: 0.0 corresponds to a width of 0,
 *				1.0 to the width of the master (defaults to 0.0).
 * @param	rel_height	The relative height of the widget: 0.0 corresponds to a height of 0,
 *				1.0 to the height of the master (defaults to 0.0).
 */
void		ei_place	(ei_widget_t		widget,
                         ei_anchor_t*		anchor,
                         int*			x,
                         int*			y,
                         int*			width,
                         int*			height,
                         float*			rel_x,
                         float*			rel_y,
                         float*			rel_width,
                         float*			rel_height){

    if(widget->placer_params==NULL)
        widget->placer_params = calloc(1,sizeof(ei_impl_placer_params_t));
    if(anchor!=NULL){
        widget->placer_params->anchor = *anchor;
    }else if(widget->placer_params->anchor == 0){
        widget->placer_params->anchor = ei_anc_northwest;
    }
    if(x!=NULL){
        widget->placer_params->abs_x =  *x;
        if(widget->parent == NULL){
            widget->screen_location.top_left.x = 0;
        }
    }else if(widget->placer_params->abs_x == 0){
        widget->placer_params->abs_x = 0;
        if(widget->parent == NULL)
            widget->screen_location.top_left.x = 0;
    }
    if(y!=NULL){
        widget->placer_params->abs_y = *y;
        if(widget->parent == NULL){
            widget->screen_location.top_left.y = 0;
        }
    }else if(widget->placer_params->abs_y == 0){
        widget->placer_params->abs_y = 0;
        if(widget->parent == NULL)
            widget->screen_location.top_left.y = 0;
    }
    if(width != NULL){
        widget->placer_params->width = *width;
    }else if(rel_width==NULL && widget->placer_params->width==0){
        widget->placer_params->width = widget->requested_size.width;
    }
    if(height!=NULL){
        widget->placer_params->height = *height;
    }else if(rel_height==NULL && widget->placer_params->height==0){
        widget->placer_params->height = widget->requested_size.height;
    }
    if(rel_x!=NULL){
        widget->placer_params->rel_x = *rel_x;
    }
    if(rel_y!=NULL){
        widget->placer_params->rel_y = *rel_y;
    }
    if(rel_width!=NULL){
        widget->placer_params->rel_width = *rel_width;
    }
    if(rel_height!=NULL){
        widget->placer_params->rel_height = *rel_height;
    }
    //after placement we update the position
    ei_impl_placer_run(widget);

}



/**
 * \brief	Tells the placer to remove a widget from the screen and forget about it.
 *		Note: the widget is not destroyed and still exists in memory.
 *
 * @param	widget		The widget to remove from screen.
 */
void		ei_placer_forget(ei_widget_t widget){
    if(widget->placer_params != NULL) free(widget->placer_params);
}