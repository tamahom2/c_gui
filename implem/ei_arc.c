#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "hw_interface.h"
#include "ei_arc.h"
#include "ei_draw.h"
#include "ei_types.h"



/* arc --
 *
 *	Draws an arc 
 */
void arc(ei_point_t *points,
        int total_points,
        ei_point_t center, 
        int radius,
        int angle_start,
        int angle_finish)
{
    int start = angle_start;
	int			i;
    float theta;
	/* Draw the square */
	for(i = 0; i < total_points; i++) {
		/*	Calculate coordinates of each point */
        theta = (start*M_PI)/180.0;
		points[i].x	=  floor(center.x+radius*cos(theta));
		points[i].y	=  floor(center.y+radius*sin(theta));
        start++;
    }
}

void rounded_frame(ei_point_t **pts,
                   ei_rect_t rect,
                   int radius,
                   int total){
    //Creation of the upper frame
    int h = rect.size.height/2;
    if(total==0){
        /*First we create the first arc*/
        int angle_start1 = 180,angle_finish1 = 270;
        int angle_start2 = 270,angle_finish2 = 315;
        int angle_start3 = 135,angle_finish3 = 180;
        int cur = 0;
        int total_points = 90+45+45+3;
        *pts = malloc(total_points*sizeof(ei_point_t)+1);
        assert(*pts!=NULL);
        ei_point_t center = {rect.top_left.x+radius,rect.top_left.y+radius};
        arc(*pts,90,center,radius,angle_start1,angle_finish1);
        cur += 90;
        ei_point_t center1 = {rect.top_left.x+rect.size.width-radius,rect.top_left.y+radius};
        arc(*pts+cur,45,center1,radius,angle_start2,angle_finish2);
        cur += 45;
        //the second line
        (*pts+cur)->x = rect.top_left.x+rect.size.width-h;
        (*pts+cur)->y = rect.top_left.y+h;
        cur++;  
        //the third line
        (*pts+cur)->x = rect.top_left.x+h;
        (*pts+cur)->y = rect.top_left.y+h;
        cur++;
        ei_point_t center2 = {rect.top_left.x+radius,rect.top_left.y+rect.size.height-radius};
        arc(*pts+cur,45,center2,radius,angle_start3,angle_finish3);
        cur += 45;
        (*pts+cur)->x = (*pts)->x;
        (*pts+cur)->y = (*pts)->y;
    }
    //Build the lower frame
    else if(total==1){
        /*First we create the first arc*/
        int angle_start1 = 0,angle_finish1 = 90;        
        int angle_start2 = 90,angle_finish2 = 135;
        int angle_start3 = 315,angle_finish3 = 360;

        int cur = 0;
        int total_points = 90+45+45+3;
        *pts = malloc(total_points*sizeof(ei_point_t)+1);
        assert(*pts!=NULL);
        ei_point_t center = {rect.top_left.x+rect.size.width-radius,rect.top_left.y+rect.size.height-radius};
        ei_point_t center1 = {rect.top_left.x+radius,rect.top_left.y+rect.size.height-radius};
        ei_point_t center2 = {rect.top_left.x+rect.size.width-radius,rect.top_left.y+radius};
        arc(*pts,90,center,radius,angle_start1,angle_finish1);
        cur += 90;
        arc(*pts+cur,45,center1,radius,angle_start2,angle_finish2);
        cur += 45;
        //the second line
        (*pts+cur)->x = rect.top_left.x+h;
        (*pts+cur)->y = rect.top_left.y+rect.size.height-h;
        cur++;
        //the third line
        (*pts+cur)->x = rect.top_left.x+rect.size.width-h;
        (*pts+cur)->y = rect.top_left.y+h;
        cur++;
        arc(*pts+cur,45,center2,radius,angle_start3,angle_finish3);
        cur += 45;
        (*pts+cur)->x = (*pts)->x;
        (*pts+cur)->y = (*pts)->y;
    }else{
        int angle_start1 = 0,angle_finish1 = 90;        
        int angle_start2 = 90,angle_finish2 = 180;
        int angle_start3 = 180,angle_finish3 = 270;
        int angle_start4 = 270,angle_finish4 = 360;
        int total_points = 361;
        int cur = 0;
        *pts = malloc(total_points*sizeof(ei_point_t)+1);
        assert(*pts!=NULL);
        ei_point_t center = {rect.top_left.x+rect.size.width-radius,rect.top_left.y+rect.size.height-radius};
        arc(*pts,90,center,radius,angle_start1,angle_finish1);
        cur += 90;
        ei_point_t center1 = {rect.top_left.x+radius,rect.top_left.y+rect.size.height-radius};
        arc(*pts+cur,90,center1,radius,angle_start2,angle_finish2);
        cur += 90;
        ei_point_t center2 = {rect.top_left.x+radius,rect.top_left.y+radius};;
        arc(*pts+cur,90,center2,radius,angle_start3,angle_finish3);
        cur += 90;
        ei_point_t center3 = {rect.top_left.x+rect.size.width-radius,rect.top_left.y+radius};  
        arc(*pts+cur,90,center3,radius,angle_start4,angle_finish4);
        cur += 90;
        (*pts+cur)->x = (*pts)->x;
        (*pts+cur)->y = (*pts)->y;
    }
}


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
				 const ei_rect_t*	clipper){
        ei_point_t *pts = NULL;
        int total_points = 183;
        ei_color_t color_tinted = {color.red*0.75,color.green*0.75,color.blue*0.75,color.alpha};
        ei_color_t color_shaded = {color.red + (255-color.red)*0.25,color.green + (255-color.green)*0.25,255-color.blue+(255-color.blue)*0.25,color.alpha};
        if(relief==ei_relief_sunken){
            ei_color_t tmp = color_shaded;
            color_shaded = color_tinted;
            color_tinted = tmp;
        }else if(relief == ei_relief_none){
            color_shaded = color;
            color_tinted = color;
        }
        rounded_frame(&pts,big_frame,radius_big,0);
        ei_draw_polygon(surface,pts,total_points,color_shaded,clipper);
        pts = NULL;
        rounded_frame(&pts,big_frame,radius_big,1);
        ei_draw_polygon(surface,pts,total_points,color_tinted,clipper);
        pts = NULL;
        total_points = 361;
        rounded_frame(&pts,small_frame,radius_small,2);
        ei_draw_polygon(surface,pts,total_points,color,clipper);
}

