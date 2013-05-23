/*****************************************************
 * Groufix  :  Graphics Engine produced by Ckef Worx *
 * www      :  http://www.ejb.ckef-worx.com          *
 *                                                   *
 * Copyright (C) Stef Velzel :: All Rights Reserved  *
 *****************************************************/

#ifndef GFX_UTILS_H
#define GFX_UTILS_H

// Concatenation
#define CAT_BAD(x,y) x ## y
#define CAT(x,y) CAT_BAD(x,y)

// Template naming
#define NAME_BAD(x,y) x ## _ ## y
#define NAME(x,y) NAME_BAD(x,y)

#endif // GFX_UTILS_H
