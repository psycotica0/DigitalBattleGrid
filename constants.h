#ifndef M_PI
#define M_PI 3.14159265
#endif

#define DegToRad M_PI/180

#define tileSize 4.5f
#define edgeSize 0.5f

/* Currently my GL Units are in feet, so no conversion is required */
#define feetToUnits(feet) feet

/* Current my squares are 5 feet */
#define squaresToUnits(squares) feetToUnits(squares * 5)
