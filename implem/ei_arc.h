/**
 * @file	ei_arc.h
 *
 * @brief 	Private definitions.
 * 
 */

#ifndef EI_ARC_H
#define EI_ARC_H

#include "hw_interface.h"
#include "ei_types.h"
#include "ei_draw.h"

/**
 * \brief	Creates a list of points that represents an arc
 *
 * @param	points	
 * @param	total_points	
 *
 */
void arc(ei_point_t *points,
        int total_points,
        ei_point_t center, 
        int radius,
        int angle_start,
        int angle_finish);


/**
 * \brief	Creates a list of points that represents a rounded frame
 *
 * @param	pts	
 * @param	rect	
 *
 */
void rounded_frame(ei_point_t **pts,
                   ei_rect_t rect,
                   int radius,
                   int total);

/**
 * @brief Draws a button
*/
void draw_button(ei_surface_t		surface,
		 ei_color_t		color,
                 ei_rect_t big_frame,
                 ei_rect_t small_frame,
                 int radius_big,
                 int radius_small,
                 ei_relief_t relief,
		 const ei_rect_t*	clipper);

#endif
