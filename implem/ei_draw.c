/**
 *  @file	ei_draw.h
 *  @brief	Graphical primitives to draw lines, polygons, text, and operation of drawing
 *		surfaces.
 *
 *  \author 
 *  Created by François Bérard on 30.12.11.
 *  Copyright 2011 Ensimag. All rights reserved.
 *
 */

#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_utils.h"
#include "ei_implementation.h"



/**
 * \brief	A structure storing the polygon sides.
 *		
 */
typedef struct ei_sides_t {
	int				      ymax;
    int                   ymin;
	float			      x_ymin;
    float                 pente;
    struct ei_sides_t     *next;
} ei_sides_t;


/**
 * \brief	Fills the surface with the specified color.
 *
 * @param	surface		The surface to be filled. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	color		The color used to fill the surface. If NULL, it means that the
 *				caller want it painted black (opaque).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void    ei_fill	(ei_surface_t		surface,
				 const ei_color_t*	color,
				 const ei_rect_t*	clipper){
                    uint32_t c = 0;
                    int ir,ig,ib,ia;
                    uint8_t r=0,g=0,b=0,a=0;
                    hw_surface_get_channel_indices(surface,&ir,&ig,&ib,&ia);
                    if(color != NULL){
                        c = ei_impl_map_rgba(surface,*color);
                        r = color->red;
                        g = color->green;
                        b = color->blue;
                        a = color->alpha;
                    }
                    uint32_t* pixel = (uint32_t*)hw_surface_get_buffer(surface);
                    ei_size_t size = hw_surface_get_size(surface);
                    ei_size_t size1 = size;
                    if(clipper != NULL){
                        //Let's try analytic method
                        if((clipper->top_left).x + (clipper->size).width <= size.width){
                            size1.width = (clipper->size).width;
                        }
                        if((clipper->top_left).y + (clipper->size).height <= size.height){
                            size1.height = (clipper->size).height;
                        }
                        pixel += (clipper->top_left).y*(size.width)+(clipper->top_left).x;
                    }

                    for(int i = 0;i<size1.height;i++){
                        for (int j = 0;j<size1.width;j++){
                            if(hw_surface_has_alpha(surface)){
                                uint8_t *color_pixel = (uint8_t *)(pixel);
                                color_pixel[ir] = (r*a+(255-a)*color_pixel[ir])/255;
                                color_pixel[ig] = (g*a+(255-a)*color_pixel[ig])/255;
                                color_pixel[ib] = (b*a+(255-a)*color_pixel[ib])/255;
                                if(ia!=-1) color_pixel[ia] = 255;
                            }else {
                                *pixel = c;
                            }
                            pixel++;
                        }
                        pixel += size.width-size1.width;
                    }
                 }

/**
 * \brief	Draws a line that can be made of many line segments.
 *
 * @param	surface 	Where to draw the line. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	point_array 	The array of points defining the polyline. Its size is provided in the
 * 				next parameter (point_array_size). The array can be empty (i.e. nothing
 * 				is drawn) or it can have a single point, or more.
 *				If the last point is the same as the first point, then this pixel is
 *				drawn only once.
 * @param	point_array_size The number of points in the point_array. Can be 0.
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_draw_polyline	(ei_surface_t		surface,
				 ei_point_t*		point_array,
				 size_t			point_array_size,
				 ei_color_t		color,
				 const ei_rect_t*	clipper){
                    uint32_t c = ei_impl_map_rgba(surface,color);
                    uint32_t* pixel = (uint32_t*)hw_surface_get_buffer(surface);
                    ei_size_t size = hw_surface_get_size(surface);
                    for(size_t i=0;i<point_array_size-1;i++){
                        int x0 = point_array[i].x,x1 = point_array[i+1].x;
                        int y0 = point_array[i].y,y1 = point_array[i+1].y;
                        int dx = x1-x0,dy = y1-y0;
                        int swap = 0;
                        if(dx==0 && dy==0){
                            pixel[x0+y0*size.width] = c;
                            continue;
                        }
                        if(dx==0){
                            for(int j=y0;j!=y1; j += (dy>0)?1:-1){
                                pixel[x0+j*size.width] = c;
                            }
                            continue;
                        }else if(dy==0)
                        {
                            for(int j=x0;j!=x1; j += (dx>0)?1:-1){
                                pixel[j+y0*size.width] = c;
                            }
                            continue;
                        }
                        
                        int e = 0;
                        int incr_x = 1;
                        int incr_y = 1;
                        //Case dx<0
                        if(dx < 0){
                            dx = -dx;
                            incr_x = -1;
                        } 
                        //Case dy<0
                        if(dy < 0){
                            dy = -dy;
                            incr_y = -1;
                        }
                        //Case |dx| < |dy|
                        if(dx < dy){
                            int tmp = x0;
                            x0 = y0;
                            y0 = tmp;
                            tmp = x1;
                            x1 = y1;
                            y1 = tmp;
                            tmp = dx;
                            dx = dy;
                            dy = tmp;
                            tmp = incr_x;
                            incr_x = incr_y;
                            incr_y = tmp;
                            swap = 1;
                        }
                        while(x0!=x1){
                            if(swap){
                                //Only do bruteforce clipping for now
                                if(clipper!=NULL && (clipper->top_left).x <=y0 && y0<=(clipper->top_left).x + (clipper->size).width  && (clipper->top_left).y <= x0 && x0 <= (clipper->top_left).y + (clipper->size).height){
                                    pixel[y0+x0*size.width] = c;
                                }else if(clipper == NULL)
                                {
                                    pixel[y0+x0*size.width] = c;
                                }
                            }
                            else{
                                //Only do bruteforce clipping for now
                                if(clipper!=NULL && (clipper->top_left).x <=x0 && x0<=(clipper->top_left).x + (clipper->size).width  && (clipper->top_left).y <= y0 && y0 <= (clipper->top_left).y + (clipper->size).height){
                                    pixel[x0+y0*size.width] = c;
                                }else if(clipper == NULL)
                                {
                                    pixel[x0+y0*size.width] = c;
                                }
                            }
                                x0 += incr_x;
                                e += dy;
                                if(2*e>dx){
                                    y0 += incr_y;
                                    e -= dx;
                            }
                        }
                    }
                 }

/**
 * \brief	Insert a side in the last position.
 *
 * 
 */
