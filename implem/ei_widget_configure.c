

#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_widget_configure.h"
#include "ei_widget_frame.h"
#include "ei_application.h"
#include "ei_widget_button.h"
#include "ei_widget_toplevel.h"
/**
 * @brief	Configures the attributes of widgets of the class "frame".
 *
 *		Parameters obey the "optional parameters" protocol: if a parameter is "NULL", the state
 *		of the configured frame for that parameter does not change.
 *		When a frame is created, its parameters are set with the default values specified here.
 *
 * @param	widget		The widget to configure.
 * @param	requested_size	The size requested for this widget, including the widget's borders.
 *				The geometry manager may override this size due to other constraints.
 *				Defaults to the "natural size" of the widget, ie. big enough to
 *				display the border and the text or the image. This may be (0, 0)
 *				if the widget has border_width=0, and no text and no image.
 * @param	color		The color of the background of the widget. Defaults to
 *				\ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border decoration of the widget. The final
 *				appearance depends on the "relief" parameter. Defaults to 0.
 * @param	relief		Appearance of the border of the widget. Defaults to
 *				\ref ei_relief_none.
 * @param	text		The text to display in the widget, or NULL. Only one of the
 *				parameter "text" and "img" should be used (i.e. non-NULL). Defaults
 *				to NULL.
 * @param	text_font	The font used to display the text. Defaults to \ref ei_default_font.
 * @param	text_color	The color used to display the text. Defaults to
 *				\ref ei_font_default_color.
 * @param	text_anchor	The anchor of the text, i.e. where it is placed within the widget.
 *				Defines both the anchoring point on the parent and on the widget.
 *				Defaults to \ref ei_anc_center.
 * @param	img		The image to display in the widget, or NULL. Any surface can be
 *				used, but usually a surface returned by \ref hw_image_load. Only one
 *				of the parameter "text" and "img" should be used (i.e. non-NULL).
 				Defaults to NULL.
 * @param	img_rect	If not NULL, the rectangle defines a subpart of "img" to use as the
 *				image displayed in the widget. Defaults to NULL.
 * @param	img_anchor	The anchor of the image, i.e. where it is placed within the widget
 *				when the size of the widget is bigger than the size of the image.
 *				Defaults to \ref ei_anc_center.
 */
void    ei_frame_configure	(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 ei_relief_t*		relief,
							 ei_string_t*		text,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_ptr_t*		img_rect,
							 ei_anchor_t*		img_anchor){
    ei_frame_t *frame = ((ei_frame_t *) widget);
    if(color!=NULL)
        (frame)->color = *color;
    if(border_width!=NULL)
        (frame)->border_width = *border_width;
    if(relief!=NULL)
        (frame)->relief = *relief;
    if(text!=NULL) {
        if(*text!=NULL) {
            (frame)->text = calloc(strlen(*text), 1);
            strcpy((frame)->text, *text);

        }else{
            frame->text = "\0";
        }
        (frame)->text_font = ei_default_font;
        (frame)->text_anchor = ei_anc_center;
        (frame)->text_color = ei_font_default_color;
        }
    if(text_font!=NULL)
        (frame)->text_font = *text_font;
    if(text_color!=NULL)
        (frame)->text_color = *text_color;
    if(text_anchor!=NULL)
        (frame)->text_anchor = *text_anchor;
    if(img!=NULL) {
        ei_size_t size = hw_surface_get_size(*img);
        (frame)->img = hw_surface_create(*img,size,true);
        hw_surface_lock(frame->img);
        hw_surface_lock(*img);
        ei_copy_surface((frame)->img,NULL,*img,NULL,true);
        hw_surface_unlock(frame->img);
        hw_surface_unlock(*img);
        ei_rect_t rect = hw_surface_get_rect(*img);
        (frame)->img_rect = rect;
        (frame)->img_anchor = ei_anc_center;
    }
    if(img_rect!=NULL)
        (frame)->img_rect = **img_rect;
    if(img_anchor!=NULL)
        (frame)->img_anchor = *img_anchor;
    if(requested_size!=NULL) {
        widget->requested_size = *requested_size;
        widget->screen_location.size = *requested_size;
    }else if(text!=NULL && *text!=NULL){
        int width=0,height=0;
        hw_text_compute_size(*text,frame->text_font,&width,&height);
        widget->requested_size.height = height;
        widget->requested_size.width = width;
    }
    ei_app_invalidate_rect(&widget->screen_location);
}
/**
 * @brief	Configures the attributes of widgets of the class "((ei_button_t *) widget)".
 *
 * @param	widget, requested_size, color, border_width, relief,
 *		text, text_font, text_color, text_anchor,
 *		img, img_rect, img_anchor
 *				See the parameter definition of \ref ei_frame_configure. The only
 *				difference is that relief defaults to \ref ei_relief_raised
 *				and border_width defaults to \ref k_default_((ei_button_t *) widget)_border_width.
 * @param	corner_radius	The radius (in pixels) of the rounded corners of the ((ei_button_t *) widget).
 *				0 means straight corners. Defaults to \ref k_default_((ei_button_t *) widget)_corner_radius.
 * @param	callback	The callback function to call when the user clicks on the ((ei_button_t *) widget).
 *				Defaults to NULL (no callback).
 * @param	user_param	A programmer supplied parameter that will be passed to the callback
 *				when called. Defaults to NULL.
 */
