#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widgetclass.h"
#include "ei_application.h"
#include "ei_event.h"
#include "ei_widget_frame.h"
#include "ei_widget.h"
#include "ei_widget_button.h"
#include "ei_widget_toplevel.h"
#include "ei_widget_configure.h"
ei_surface_t ROOT_SURFACE;
ei_widget_t ROOT_WIDGET;
ei_widget_t  OFFSCREEN_PICKING;
ei_rect_t  CLIPPER_TOTAL;
ei_linked_rect_t *RECT_UPDATE = NULL;
bool quit = false;


/**
 * @brief	A function that is called to register the widgetclass button
*/
void	ei_widget_button_register(){
    ei_widgetclass_t *button = malloc(sizeof(ei_widgetclass_t));
    strcpy(button->name,"button");
    button->allocfunc = &ei_button_allocfunc;
    button->releasefunc = &ei_button_releasefunc;
    button->drawfunc = &ei_button_drawfunc;
    button->setdefaultsfunc = &ei_button_setdefaultsfunc;
    button->geomnotifyfunc = NULL;
    button->handlefunc = &ei_button_handlefunc;
    button->next = NULL;
    ei_widgetclass_register(button);
}

/**
 * @brief	A function that is called to register the widgetclass frame
*/
void	ei_widget_frame_register(){
    ei_widgetclass_t *frame = malloc(sizeof(ei_widgetclass_t));
    strcpy(frame->name,"frame");
    frame->allocfunc = &ei_frame_allocfunc;
    frame->releasefunc = &ei_frame_releasefunc;
    frame->drawfunc = &ei_frame_drawfunc;
    frame->setdefaultsfunc = &ei_frame_setdefaultsfunc;
    frame->geomnotifyfunc = NULL;
    frame->handlefunc = NULL;
    frame->next = NULL;
    ei_widgetclass_register(frame);
}

/**
 * @brief	A function that is called to register the widgetclass toplevel
*/
void	ei_widget_toplevel_register(){
    ei_widgetclass_t *toplevel = malloc(sizeof(ei_widgetclass_t));
    strcpy(toplevel->name,"toplevel");
    toplevel->allocfunc = &ei_toplevel_allocfunc;
    toplevel->releasefunc = &ei_toplevel_releasefunc;
    toplevel->drawfunc = &ei_toplevel_drawfunc;
    toplevel->setdefaultsfunc = &ei_toplevel_setdefaultsfunc;
    toplevel->geomnotifyfunc = &ei_toplevel_geomnotifyfunc_t;
    toplevel->handlefunc = &ei_toplevel_handlefunc;
    toplevel->next = NULL;
    ei_widgetclass_register(toplevel);
}


/**
 * \brief	Creates an application.
 *		<ul>
 *			<li> initializes the hardware (calls \ref hw_init), </li>
 *			<li> registers all classes of widget, </li>
 *			<li> creates the root window (either in a system window, or the entire screen), </li>
 *			<li> creates the root widget to access the root window. </li>
 *		</ul>
 *
 * @param	main_window_size	If "fullscreen is false, the size of the root window of the
 *					application.
 *					If "fullscreen" is true, the current monitor resolution is
 *					used as the size of the root window. \ref hw_surface_get_size
 *					can be used with \ref ei_app_root_surface to get the size.
 * @param	fullScreen		If true, the root window is the entire screen. Otherwise, it
 *					is a system window.
 */