void insert_last(ei_sides_t** TC,
                int ymax,
                int ymin,
	            float x_ymin,
                float pente){
    // Create a sentinel 
    ei_sides_t sent = {-1,-1,-1,-1, *TC};
    ei_sides_t *queue = &sent;
    while(queue->next != NULL){
        queue = queue->next;
    }
    queue->next  = malloc(sizeof(ei_sides_t));
    queue->next->ymax = ymax;
    queue->next->x_ymin = x_ymin;
    queue->next->ymin = ymin;
    queue->next->pente = pente;
    queue->next->next = NULL;

    *TC = sent.next;
}

/**
 * \brief	Sort sides by their x-axis (using bubblesort for now)
 *
 * 
 */
void sort_xaxis(ei_sides_t** TC){
    ei_sides_t sent = {-1,-1,-1,-1, *TC};
    ei_sides_t *queue = &sent;
    ei_sides_t *i = queue->next;
    while(i!=NULL){
        ei_sides_t *j = i;
        while(j->next != NULL){
            if(j->x_ymin > j->next->x_ymin){
                //swap both
                float tmp = j->x_ymin;
                j->x_ymin = j->next->x_ymin;
                j->next->x_ymin = tmp;
                tmp = j->pente;
                j->pente = j->next->pente;
                j->next->pente = tmp;
                int tmp1 = j->ymax;
                j->ymax = j->next->ymax;
                j->next->ymax = tmp1;
                tmp1 = j->ymin;
                j->ymin = j->next->ymin;
                j->next->ymin = tmp1;
            }
            j = j->next;
        }
        i = i->next;
    }
    *TC = sent.next;
}

