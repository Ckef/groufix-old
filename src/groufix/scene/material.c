/**
 * Groufix  :  Graphics Engine produced by Ckef Worx.
 * www      :  <http://www.ckef-worx.com>.
 *
 * This file is part of Groufix.
 *
 * Copyright (C) Stef Velzel :: All Rights Reserved.
 *
 * Groufix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Groufix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Groufix.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "groufix/scene.h"
#include "groufix/scene/internal.h"

#include <stdlib.h>

/******************************************************/
/* Internal material */
struct GFX_Material
{
	/* Super class */
	GFX_LodMap map;

	/* Hidden data */
	GFXVector  units;   /* Stores GFX_Unit */
	GFXVector  buckets; /* Stores (GFXPipe* + size_t (upper bound in units) * propertyMaps) */
};

/* Internal unit */
struct GFX_Unit
{
	size_t  src;  /* Sort key */
	size_t  unit; /* Actual bucket unit */
};

/******************************************************/
static inline size_t _gfx_material_get_num_maps(

		GFXLodMap* map)
{
	return gfx_lod_map_count(map, map->levels);
}

/******************************************************/
static inline size_t _gfx_material_get_map_index(

		GFXLodMap*  map,
		size_t      level,
		size_t      index)
{
	return gfx_lod_map_count(map, level) + index;
}

/******************************************************/
static inline size_t _gfx_material_get_bucket_size(

		struct GFX_Material* material)
{
	return sizeof(GFXPipe*) + sizeof(size_t) *
		_gfx_material_get_num_maps((GFXLodMap*)material);
}

/******************************************************/
static inline size_t* _gfx_material_bucket_at(

		GFXVectorIterator  bucket,
		size_t             index)
{
	return ((size_t*)(((GFXPipe**)bucket) + 1)) + index;
}

/******************************************************/
static void _gfx_material_get_boundaries(

		struct GFX_Material*  material,
		GFXVectorIterator     bucket,
		size_t                index,
		size_t*               begin,
		size_t*               end)
{
	size_t* it = _gfx_material_bucket_at(bucket, index);
	*end = *it;

	/* Get previous upper bound as begin */
	if(index) *begin = *(it - 1);

	/* Get start as begin */
	else if(bucket == material->buckets.begin) *begin = 0;

	/* Get upper bound of previous buffer as begin */
	else *begin = *(((size_t*)bucket) - 1);
}

/******************************************************/
static void _gfx_material_get_bucket_boundaries(

		struct GFX_Material*  material,
		GFXVectorIterator     bucket,
		size_t*               begin,
		size_t*               end)
{
	size_t upper = _gfx_material_get_num_maps((GFXLodMap*)material);
	*end = *_gfx_material_bucket_at(bucket, upper - 1);

	/* Get start as begin */
	if(bucket == material->buckets.begin) *begin = 0;

	/* Get upper bound of previous buffer as begin */
	else *begin = *(((size_t*)bucket) - 1);
}

/******************************************************/
static GFXVectorIterator _gfx_material_find_bucket(

		struct GFX_Material*  material,
		GFXPipe*              pipe)
{
	/* Iterate and find */
	size_t bucketSize = _gfx_material_get_bucket_size(material);
	GFXVectorIterator it;

	for(
		it = material->buckets.begin;
		it != material->buckets.end;
		it = gfx_vector_advance(&material->buckets, it, bucketSize))
	{
		if(*(GFXPipe**)it == pipe) break;
	}

	return it;
}

/******************************************************/
static void _gfx_material_increase_bounds(

		struct GFX_Material*  material,
		GFXVectorIterator     bucket,
		size_t                index,
		size_t                num,
		size_t                diff)
{
	size_t size = _gfx_material_get_bucket_size(material);
	size_t u;

	/* Adjust remaining maps of the current bucket */
	for(u = index; u < num; ++u)
		*_gfx_material_bucket_at(bucket, u) += diff;

	/* Adjust other buckets */
	for(
		bucket = gfx_vector_advance(&material->buckets, bucket, size);
		bucket != material->buckets.end;
		bucket = gfx_vector_advance(&material->buckets, bucket, size))
	{
		for(u = 0; u < num; ++u)
			*_gfx_material_bucket_at(bucket, u) += diff;
	}
}