void ei_app_create(ei_size_t main_window_size, bool fullscreen){
    //initialize the hardware
    hw_init();
    //registers all classes
    ei_widget_frame_register();
    ei_widget_button_register();
    ei_widget_toplevel_register();
    //create the main_window
    ROOT_SURFACE = hw_create_window(main_window_size, fullscreen);
    //Create offscreen picking surface
    OFFSCREEN_PICKING = hw_surface_create(ROOT_SURFACE, hw_surface_get_size(ROOT_SURFACE), hw_surface_has_alpha(ROOT_SURFACE));
    //create the root widget
    ROOT_WIDGET = ei_widget_create("frame",NULL,NULL,NULL);
    ei_frame_configure(ROOT_WIDGET,&main_window_size,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    ei_place_xy(ROOT_WIDGET,0,0);
    CLIPPER_TOTAL = ROOT_WIDGET->screen_location;
    ei_app_invalidate_rect(&ROOT_WIDGET->content_rect);
}


void ei_app_free(void){
    if(ROOT_SURFACE!=NULL){
    hw_surface_free(ROOT_SURFACE);
    }
    if(ROOT_WIDGET->wclass!=NULL){
        ROOT_WIDGET->wclass->releasefunc(ROOT_WIDGET);
    }
    if(OFFSCREEN_PICKING!=NULL){
        hw_surface_free(OFFSCREEN_PICKING);
    }
    //free(RECT_UPDATE);
    hw_quit();
}

ei_widget_t  dfs_offscreen(uint32_t pick_id,ei_widget_t visiting){

    if(visiting == NULL || visiting->pick_id == pick_id){
        return visiting;
    }
    ei_widget_t found = dfs_offscreen(pick_id,visiting->children_head);
    if(found==NULL){
        found = dfs_offscreen(pick_id,visiting->next_sibling);
    }
    return found;
}

ei_widget_t		ei_widget_pick			(ei_point_t*		where){
    hw_surface_lock(OFFSCREEN_PICKING);
    uint32_t *pixel = (uint32_t *) hw_surface_get_buffer(OFFSCREEN_PICKING);
    ei_size_t size = hw_surface_get_size(OFFSCREEN_PICKING);

    int ir,ig,ib,ia;
    hw_surface_get_channel_indices(OFFSCREEN_PICKING,&ir,&ig,&ib,&ia);
    uint8_t *pixel_color = (uint8_t *)(pixel+where->x+where->y*size.width);
    uint32_t color = pixel_color[ir]*(256*256)+pixel_color[ig]*256+pixel_color[ib];
    hw_surface_unlock(OFFSCREEN_PICKING);
    return dfs_offscreen(color,ei_app_root_widget());

}


void ei_app_run(void){
    //we set the root widget as the starting active widget
    ei_event_set_active_widget(ei_app_root_widget());
    while(!quit) {
        struct ei_event_t *event = malloc(sizeof(struct ei_event_t));
        //First we lock the surface
        hw_surface_lock(ROOT_SURFACE);
        hw_surface_lock(OFFSCREEN_PICKING);
        //We should add pick_surface after
        //visit all widgets
        ei_widget_t visiting = ROOT_WIDGET;
        ei_impl_widget_draw_children(visiting,ROOT_SURFACE,OFFSCREEN_PICKING,&CLIPPER_TOTAL);

        //We unlock the surface and update
        hw_surface_unlock(OFFSCREEN_PICKING);
        hw_surface_unlock(ROOT_SURFACE);
        if(RECT_UPDATE!=NULL) {
            hw_surface_update_rects(ROOT_SURFACE, NULL);
        }
        RECT_UPDATE = NULL;
        visiting = ROOT_WIDGET;
        //then we wait for events
        hw_event_wait_next(event);
        //If it's a mouse click we should change the active widget
        if(event->type==ei_ev_mouse_buttondown){
            ei_point_t  mouse_click = event->param.mouse.where;
            ei_event_set_active_widget(ei_widget_pick(&mouse_click));
        }
        visiting = ei_event_get_active_widget();
        ei_default_handle_func_t default_handler_func = ei_event_get_default_handle_func();
        if(visiting!=NULL && visiting->wclass->handlefunc==NULL && default_handler_func!=NULL) default_handler_func(event);
        else if(visiting!=NULL && visiting->wclass->handlefunc != NULL){
            if(!visiting->wclass->handlefunc(visiting,event)) default_handler_func(event);
        }else{
            if(default_handler_func != NULL) default_handler_func(event);
        }

    }

}

void ei_app_invalidate_rect(const ei_rect_t* rect){
    if(RECT_UPDATE == NULL){
        RECT_UPDATE = calloc(1,sizeof(ei_linked_rect_t));
    }
    ei_linked_rect_t *linkedRect = malloc(sizeof(ei_linked_rect_t));
    linkedRect->rect = *rect;
    linkedRect->next = RECT_UPDATE;
    RECT_UPDATE = linkedRect;
    free(linkedRect);
}

void ei_app_quit_request(void){
    quit = true;
}

ei_widget_t ei_app_root_widget(void){
    return ROOT_WIDGET;
}

ei_surface_t ei_app_root_surface(void){
    return ROOT_SURFACE;
}