/**
 * \brief	Get sides that starts in the current scanline
 *          and store them
 *
 * 
 */
void get_scanline_sides(ei_sides_t** TC, ei_sides_t** TCA, int y){
    ei_sides_t sent = {-1,-1,-1,-1, *TC};
    ei_sides_t *queue = &sent;
    while(queue->next != NULL){
        if(queue->next->ymin == y){
            insert_last(TCA,queue->next->ymax,y,queue->next->x_ymin,queue->next->pente);
            ei_sides_t *next = queue->next;
            queue->next = next->next;
            free(next);
        }else{
            queue = queue->next;
        }
        
    }
    *TC = sent.next;
}




/**
 * \brief	Delete all occ of an element
 *
 * 
 */
void delete_all_occ(ei_sides_t** TC, int ymax){
    ei_sides_t sent = {-1,-1,-1,-1, *TC};
    ei_sides_t *queue = &sent;
    while(queue->next != NULL ){
        if(queue->next->ymax == ymax){
            ei_sides_t *next = queue->next;
            queue->next = next->next;
            free(next);
        }else{
        queue = queue->next;
        }
    }

    *TC = sent.next;
}

/**
 * \brief	Draw the intersections
 *
 * 
 */
void draw_intersection(ei_sides_t* TC,
                       int current_scanline,
                       ei_surface_t		surface,
                       ei_color_t		color,
                       const ei_rect_t*	clipper){
        uint32_t c = ei_impl_map_rgba(surface,color);
        int ir,ig,ib,ia;
        hw_surface_get_channel_indices(surface,&ir,&ig,&ib,&ia);
        uint8_t r=color.red,g=color.green,b=color.blue,a=color.alpha;
        uint32_t* pixel = (uint32_t*)hw_surface_get_buffer(surface);
        ei_size_t size = hw_surface_get_size(surface);
        ei_sides_t *queue = TC;
        while(queue != NULL && queue->next != NULL){
            int x0 = ceil(queue->x_ymin);
            int x1 = floor(queue->next->x_ymin);
            for(int x=x0;x<x1;x++){
                //TODO add alpha
                if(clipper!=NULL && (clipper->top_left).x <x && x <(clipper->top_left).x + (clipper->size).width  && (clipper->top_left).y < current_scanline && current_scanline < (clipper->top_left).y + (clipper->size).height){
                            if(hw_surface_has_alpha(surface)){
                                uint8_t *color_pixel = (uint8_t *)(pixel+x+current_scanline*size.width);
                                color_pixel[ir] = (r*a+(255-a)*color_pixel[ir])/255;
                                color_pixel[ig] = (g*a+(255-a)*color_pixel[ig])/255;
                                color_pixel[ib] = (b*a+(255-a)*color_pixel[ib])/255;
                                if(ia!=-1) color_pixel[ia] = 255;
                            }else {
                                pixel[x+current_scanline*size.width] = c;
                            }

                }else if(clipper == NULL)
                {
                    if(hw_surface_has_alpha(surface)){
                        uint8_t *color_pixel = (uint8_t *)(pixel+x+current_scanline*size.width);
                        color_pixel[ir] = (r*a+(255-a)*color_pixel[ir])/255;
                        color_pixel[ig] = (g*a+(255-a)*color_pixel[ig])/255;
                        color_pixel[ib] = (b*a+(255-a)*color_pixel[ib])/255;
                        if(ia!=-1) color_pixel[ia] = 255;
                    }else {
                        pixel[x+current_scanline*size.width] = c;
                    }
                }
            }
            queue = queue->next->next;
        }
    }

/**
 * \brief	Update the value of the x axis
 *
 * 
 */
void update_xaxis(ei_sides_t** TC){
    ei_sides_t sent = {-1,-1,-1,-1, *TC};
    ei_sides_t *queue = &sent;
    while(queue->next != NULL ){
        queue->next->x_ymin += queue->next->pente;
        queue = queue->next;
    }
    *TC = sent.next;
}

