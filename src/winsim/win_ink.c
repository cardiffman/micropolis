/*
 * win_ink.c
 *
 *  Created on: Jun 23, 2018
 *      Author: Mike
 */
#include "win_view.h"
#include "sim.h"
#include <stddef.h>

#define POINT_BATCH 32

Ink *OldInk = NULL;

/* XXX: todo: ink locking so someone doesn't erase ink that's being drawn */

Ink *
NewInk(void)
{
  Ink *ink;

  if (OldInk) {
    ink = OldInk;
    OldInk = OldInk->next;
  } else {
    ink = (Ink *)malloc(sizeof(Ink));
    ink->maxlength = POINT_BATCH;
    ink->points = (POINT *)malloc(POINT_BATCH * sizeof(POINT));
  }
  ink->length = 0;
  ink->color = COLOR_WHITE;
  ink->next = NULL;
  ink->left = ink->right = ink->top = ink->bottom =
    ink->last_x = ink->last_y = -1;
  return (ink);
}

void FreeInk(Ink *ink)
{
  ink->next = OldInk;
  OldInk = ink;
}


void StartInk(Ink *ink, int x, int y)
{
  ink->length = 1;
  ink->left = ink->right = ink->last_x = ink->points[0].x = x;
  ink->top = ink->bottom = ink->last_y = ink->points[0].y = y;
}


void AddInk(Ink *ink, int x, int y)
{
  int dx = x - ink->last_x;
  int dy = y - ink->last_y;

  if ((dx != 0) || (dy != 0)) {
/*
    if (ink->length > 1) {
      if ((dx == 0) &&
	  (ink->points[ink->length - 1].x == 0) &&
	  ((ink->points[ink->length - 1].y < 0) ?
	   (dy < 0) : (dy > 0))) {
	ink->points[ink->length - 1].y += dy;
	goto ADJUST;
      } else if ((dy == 0) &&
		 (ink->points[ink->length - 1].y == 0) &&
		 ((ink->points[ink->length - 1].x < 0) ?
		  (dx < 0) : (dx > 0))) {
	ink->points[ink->length - 1].x += dx;
	goto ADJUST;
      }
    }
*/

    if (ink->length >= ink->maxlength) {
      ink->maxlength += POINT_BATCH;
      ink->points = (POINT *)realloc((void *)ink->points,
				      ink->maxlength * sizeof(POINT));
    }
    ink->points[ink->length].x = dx;
    ink->points[ink->length].y = dy;
    ink->length++;

  ADJUST:
    if (x < ink->left)
      ink->left = x;
    if (x > ink->right)
      ink->right = x;
    if (y < ink->top)
      ink->top = y;
    if (y > ink->bottom)
      ink->bottom = y;

    { int left, right, top, bottom;
      SimView *view;

      if (ink->last_x < x) { left = ink->last_x; right = x; }
      else { left = x; right = ink->last_x; }
      if (ink->last_y < y) { top = ink->last_y; bottom = y; }
      else { top = y; bottom = ink->last_y; }

      left -= 5; right += 5; top -= 5; bottom += 5;

      for (view = sim->editor; view != NULL; view = view->next) {
	int vleft, vtop;

	if ((right >= (vleft = view->pan_x - (view->w_width / 2))) &&
	    (left <= vleft + view->w_width) &&
	    (bottom >= (vtop = view->pan_y - (view->w_height / 2))) &&
	    (top <= vtop + view->w_height)) {
	  /* XXX: do studly incremental update instead */
	  view->overlay_mode = 0;
	  // currently empty fn EventuallyRedrawView(view);
	}
      }
    }
    ink->last_x = x; ink->last_y = y;
  }
}