/******************************************************/
static void _gfx_material_erase_units(

		struct GFX_Material*  material,
		GFXBucket*            bucket,
		size_t                begin,
		size_t                end)
{
	while(begin < end)
	{
		/* Erase from the bucket */
		GFXVectorIterator unit = gfx_vector_at(&material->units, begin++);
		gfx_bucket_erase(bucket, ((struct GFX_Unit*)unit)->unit);
	}
}

/******************************************************/
static void _gfx_material_unregister(

		struct GFX_Material*  material,
		GFXVectorIterator     bucket)
{
	/* Unregister material at pip */
	GFXPipeCallback call;
	call.key = GFX_SCENE_KEY_MATERIAL;
	call.data = material;

	gfx_pipe_unregister(*(GFXPipe**)bucket, call);

	/* Erase bucket from vector */
	bucket = gfx_vector_erase_range(
		&material->buckets,
		_gfx_material_get_bucket_size(material),
		bucket
	);
}

/******************************************************/
static void _gfx_material_erase_buckets(

		struct GFX_Material* material)
{
	/* Iterate over all buckets */
	size_t bucketSize = _gfx_material_get_bucket_size(material);
	size_t bucketIndex = gfx_vector_get_byte_size(&material->buckets);

	while(bucketIndex)
	{
		bucketIndex -= bucketSize;
		GFXVectorIterator bucket = gfx_vector_at(
			&material->buckets,
			bucketIndex);

		size_t begin;
		size_t end;
		_gfx_material_get_bucket_boundaries(
			material,
			bucket,
			&begin,
			&end
		);

		/* Erase all associated units */
		_gfx_material_erase_units(material, (*(GFXPipe**)bucket)->bucket, begin, end);

		/* Unregister material at bucket */
		_gfx_material_unregister(material, bucket);
	}

	gfx_vector_clear(&material->units);
	gfx_vector_clear(&material->buckets);
}

/******************************************************/
static void _gfx_material_callback(

		GFXPipe*          pipe,
		GFXPipeCallback*  callback)
{
	struct GFX_Material* mat = callback->data;

	/* Find bucket and erase it */
	GFXVectorIterator bucket = _gfx_material_find_bucket(mat, pipe);
	if(bucket != mat->buckets.end)
	{
		size_t begin;
		size_t end;
		_gfx_material_get_bucket_boundaries(
			mat,
			bucket,
			&begin,
			&end
		);

		size_t size = _gfx_material_get_bucket_size(mat);
		size_t numMaps = _gfx_material_get_num_maps((GFXLodMap*)mat);
		size_t diff = end - begin;

		/* Erase from vectors */
		gfx_vector_erase_range_at(&mat->units, diff, begin);
		bucket = gfx_vector_erase_range(&mat->buckets, size, bucket);

		/* Adjust upper bounds */
		_gfx_material_increase_bounds(
			mat,
			bucket,
			0,
			numMaps,
			-diff
		);
	}
}

/******************************************************/
static size_t _gfx_material_find_units(

		struct GFX_Material*  material,
		GFXVectorIterator     bucket,
		size_t                index,
		size_t                src,
		size_t*               num)
{
	size_t min;
	size_t max;
	_gfx_material_get_boundaries(
		material,
		bucket,
		index,
		&min,
		&max
	);

	/* Binary search for the source ID */
	size_t mid = min;
	while(max > min)
	{
		mid = min + ((max - min) >> 1);

		GFXVectorIterator it = gfx_vector_at(&material->units, mid);
		size_t found = ((struct GFX_Unit*)it)->src;

		/* Compare against key */
		if(found < src)
			min = ++mid;
		else if(found > src)
			max = mid;

		else break;
	}

	/* Binary search for the first equivalent source */
	size_t find = mid;
	while(find > min)
	{
		size_t quart = min + ((find - min) >> 1);

		GFXVectorIterator it = gfx_vector_at(&material->units, quart);
		size_t found = ((struct GFX_Unit*)it)->src;

		/* Compare against key */
		if(found < src) min = quart + 1;
		else find = quart;
	}

	/* Binary search for the last equivalent source */
	find = mid;
	while(max > find)
	{
		size_t quart = find + ((max - find) >> 1);

		GFXVectorIterator it = gfx_vector_at(&material->units, quart);
		size_t found = ((struct GFX_Unit*)it)->src;

		/* Compare against key (*/
		if(found > src) max = quart;
		else find = quart + 1;
	}

	/* Return the range */
	*num = max - min;
	return min;
}