/**
 * \brief	Draws a filled polygon.
 *
 * @param	surface 	Where to draw the polygon. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	point_array 	The array of points defining the polygon. Its size is provided in the
 * 				next parameter (point_array_size). The array can be empty (i.e. nothing
 * 				is drawn) or else it must have more than 2 points.
 * @param	point_array_size The number of points in the point_array. Must be 0 or more than 2.
 * @param	color		The color used to draw the polygon. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_draw_polygon		(ei_surface_t		surface,
				 ei_point_t*		point_array,
				 size_t			point_array_size,
				 ei_color_t		color,
				 const ei_rect_t*	clipper){
                    ei_sides_t *TC = NULL;
                    int current_scanline = INT32_MAX;
                    for(size_t i=0; i<point_array_size-1;i++){
                        int x0 = point_array[i].x,x1 = point_array[i+1].x;
                        int y0 = point_array[i].y,y1 = point_array[i+1].y;
                        if(y0==y1){
                            //Horizontal line
                            continue;
                        }
                        int ymax = (y0>y1)?y0:y1;
                        int ymin = (y0>y1)?y1:y0;
                        float x_ymin =  (y0>y1)?x1:x0;
                        current_scanline = (y0>y1)?(y1<current_scanline)?y1:current_scanline:(y0<current_scanline)?y0:current_scanline;
                        int dx = x1-x0,dy=y1-y0;
                        float pente = ((float) dx)/dy;
                        insert_last(&TC,ymax,ymin,x_ymin,pente);
                    }
                    //TODO : work only with int instead of float for pente
                    ei_sides_t *TCA = NULL;
                    while(TC != NULL || TCA != NULL){
                        get_scanline_sides(&TC,&TCA,current_scanline);
                        delete_all_occ(&TCA,current_scanline);
                        sort_xaxis(&TCA);
                        draw_intersection(TCA,current_scanline,surface,color,clipper);
                        current_scanline++;
                        update_xaxis(&TCA);
                    }
                 }



/**
* \brief	Copies pixels from a source surface to a destination surface.
*		The source and destination areas of the copy (either the entire surfaces, or
*		subparts) must have the same size before considering clipping.
*		Both surfaces must be *locked* by \ref hw_surface_lock.
*
* @param	destination	The surface on which to copy pixels.
* @param	dst_rect	If NULL, the entire destination surface is used. If not NULL,
*				defines the rectangle on the destination surface where to copy
*				the pixels.
* @param	source		The surface from which to copy pixels.
* @param	src_rect	If NULL, the entire source surface is used. If not NULL, defines the
*				rectangle on the source surface from which to copy the pixels.
* @param	alpha		If true, the final pixels are a combination of source and
*				destination pixels weighted by the source alpha channel and
*				the transparency of the final pixels is set to opaque.
*				If false, the final pixels are an exact copy of the source pixels,
including the alpha channel.
*
* @return			Returns 0 on success, 1 on failure (different sizes between
* 				source and destination).
*/
int	ei_copy_surface		(ei_surface_t		destination,
                               const ei_rect_t*	dst_rect,
                               ei_surface_t		source,
                               const ei_rect_t*	src_rect,
                               bool			alpha){
    ei_size_t src_size;
    ei_size_t src_total_size = hw_surface_get_size(source);
    ei_size_t dst_size;
    ei_size_t dst_total_size = hw_surface_get_size(destination);
    ei_point_t src_start;
    ei_point_t dst_start;
    if(src_rect==NULL){
        src_size = src_total_size;
        src_start = ei_point(0,0);
    }else{
        src_size = src_rect->size;
        src_start = src_rect->top_left;
    }
    if(dst_rect==NULL) {
        dst_size = dst_total_size;
        dst_start = ei_point(0,0);
    }else{
        dst_size = dst_rect->size;
        dst_start = dst_rect->top_left;
    }
    if(src_size.width != dst_size.width && src_size.height != dst_size.height){
        return 1;
    }
    uint32_t* pixel_src = (uint32_t*)hw_surface_get_buffer(source);
    uint32_t* pixel_dst = (uint32_t*)hw_surface_get_buffer(destination);
    int x0,x1,y0=src_start.y,y1=dst_start.y;
    int ir_src,ig_src,ib_src,ia_src,ir_dst,ig_dst,ib_dst,ia_dst;
    hw_surface_get_channel_indices(source,&ir_src,&ig_src,&ib_src,&ia_src);
    hw_surface_get_channel_indices(destination,&ir_dst,&ig_dst,&ib_dst,&ia_dst);
    for(int i=0;i<src_size.height;i++){
        x0=src_start.x;
        x1=dst_start.x;
        for(int j=0;j<src_size.width;j++){
            if(!alpha) {
                pixel_dst[(x1+y1*dst_total_size.width)] = pixel_src[(x0+y0*src_total_size.width)];
            } else{
                uint8_t *color_pixel_dst = (uint8_t *)(pixel_dst+(x1+y1*dst_total_size.width));
                uint8_t *color_pixel_src = (uint8_t *)(pixel_src+(x0+y0*src_total_size.width));
                color_pixel_dst[ir_dst] = (color_pixel_src[ir_src]*color_pixel_src[ia_src]+(255-color_pixel_src[ia_src])*color_pixel_dst[ir_dst])/255;
                color_pixel_dst[ig_dst] = (color_pixel_src[ig_src]*color_pixel_src[ia_src]+(255-color_pixel_src[ia_src])*color_pixel_dst[ig_dst])/255;
                color_pixel_dst[ib_dst] = (color_pixel_src[ib_src]*color_pixel_src[ia_src]+(255-color_pixel_src[ia_src])*color_pixel_dst[ib_dst])/255;
                if(ia_dst!=-1) color_pixel_dst[ia_dst] = 255;
            }
            x1++;
            x0++;
        }
        y0++;
        y1++;
    }
    return 0;
}


