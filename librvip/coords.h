#if !defined(COORDS_H_JO935R2N)
#define COORDS_H_JO935R2N

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>

struct coords {
	coords() {}
	coords(float x, float y, float z, float rox, float roy, float roz) : tx(x), ty(y), tz(z), rx(rox), ry(roy), rz(roz) {}
	OSG::Vec3f t() { return OSG::Vec3f(tx, ty, tz); }
	OSG::Vec3f r() { return OSG::Vec3f(rx, ry, rz); }
	float tx, ty, tz;
	float rx, ry, rz;
};

#endif /* end of include guard: COORDS_H_JO935R2N */