void			ei_button_configure			(ei_widget_t		widget,
                                            ei_size_t*		requested_size,
                                            const ei_color_t*	color,
                                            int*			border_width,
                                            int*			corner_radius,
                                            ei_relief_t*		relief,
                                            ei_string_t*		text,
                                            ei_font_t*		text_font,
                                            ei_color_t*		text_color,
                                            ei_anchor_t*		text_anchor,
                                            ei_surface_t*		img,
                                            ei_rect_ptr_t*		img_rect,
                                            ei_anchor_t*		img_anchor,
                                            ei_callback_t*		callback,
                                            ei_user_param_t*	user_param){
    ei_button_t *button = (ei_button_t*) widget;
    if(color!=NULL)
        (button)->color = *color;
    if(border_width!=NULL)
        (button)->border_width = *border_width;
    if(relief!=NULL)
        (button)->relief = *relief;
    if(text!=NULL) {
        (button)->text = malloc(strlen(*text)+1);
        strcpy((button)->text,*text);
        (button)->text_font = ei_default_font;
        if( (button)->text_anchor == 0) (button)->text_anchor = ei_anc_center;
        (button)->text_color = ei_font_default_color;
        ei_app_invalidate_rect(&widget->screen_location);
    }
    if(text_font!=NULL)
        (button)->text_font = *text_font;
    if(text_color!=NULL)
        (button)->text_color = *text_color;
    if(text_anchor!=NULL)
        (button)->text_anchor = *text_anchor;
    if(img!=NULL) {
        if(*img!=NULL){
            ei_size_t size = hw_surface_get_size(*img);
            (button)->img = hw_surface_create(*img,size,true);
            hw_surface_lock(button->img);
            hw_surface_lock(*img);
            ei_copy_surface((button)->img,NULL,*img,NULL,true);
            hw_surface_unlock(button->img);
            hw_surface_unlock(*img);
            ei_rect_t rect = hw_surface_get_rect(*img);
            (button)->img_rect = rect;
            widget->requested_size = rect.size;
            if( (button)->img_anchor == 0) (button)->img_anchor = ei_anc_center;
        }else{
            (button)->img = NULL;
        }

    }
    if(img_rect!=NULL)
        (button)->img_rect = **img_rect;
    if(img_anchor!=NULL)
        (button)->img_anchor = *img_anchor;
    if(corner_radius!=NULL)
        (button)->corner_radius = *corner_radius;
    else
        (button)->corner_radius = k_default_button_corner_radius;
    if(callback!=NULL)
        (button)->callback = *callback;
    if(user_param!=NULL)
        (button)->user_param = *user_param;
    if(requested_size!=NULL) {
        widget->requested_size = *requested_size;
        widget->screen_location.size = *requested_size;
    }else if(text!=NULL){
        int width=0,height=0;
        hw_text_compute_size(*text,button->text_font,&width,&height);
        widget->requested_size.height = height;
        widget->requested_size.width = width;
    }


}

/**
 * @brief	Configures the attributes of widgets of the class "toplevel".
 *
 * @param	widget		The widget to configure.
 * @param	requested_size	The content size requested for this widget, this does not include
 *				the decorations	(border, title bar). The geometry manager may
 *				override this size due to other constraints.
 *				Defaults to (320x240).
 * @param	color		The color of the background of the content of the widget. Defaults
 *				to \ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border of the widget. Defaults to 4.
 * @param	title		The string title displayed in the title bar. Defaults to "Toplevel".
 *				Uses the font \ref ei_default_font.
 * @param	closable	If true, the toplevel is closable by the user, the toplevel must
 *				show a close button in its title bar. Defaults to true.
 * @param	resizable	Defines if the widget can be resized horizontally and/or vertically
 *				by the user. Defaults to \ref ei_axis_both.
 * @param	min_size	For resizable widgets, defines the minimum size of its content.
 *				The default minimum size of a toplevel is (160, 120).
 *				If *min_size is NULL, this requires the min_size to be configured to
 *				the default size.
 */
void			ei_toplevel_configure		(ei_widget_t		widget,
                                              ei_size_t*		requested_size,
                                              ei_color_t*		color,
                                              int*			border_width,
                                              ei_string_t*		title,
                                              bool*			closable,
                                              ei_axis_set_t*		resizable,
                                              ei_size_ptr_t*		min_size){
    ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
    if(color!=NULL){
        toplevel->color = *color;
    }
    if(border_width!=NULL){
        toplevel->border_width = *border_width;
    }
    if(title!=NULL){
        toplevel->title = malloc(strlen(*title)+1);
        strcpy((toplevel)->title,*title);
    }
    if(closable!=NULL){
        toplevel->closable = *closable;
    }
    if(resizable!=NULL){
        toplevel->resizable = *resizable;
    }
    if(min_size!=NULL){
        toplevel->min_size = *min_size;
    }
    if(requested_size!=NULL){
        int width=0,height=0;
        if(title!=NULL){
            hw_text_compute_size(*title,ei_default_font,&width,&height);
        }
        widget->requested_size = *requested_size;
        widget->requested_size.height+= height;
        widget->screen_location.size = *requested_size;
        widget->screen_location.size.height += height;
        widget->content_rect.size = *requested_size;
        ((ei_toplevel_t *) widget)->title_height = height;
    }
}