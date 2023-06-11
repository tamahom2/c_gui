//
// Created by badaouii on 18/05/2023.
//

#include "ei_widget_attributes.h"
#include "ei_application.h"
#include "ei_widget.h"
#include "ei_implementation.h"

const ei_rect_t* ei_widget_get_screen_location(ei_widget_t widget){
    return &widget->screen_location;
}

void *ei_widget_get_user_data(ei_widget_t widget){
    return widget->user_data;
}

ei_widget_t ei_widget_get_last_child(ei_widget_t widget){
    return widget->children_tail;
}

ei_widget_t ei_widget_get_first_child(ei_widget_t widget){
    return widget->children_head;
}

ei_widgetclass_t *ei_widget_get_class(ei_widget_t widget){
    return widget->wclass;
}

ei_widget_t ei_widget_get_next_sibling(ei_widget_t widget){
    return widget->next_sibling;
}

void	 		ei_widget_set_content_rect	(ei_widget_t		widget,
                                               const ei_rect_t*	content_rect){
    widget->content_rect = *content_rect;
}

ei_widget_t ei_widget_get_parent(ei_widget_t widget){
    return widget->parent;
}

const ei_rect_t* ei_widget_get_content_rect(ei_widget_t widget){
    return &widget->content_rect;
}

const ei_color_t* ei_widget_get_pick_color(ei_widget_t widget){
    return &widget->pick_color;
}

bool ei_widget_is_displayed(ei_widget_t widget){
    ei_widget_t root = ei_app_root_widget();
    return (root->screen_location.top_left.x<=widget->screen_location.top_left.x &&
            widget->screen_location.top_left.x<=root->screen_location.top_left.x+root->screen_location.size.width &&
            root->screen_location.top_left.y<=widget->screen_location.top_left.y &&
            widget->screen_location.top_left.y<=root->screen_location.top_left.y+root->screen_location.size.height);
}