/******************************************************/
size_t _gfx_material_add_bucket_unit(

		GFXMaterial*   material,
		size_t         level,
		size_t         index,
		GFXPipe*       pipe,
		size_t         src,
		GFXBatchState  state,
		int            visible)
{
	/* Validate pipe type */
	if(gfx_pipe_get_type(pipe) != GFX_PIPE_BUCKET) return 0;

	/* First get the property map */
	struct GFX_Material* internal = (struct GFX_Material*)material;

	size_t num;
	GFXPropertyMap** data = gfx_lod_map_get(
		(GFXLodMap*)material,
		level,
		&num);

	if(index >= num) return 0;

	/* Create the unit */
	struct GFX_Unit unit;
	unit.src = src;
	unit.unit = gfx_bucket_insert(
		pipe->bucket,
		src,
		data[index],
		state,
		visible
	);

	if(!unit.unit) return 0;

	/* Get bucket index of the map */
	size_t numMaps =
		_gfx_material_get_num_maps((GFXLodMap*)internal);
	size_t ind =
		_gfx_material_get_map_index((GFXLodMap*)internal, level, index);

	/* Also find the bucket while we're at it */
	GFXVectorIterator bucket = _gfx_material_find_bucket(internal, pipe);

	if(bucket == internal->buckets.end)
	{
		/* Try to insert a new bucket */
		size_t bucketSize = _gfx_material_get_bucket_size(internal);
		bucket = gfx_vector_insert_range(
			&internal->buckets,
			bucketSize,
			NULL,
			internal->buckets.end
		);

		if(bucket != internal->buckets.end)
		{
			/* Try to insert a new unit */
			GFXVectorIterator unitIt = gfx_vector_insert(
				&internal->units,
				&unit,
				internal->units.end
			);

			if(unitIt != internal->units.end)
			{
				/* Register material at pipe */
				GFXPipeCallback call;
				call.key = GFX_SCENE_KEY_MATERIAL;
				call.data = internal;

				if(gfx_pipe_register(pipe, call, _gfx_material_callback))
				{
					/* Initialize the bucket */
					*(GFXPipe**)bucket = pipe;
					size_t upper = gfx_vector_get_size(&internal->units);

					while(numMaps--)
					{
						size_t* set = _gfx_material_bucket_at(bucket, numMaps);
						*set = (numMaps >= ind) ? upper : upper - 1;
					}

					return unit.unit;
				}

				/* Failed, remove unit */
				gfx_vector_erase(&internal->units, unitIt);
			}

			/* Remove bucket */
			gfx_vector_erase_range(&internal->buckets, bucketSize, bucket);
		}

		/* Erase unit from bucket */
		gfx_bucket_erase(pipe->bucket, unit.unit);

		return 0;
	}

	/* Find the range of units with the given source */
	size_t units;
	size_t begin = _gfx_material_find_units(
		internal,
		bucket,
		ind,
		src,
		&units);

	/* Try to insert the unit */
	GFXVectorIterator unitIt = gfx_vector_insert_at(
		&internal->units,
		&unit,
		begin + units);

	if(unitIt == internal->units.end)
	{
		gfx_bucket_erase(pipe->bucket, unit.unit);
		return 0;
	}

	/* Increase bounds */
	_gfx_material_increase_bounds(
		internal,
		bucket,
		ind,
		numMaps,
		1
	);

	return unit.unit;
}

/******************************************************/
void* _gfx_material_get_bucket_units(

		GFXMaterial*   material,
		size_t         level,
		size_t         index,
		GFXPipe*       pipe,
		size_t         src,
		size_t*        num)
{
	struct GFX_Material* internal = (struct GFX_Material*)material;

	/* Get bucket index of the map */
	size_t numMaps =
		_gfx_material_get_num_maps((GFXLodMap*)internal);
	size_t ind =
		_gfx_material_get_map_index((GFXLodMap*)internal, level, index);

	/* Find the bucket */
	GFXVectorIterator bucket = _gfx_material_find_bucket(internal, pipe);

	/* Crude check if the index is within the maps */
	/* At least it will never segfault */
	if(ind >= numMaps || bucket == internal->buckets.end)
	{
		*num = 0;
		return NULL;
	}

	/* Find the range of units with the given source */
	size_t begin = _gfx_material_find_units(
		internal,
		bucket,
		ind,
		src,
		num
	);

	return gfx_vector_at(&internal->units, begin);
}

