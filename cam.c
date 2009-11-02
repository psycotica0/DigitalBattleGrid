#include "cam.h"

void UseCam(Camera* cam) {
	/* Calculate All Angles */
	/* (Negate the angles becuase we are actually dealing with the object, not the cam) */
	float sinYaw = sin(DegToRad * -cam->yaw);
	float cosYaw = cos(DegToRad * -cam->yaw);
	float sinPitch = sin(DegToRad * -cam->pitch);
	float cosPitch = cos(DegToRad * -cam->pitch);
	float sinRoll = sin(DegToRad * -cam->roll);
	float cosRoll = cos(DegToRad * -cam->roll);

	camMatrix[0] = (cosYaw * cosRoll) + (sinYaw * sinPitch * sinRoll);
	camMatrix[1] = (cosYaw * sinRoll) - (sinYaw * sinPitch * cosRoll);
	camMatrix[2] = sinYaw * cosPitch;

	camMatrix[4] = -cosPitch * sinRoll;
	camMatrix[5] = cosPitch * cosRoll;
	camMatrix[6] = sinPitch;

	camMatrix[8] = (-sinYaw * cosRoll) + (cosYaw * sinPitch * sinRoll);
	camMatrix[9] = (-sinYaw * sinRoll) - (cosYaw * sinPitch * cosRoll);
	camMatrix[10] = cosYaw * cosPitch;

	/* Translate */
	/* In order to work normally it has to be translated with respect to the rotations we've performed above */
	camMatrix[12] = -1 * ((cam->x * camMatrix[0]) + (cam->y *camMatrix[4]) + (cam->z * camMatrix[8]));
	camMatrix[13] = -1 * ((cam->x * camMatrix[1]) + (cam->y *camMatrix[5]) + (cam->z * camMatrix[9]));
	camMatrix[14] = -1 * ((cam->x * camMatrix[2]) + (cam->y *camMatrix[6]) + (cam->z * camMatrix[10]));

	glLoadMatrixf(camMatrix);
}

