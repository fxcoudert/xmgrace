/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2000 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 *
 * set defaults - changes to the types in defines.h
 * will require changes in here also
 *
 */

#include <config.h>
#include <cmath.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "globals.h"
#include "graphs.h"
#include "utils.h"
#include "protos.h"

static defaults d_d =
{1, 0, 1, 1, 1, 1.0, 0, 1.0};

/* defaults layout
    int color;
    int bgcolor;
    int pattern;
    int lines;
    double linew;
    double charsize;
    int font;
    double symsize;
*/

static world d_w =
{0.0, 1.0, 0.0, 1.0};

static view d_v =
{0.15, 0.85, 0.15, 0.85};

void set_program_defaults(void)
{
    int i;
    
    grdefaults = d_d;
        
    for (i = 0; i < MAXREGION; i++) {
        set_region_defaults(i);
    }
    
    set_default_string(&timestamp);
    
    timestamp.x = 0.03;
    timestamp.y = 0.03;
    
    target_set.gno = -1;
    target_set.setno = -1;
}

void set_region_defaults(int rno)
{
    rg[rno].active = FALSE;
    rg[rno].type = 0;
    rg[rno].color = grdefaults.color;
    rg[rno].lines = grdefaults.lines;
    rg[rno].linew = grdefaults.linew;
    rg[rno].n = 0;
    rg[rno].x = rg[rno].y = NULL;
    rg[rno].x1 = rg[rno].y1 = rg[rno].x2 = rg[rno].y2 = 0.0;

    rg[rno].linkto = 0;
}

void set_default_framep(framep * f)
{
    f->type = 0;                /* frame type */
    f->lines = grdefaults.lines;
    f->linew = grdefaults.linew;
    f->pen.color = grdefaults.color;
    f->pen.pattern = grdefaults.pattern;
    f->fillpen.color = grdefaults.bgcolor;      /* fill background */
    f->fillpen.pattern = 0;
}

void set_default_world(world * w)
{
    memcpy(w, &d_w, sizeof(world));
}

void set_default_view(view * v)
{
    memcpy(v, &d_v, sizeof(view));
}

void set_default_string(plotstr * s)
{
    s->active = FALSE;
    s->loctype = COORD_VIEW;
    s->gno = -1;
    s->x = s->y = 0.0;
    s->color = grdefaults.color;
    s->rot = 0;
    s->font = grdefaults.font;
    s->just = JUST_LEFT|JUST_BLINE;
    s->charsize = grdefaults.charsize;
    s->s = NULL;
}

void set_default_arrow(Arrow *arrowp)
{
    arrowp->type = line_atype;
    arrowp->length = line_asize;
    arrowp->dL_ff = line_a_dL_ff;
    arrowp->lL_ff = line_a_lL_ff;
}

void set_default_line(linetype * l)
{
    l->active = FALSE;
    l->loctype = COORD_VIEW;
    l->gno = -1;
    l->x1 = l->y1 = l->x2 = l->y2 = 0.0;
    l->lines = grdefaults.lines;
    l->linew = grdefaults.linew;
    l->color = grdefaults.color;
    l->arrow_end = 0;
    set_default_arrow(&l->arrow);
}

void set_default_box(boxtype * b)
{
    b->active = FALSE;
    b->loctype = COORD_VIEW;
    b->gno = -1;
    b->x1 = b->y1 = b->x2 = b->y2 = 0.0;
    b->lines = grdefaults.lines;
    b->linew = grdefaults.linew;
    b->color = grdefaults.color;
    b->fillcolor = grdefaults.color;
    b->fillpattern = grdefaults.pattern;
}

void set_default_ellipse(ellipsetype * e)
{
    e->active = FALSE;
    e->loctype = COORD_VIEW;
    e->gno = -1;
    e->x1 = e->y1 = e->x2 = e->y2 = 0.0;
    e->lines = grdefaults.lines;
    e->linew = grdefaults.linew;
    e->color = grdefaults.color;
    e->fillcolor = grdefaults.color;
    e->fillpattern = grdefaults.pattern;
}

void set_default_legend(int gno, legend * l)
{
    l->active = TRUE;
    l->loctype = COORD_VIEW;
    l->vgap = 1;
    l->hgap = 1;
    l->len = 4;
    l->invert = FALSE;
    l->legx = 0.5;
    l->legy = 0.8;
    l->font = grdefaults.font;
    l->charsize = grdefaults.charsize;
    l->color = grdefaults.color;
    l->boxpen.color = grdefaults.color;
    l->boxpen.pattern = grdefaults.pattern;
    l->boxfillpen.color = 0;
    l->boxfillpen.pattern = grdefaults.pattern;
    l->boxlinew = grdefaults.linew;
    l->boxlines = grdefaults.lines;
    l->bb.xv1 = l->bb.xv2 = l->bb.yv1 = l->bb.yv2 = 0.0;
}