/******************************************************/
size_t _gfx_material_bucket_units_at(

		void*   array,
		size_t  unit)
{
	return ((struct GFX_Unit*)array)[unit].unit;
}

/******************************************************/
void _gfx_material_remove_bucket_units(

		GFXMaterial*  material,
		size_t        level,
		size_t        index,
		GFXPipe*      pipe,
		size_t        src,
		size_t        unit,
		size_t        num)
{
	struct GFX_Material* internal = (struct GFX_Material*)material;

	/* Get bucket index of the map */
	size_t numMaps =
		_gfx_material_get_num_maps((GFXLodMap*)internal);
	size_t ind =
		_gfx_material_get_map_index((GFXLodMap*)internal, level, index);

	/* Find the bucket */
	GFXVectorIterator bucket = _gfx_material_find_bucket(internal, pipe);

	/* Crude check if the index is within the maps */
	/* At least it will never segfault */
	if(ind < numMaps && bucket != internal->buckets.end)
	{
		/* Find the range of units with the given source */
		size_t units;
		size_t begin = _gfx_material_find_units(
			internal,
			bucket,
			ind,
			src,
			&units
		);

		begin += unit;
		units = (unit > units) ? 0 : units - unit;
		num = (num > units) ? units : num;

		if(num)
		{
			/* Erase the units */
			_gfx_material_erase_units(internal, pipe->bucket, begin, num);
			gfx_vector_erase_range_at(&internal->units, num, begin);

			/* Decrease bounds */
			_gfx_material_increase_bounds(
				internal,
				bucket,
				ind,
				numMaps,
				-num);

			/* Check if the bucket has any units left */
			_gfx_material_get_bucket_boundaries(
				internal,
				bucket,
				&begin,
				&num
			);

			/* Unregister if none are left */
			if(!(num - begin)) _gfx_material_unregister(internal, bucket);
		}
	}
}

/******************************************************/
GFXMaterial* gfx_material_create(void)
{
	/* Allocate material */
	struct GFX_Material* mat = calloc(1, sizeof(struct GFX_Material));
	if(!mat) return NULL;

	/* Initialize */
	_gfx_lod_map_init(
		(GFX_LodMap*)mat,
		sizeof(GFXPropertyMap*),
		sizeof(GFXPropertyMap*)
	);

	gfx_vector_init(&mat->units, sizeof(struct GFX_Unit));
	gfx_vector_init(&mat->buckets, 1);

	return (GFXMaterial*)mat;
}

/******************************************************/
void gfx_material_free(

		GFXMaterial* material)
{
	if(material)
	{
		struct GFX_Material* internal = (struct GFX_Material*)material;

		/* Erase all buckets */
		_gfx_material_erase_buckets(internal);

		/* Iterate over all levels */
		size_t levels = material->lodMap.levels;
		while(levels)
		{
			/* Free all property maps in it */
			size_t num;
			GFXPropertyMap** data = gfx_lod_map_get(
				(GFXLodMap*)material,
				--levels,
				&num
			);

			while(num) gfx_property_map_free(data[--num]);
		}

		_gfx_lod_map_clear((GFX_LodMap*)material);
		free(material);
	}
}

/******************************************************/
GFXPropertyMap* gfx_material_add(

		GFXMaterial*   material,
		size_t         level,
		GFXProgram*    program,
		unsigned char  properties)
{
	struct GFX_Material* internal = (struct GFX_Material*)material;

	/* Get data from before anything is inserted */
	size_t bucketSize = _gfx_material_get_bucket_size(internal);
	size_t ind = gfx_lod_map_count((GFXLodMap*)internal, level + 1);

	/* Create new property map and add it to the LOD map */
	GFXPropertyMap* map = gfx_property_map_create(program, properties);
	if(!map) return NULL;

	if(gfx_lod_map_add((GFXLodMap*)internal, level, &map))
	{
		/* Calculate new size and try to reserve it */
		size_t bucketIndex = gfx_vector_get_byte_size(&internal->buckets);
		size_t size = bucketIndex + (bucketIndex / bucketSize) * sizeof(size_t);

		if(gfx_vector_reserve(&internal->buckets, size))
		{
			/* Iterate over all buckets and insert it */
			while(bucketIndex)
			{
				bucketIndex -= bucketSize;
				GFXVectorIterator bucket = gfx_vector_at(
					&internal->buckets,
					bucketIndex);

				size_t* insert = _gfx_material_bucket_at(bucket, ind);

				gfx_vector_insert_range(
					&internal->buckets,
					sizeof(size_t),
					insert - 1,
					insert);
			}

			return map;
		}

		/* Nevermind */
		gfx_lod_map_remove((GFXLodMap*)internal, level, &map);
	}

	/* All failed D: */
	gfx_property_map_free(map);

	return NULL;
}

