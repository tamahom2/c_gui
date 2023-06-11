/**
 *  @file	ei_widgetclass.c
 *  @brief	Definition and registration of widget classes.
 *
 *  \author
 *  Created by François Bérard on 02.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */

#include <assert.h>

#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_widgetclass.h"
#include "ei_implementation.h"

ei_widgetclass_t *ROOT = NULL;

size_t		ei_widget_struct_size(){
    return sizeof(ei_impl_widget_t);
}

void ei_widgetclass_register(ei_widgetclass_t* widgetclass){
    if(ROOT == NULL){
        ROOT = widgetclass;
    }else{
        widgetclass->next = ROOT;
        ROOT = widgetclass;
    }
}


ei_widgetclass_t*	ei_widgetclass_from_name	(ei_const_string_t name){
    ei_widgetclass_t* pl = ROOT;
    while(pl != NULL){
        if(strcmp(pl->name,name)==0){
                return pl;
            }
            pl = pl->next;
        }
    return NULL;
}