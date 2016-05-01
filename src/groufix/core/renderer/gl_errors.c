/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel.
 *
 * Groufix is licensed under the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the license,
 * or (at your option) any later version.
 *
 */

#define GL_GLEXT_PROTOTYPES
#include "groufix/core/internal.h"

#include <stdlib.h>

/******************************************************/
static void APIENTRY _gfx_gl_error_callback(

		GLenum         source,
		GLenum         type,
		GLuint         id,
		GLenum         severity,
		GLsizei        length,
		const GLchar*  message,
		const void*    userParam)
{
	GFXErrorCode code;
	switch(type)
	{
		/* Ignore group and marker calls */
		case GL_DEBUG_TYPE_MARKER :
		case GL_DEBUG_TYPE_PUSH_GROUP :
		case GL_DEBUG_TYPE_POP_GROUP :
			return;

		/* Unknown errors */
		case GL_DEBUG_TYPE_ERROR :
		case GL_DEBUG_TYPE_OTHER :
			code = GFX_ERROR_UNKNOWN;
			break;

		/* Known errors */
		default :
			code = type;
			break;
	}

	/* Unformat the error */
	char* form = _gfx_unformat_string(message);
	if(form) gfx_errors_push(code, form);

	free(form);
}

/******************************************************/
void _gfx_renderer_init_errors(

		GFX_CONT_ARG)
{
	GFX_REND_GET.DebugMessageCallback(
		_gfx_gl_error_callback, NULL);
	GFX_REND_GET.DebugMessageControl(
		GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

/******************************************************/
void _gfx_renderer_poll_errors(

		GFX_CONT_ARG)
{
	/* Loop over all errors */
	GLenum err = glGetError();
	while(err != GL_NO_ERROR)
	{
		gfx_errors_push(err, "An OpenGL error occurred.");
		err = glGetError();
	}
}