/******************************************************/
static int _gfx_material_remove_at(

		struct GFX_Material*  material,
		size_t                level,
		size_t                index)
{
	/* First get the property map */
	size_t num;
	GFXPropertyMap** data = gfx_lod_map_get(
		(GFXLodMap*)material,
		level,
		&num
	);

	if(index >= num) return 0;

	/* Check if any maps will be left */
	size_t numMaps = _gfx_material_get_num_maps((GFXLodMap*)material);
	size_t ind = _gfx_material_get_map_index((GFXLodMap*)material, level, index);

	if(numMaps > 1)
	{
		/* Iterate over all buckets */
		size_t bucketSize = _gfx_material_get_bucket_size(material);
		size_t bucketIndex = gfx_vector_get_byte_size(&material->buckets);

		while(bucketIndex)
		{
			bucketIndex -= bucketSize;
			GFXVectorIterator bucket = gfx_vector_at(
				&material->buckets,
				bucketIndex);

			size_t begin;
			size_t end;
			_gfx_material_get_boundaries(
				material,
				bucket,
				ind,
				&begin,
				&end
			);

			size_t diff = end - begin;

			/* Erase all associated units */
			_gfx_material_erase_units(material, (*(GFXPipe**)bucket)->bucket, begin, end);
			gfx_vector_erase_range_at(&material->units, diff, begin);

			/* Adjust upper bounds */
			_gfx_material_increase_bounds(
				material,
				bucket,
				ind,
				numMaps,
				-diff
			);
		}

		/* Iterate again and erase the maps */
		bucketIndex = gfx_vector_get_byte_size(&material->buckets);

		while(bucketIndex)
		{
			bucketIndex -= bucketSize;
			GFXVectorIterator bucket = gfx_vector_at(
				&material->buckets,
				bucketIndex);

			size_t begin;
			size_t end;
			_gfx_material_get_bucket_boundaries(
				material,
				bucket,
				&begin,
				&end
			);

			/* Unregister if no units are left */
			if(!(end - begin)) _gfx_material_unregister(material, bucket);

			/* Erase the map only */
			else gfx_vector_erase_range(
				&material->buckets,
				sizeof(size_t),
				_gfx_material_bucket_at(bucket, ind));
		}
	}

	/* Erase all buckets if necessary */
	else _gfx_material_erase_buckets(material);

	/* Free and remove it */
	gfx_property_map_free(data[index]);
	gfx_lod_map_remove_at((GFXLodMap*)material, level, index);

	return 1;
}

/******************************************************/
int gfx_material_remove(

		GFXMaterial*     material,
		GFXPropertyMap*  map)
{
	/* Find level it resides in */
	size_t levels = material->lodMap.levels;

	while(levels--)
	{
		size_t num;
		GFXPropertyMap** data = gfx_lod_map_get(
			(GFXLodMap*)material,
			levels,
			&num
		);

		size_t n;
		for(n = 0; n < num; ++n) if(data[n] == map)
		{
			/* Erase it if found */
			return _gfx_material_remove_at(
				(struct GFX_Material*)material,
				levels,
				n
			);
		}
	}

	return 0;
}

/******************************************************/
int gfx_material_remove_at(

		GFXMaterial*  material,
		size_t        level,
		size_t        index)
{
	return _gfx_material_remove_at(
		(struct GFX_Material*)material,
		level,
		index
	);
}

/******************************************************/
GFXPropertyMapList gfx_material_get(

		GFXMaterial*  material,
		size_t        level,
		size_t*       num)
{
	return gfx_lod_map_get((GFXLodMap*)material, level, num);
}

/******************************************************/
GFXPropertyMap* gfx_property_map_list_at(

		GFXPropertyMapList  list,
		size_t              index)
{
	return ((GFXPropertyMap**)list)[index];
}
