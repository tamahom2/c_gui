//
// Created by badaouii on 10/05/2023.
//
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_event.h"
#include "ei_application.h"
#include "ei_implementation.h"

ei_default_handle_func_t DEFAULT_HANDLE_FUNC = NULL;
ei_widget_t  ACTIVE_WIDGET = NULL;

void				ei_event_set_active_widget(ei_widget_t widget){
    ACTIVE_WIDGET = widget;
}

ei_widget_t			ei_event_get_active_widget(void){
    return ACTIVE_WIDGET;
}

void	ei_event_set_default_handle_func(ei_default_handle_func_t func){
    DEFAULT_HANDLE_FUNC = func;
}

ei_default_handle_func_t	ei_event_get_default_handle_func(void){
    return DEFAULT_HANDLE_FUNC;
}