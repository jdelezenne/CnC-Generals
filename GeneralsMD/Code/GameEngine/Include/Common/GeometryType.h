#pragma once

enum GeometryType
{
	GEOMETRY_SPHERE = 0,	///< partition/collision testing as sphere. (majorRadius = radius)
	GEOMETRY_CYLINDER,		///< partition/collision testing as cylinder. (majorRadius = radius, height = height)
	GEOMETRY_BOX,					///< partition/collision testing as rectangular box (majorRadius = half len in forward dir; minorRadius = half len in side dir; height = height)

	GEOMETRY_NUM_TYPES,  // keep this last
	GEOMETRY_FIRST = GEOMETRY_SPHERE
};