void	ei_draw_text		(ei_surface_t		surface,
                             const ei_point_t*	where,
                             ei_const_string_t	text,
                             ei_font_t		font,
                             ei_color_t		color,
                             const ei_rect_t*	clipper){
    int width,height;
    hw_text_compute_size(text,font,&width,&height);
    ei_rect_t rect_dst = ei_rect(ei_point(where->x,where->y), ei_size(width,height));
    ei_surface_t text_surface = hw_text_create_surface(text,font,color);
    ei_rect_t rect_text = ei_rect(ei_point(0,0), ei_size(width,height));
    if(rect_dst.top_left.x<0){
        rect_dst.size.width+=rect_dst.top_left.x;
        rect_text.top_left.x -= rect_dst.top_left.x;
        rect_text.size.width+=rect_dst.top_left.x;
        rect_dst.top_left.x = 0;
    }
    if(rect_dst.top_left.y<0){
        rect_dst.size.height+=rect_dst.top_left.y;
        rect_text.top_left.y -= rect_dst.top_left.y;
        rect_text.size.height+=rect_dst.top_left.y;

        rect_dst.top_left.y = 0;
    }
    if(clipper!=NULL){
            //Let's try analytic method
            if((clipper->top_left).x+clipper->size.width < rect_dst.top_left.x+rect_dst.size.width){
                rect_text.top_left.x += clipper->top_left.x - rect_dst.top_left.x;
                rect_dst.top_left.x = clipper->top_left.x;
            }
            if((clipper->top_left).y+clipper->size.height < rect_dst.top_left.y+rect_dst.size.height){
                rect_text.top_left.y += clipper->top_left.y - rect_dst.top_left.y;
                rect_dst.top_left.y = clipper->top_left.y;
            }
            rect_dst.size.width = clipper->size.width+ (clipper->top_left.x - rect_dst.top_left.x);
            rect_dst.size.height = clipper->size.height + (clipper->top_left.y - rect_dst.top_left.y);
            rect_text.size = rect_dst.size;
    }
    hw_surface_lock(text_surface);
    ei_copy_surface(surface,&rect_dst,text_surface,&rect_text,true);
    hw_surface_unlock(text_surface);
    hw_surface_free(text_surface);
}