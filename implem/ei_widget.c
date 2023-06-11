#include <assert.h>

#include "ei_widget.h"
#include "ei_implementation.h"
#include "ei_utils.h"
#include "ei_application.h"
#include "ei_types.h"
#include "ei_event.h"

int PICK_ID_GLOBAL = 0;
ei_widget_t		ei_widget_create		(ei_const_string_t	class_name,
							 ei_widget_t		parent,
							 ei_user_param_t	user_data,
							 ei_widget_destructor_t destructor){
    ei_widgetclass_t *wclass = ei_widgetclass_from_name(class_name);
    ei_widget_t widget = wclass->allocfunc();
    widget->wclass = wclass;
    widget->parent = parent;
    //Creation of the pick color
    widget->pick_id = PICK_ID_GLOBAL;
    int R,G,B,A;
    A = 255;
    B = PICK_ID_GLOBAL%256;
    G = ((PICK_ID_GLOBAL-B)/256)%256;
    R = (PICK_ID_GLOBAL)/(256*256);
    widget->pick_color = (ei_color_t){R,G,B,A};
    if(parent!=NULL){
        if(parent->children_head != NULL){
            //The widget will become the next sibling of last and then will become the last

            parent->children_tail->next_sibling = widget;
            parent->children_tail = widget;
        }else{
            //The widget is the first and last child of his parent
           parent->children_head = widget;
           parent->children_tail = widget;
        }
    }
    widget->wclass->setdefaultsfunc(widget);
    if(!strcmp(class_name,"testclass")){
        widget->content_rect = widget->screen_location;
    }
    widget->destructor = destructor;
    widget->user_data = user_data;
    PICK_ID_GLOBAL++;
    return widget;
}

/**
 * @brief	Destroys a widget.
 * 		Removes the widget from the screen if it is currently displayed.
 * 		Destroys all its descendants.
 *		Calls its destructor if it was provided.
 * 		Frees the memory used by the widget (e.g. the widget param).
 *
 * @param	widget		The widget that is to be destroyed.
 */
void			ei_widget_destroy		(ei_widget_t		widget){
    ei_widget_t cur_child = widget;
    if(cur_child==NULL){
        return;
    }else if(cur_child->children_head==NULL){

        if(cur_child==cur_child->parent->children_head){
            cur_child->parent->children_head = cur_child->next_sibling;
            if(cur_child==cur_child->parent->children_tail){
                cur_child->parent->children_tail = NULL;
            }
        }else {
            ei_widget_t tmp = cur_child->parent->children_head;
            while (tmp != NULL && tmp->next_sibling != cur_child) {
                tmp = tmp->next_sibling;
            }
            tmp->next_sibling = cur_child->next_sibling;
            if(cur_child==cur_child->parent->children_tail){
                cur_child->parent->children_tail = tmp;
            }
        }
        ei_app_invalidate_rect(&widget->screen_location);
        ei_placer_forget(widget);
        widget->wclass->releasefunc(widget);
        if(widget->destructor !=NULL) widget->destructor(widget);
        widget = NULL;
        free(widget);
        //ei_event_set_active_widget(widget->parent);
        return;
    }else{
            cur_child = cur_child->children_head;
        }
    while(cur_child != NULL && cur_child->parent!=NULL && cur_child!=cur_child->parent->children_tail){
        ei_widget_destroy(cur_child);
        cur_child = cur_child->next_sibling;
    }
    ei_widget_destroy(cur_child);
    //and now we destroy the widget
    ei_widget_destroy(widget);
    ei_event_set_active_widget(widget->parent);
}



