﻿/**
 * \file mlt_field.h
 * \brief a field for planting multiple transitions and services
 * \see mlt_field_s
 *
 * Copyright (C) 2003-2014 Meltytech, LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MLT_FIELD_H
#define MLT_FIELD_H
#include "mlt_defines.h"
#include "mlt_types.h"

extern MLT_DECLSPEC mlt_field mlt_field_init( );
extern MLT_DECLSPEC mlt_field mlt_field_new( mlt_multitrack multitrack, mlt_tractor tractor );
extern MLT_DECLSPEC mlt_service mlt_field_service( mlt_field self );
extern MLT_DECLSPEC mlt_tractor mlt_field_tractor( mlt_field self );
extern MLT_DECLSPEC mlt_multitrack mlt_field_multitrack( mlt_field self );
extern MLT_DECLSPEC mlt_properties mlt_field_properties( mlt_field self );
extern MLT_DECLSPEC int mlt_field_plant_filter( mlt_field self, mlt_filter that, int track );
extern MLT_DECLSPEC int mlt_field_plant_transition( mlt_field self, mlt_transition that, int a_track, int b_track );
extern MLT_DECLSPEC void mlt_field_close( mlt_field self );
extern MLT_DECLSPEC void mlt_field_disconnect_service( mlt_field self, mlt_service service );

#endif