void set_default_plotarr(plotarr * p)
{
    int i;
    p->hidden = FALSE;                          /* hidden set */
    p->type = SET_XY;                           /* dataset type */
    p->hotlink = FALSE;                         /* hot linked set */
    p->hotfile[0] = '\0';                       /* hot linked file name */

    p->sym = 0;                                 /* set plot symbol */
    p->symlines = grdefaults.lines;             /* set plot sym line style */
    p->symsize = grdefaults.symsize;            /* size of symbols */
    p->symlinew = grdefaults.linew;             /* set plot sym line width */
    p->sympen.color = grdefaults.color;         /* color for symbol line */
    p->sympen.pattern = grdefaults.pattern;     /* pattern */
    p->symfillpen.color = grdefaults.color;     /* color for symbol fill */
    p->symfillpen.pattern = 0;                  /* pattern for symbol fill */

    p->symchar = 'A';
    p->charfont = grdefaults.font;

    p->symskip = 0;                             /* How many symbols to skip */

    p->avalue.active = FALSE;                   /* active or not */
    p->avalue.type = AVALUE_TYPE_Y;             /* type */
    p->avalue.size = 1.0;                       /* char size */
    p->avalue.font = grdefaults.font;           /* font */
    p->avalue.color = grdefaults.color;         /* color */
    p->avalue.angle = 0;                        /* rotation angle */
    p->avalue.format = FORMAT_GENERAL;          /* format */
    p->avalue.prec = 3;                         /* precision */
    p->avalue.prestr[0] = '\0';
    p->avalue.appstr[0] = '\0';
    p->avalue.offset.x = 0.0;
    p->avalue.offset.y = 0.0;

    p->linet = LINE_TYPE_STRAIGHT;
    p->lines = grdefaults.lines;
    p->linew = grdefaults.linew;
    p->linepen.color = grdefaults.color;
    p->linepen.pattern = grdefaults.pattern;
    
    p->baseline_type = BASELINE_TYPE_0;
    p->baseline = FALSE;
    p->dropline = FALSE;

    p->filltype = SETFILL_NONE;                 /* fill type */
    p->fillrule = FILLRULE_WINDING;             /* fill type */
    p->setfillpen.color = grdefaults.color;     /* fill color */
    p->setfillpen.pattern = grdefaults.pattern; /* fill pattern */

    p->errbar.active = TRUE;                      /* on by default */
    p->errbar.ptype = PLACEMENT_BOTH;             /* error bar placement */
    p->errbar.pen.color = grdefaults.color;       /* color */
    p->errbar.pen.pattern = grdefaults.pattern;   /* pattern */
    p->errbar.lines = grdefaults.lines;           /* error bar line width */
    p->errbar.linew = grdefaults.linew;           /* error bar line style */
    p->errbar.riser_linew = grdefaults.linew;     /* riser line width */
    p->errbar.riser_lines = grdefaults.lines;     /* riser line style */
    p->errbar.barsize = 1.0;                      /* size of error bar */
    p->errbar.arrow_clip = FALSE;                 /* draw arrows if clipped */
    p->errbar.cliplen = 0.1;                      /* max v.p. riser length */

    p->comments[0] = 0;                           /* how did this set originate */
    p->lstr[0] = 0;                               /* legend string */

    p->data.len = 0;                              /* dataset length */
    for (i = 0; i < MAX_SET_COLS; i++) {
        p->data.ex[i] = NULL;
    }
    p->data.s = NULL;                   /* pointer to strings */
}


void set_default_ticks(tickmarks *t)
{
    int i;

    if (t == NULL) {
        return;
    }
    
    t->active = TRUE;
    t->zero = FALSE;
    t->tl_flag = TRUE;
    t->t_flag = TRUE;
    set_default_string(&t->label);
    t->label.x = 0.0;
    t->label.y = 0.08;
    t->tmajor = 0.2;
    t->nminor = 1;
    t->t_round = TRUE;
    t->offsx = 0.0;
    t->offsy = 0.0;
    t->label_layout = LAYOUT_PARALLEL;
    t->label_place = TYPE_AUTO;
    t->label_op = PLACEMENT_NORMAL;
    t->tl_format = FORMAT_GENERAL;
    t->tl_prec = 5;
    t->tl_formula = NULL;
    t->tl_angle = 0;
    t->tl_skip = 0;
    t->tl_staggered = 0;
    t->tl_starttype = TYPE_AUTO;
    t->tl_stoptype = TYPE_AUTO;
    t->tl_start = 0.0;
    t->tl_stop = 0.0;
    t->tl_op = PLACEMENT_NORMAL;
    t->tl_gaptype = TYPE_AUTO;
    t->tl_gap.x = 0.0;
    t->tl_gap.y = 0.01;
    t->tl_font = grdefaults.font;
    t->tl_charsize = grdefaults.charsize;
    t->tl_color = grdefaults.color;
    t->tl_appstr[0] = 0;
    t->tl_prestr[0] = 0;
    t->t_spec = TICKS_SPEC_NONE;
    t->t_autonum = 6;
    t->t_inout = TICKS_IN;
    t->t_op = PLACEMENT_BOTH;
    t->props.size = grdefaults.charsize;
    t->mprops.size = grdefaults.charsize / 2;
    t->t_drawbar = TRUE;
    t->t_drawbarcolor = grdefaults.color;
    t->t_drawbarlines = grdefaults.lines;
    t->t_drawbarlinew = grdefaults.linew;
    t->props.gridflag = FALSE;
    t->mprops.gridflag = FALSE;
    t->props.color = grdefaults.color;
    t->props.lines = grdefaults.lines;
    t->props.linew = grdefaults.linew;
    t->mprops.color = grdefaults.color;
    t->mprops.lines = grdefaults.lines;
    t->mprops.linew = grdefaults.linew;
    t->nticks = 0;
    for (i = 0; i < MAX_TICKS; i++) {
        t->tloc[i].wtpos = 0.0;
        t->tloc[i].label = NULL;
    }
}
