#include "stdafx.h"

// the precompiled directive above most likely means
// that many of the includes below are no longer required
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <rose.h>
#include <expx/expx.h>

#include "PopulateBaseCircle.h"
#include "../OpenGL/MatrixUtilities.h"

#if 0
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

RoseDesign *find_schema_binding(char * schema_instance);

/* This routine takes two input points (2 dimensional) and a included
angle between the points. This defines an arc where the direction of rotation
of the arc is counterclockwise.  If a negative angle is input, then the direction of rotation is clockwise.
It will compute the center and radius for this input */


extern SchemaMap * sm;			// current schema map, one of above

#define MTH_ZERO_TOL  1.0e-13
#define mth_GetSign(a,b)        ( (b) < 0 ? -(a) : (a) )

/*-----------------------------------------------------*/
double GetVectorMag (int ndim, double *a)
{

register int i;
register double mag;

mag = 0.0;

for (i = 0; i < ndim; i++) mag += a[i] * a[i];

return (sqrt(mag));
}

/*-----------------------------------------------------*/
int UnitizeVector (int ndim, double *a, double *r)
{
register double mag;
register int ii, num_zero, unit_mag_indx;

mag = GetVectorMag (ndim, a);

if (fabs(mag) > MTH_ZERO_TOL) {
for (ii = 0; ii < ndim; ii++) r[ii] = a[ii] / mag;
for (ii = num_zero = 0; ii < ndim; ii++) {
if (fabs(r[ii]) < MTH_ZERO_TOL) {
num_zero++;
} else {
unit_mag_indx = ii;
}
}
/*
If all but one of the components are effectively 0.0, then force
them to 0.0 and the absolute value of the nonzero component to 1.0
*/

if (num_zero == (ndim - 1)) {
r[unit_mag_indx] = mth_GetSign (1.0, r[unit_mag_indx]);
for (ii = 0; ii < ndim; ii++)if (ii != unit_mag_indx) r[ii] = 0.0;
}
return (0);
} else {

/*
If the magnitude of the vector is effectively zero, then return error and pass back the components that were passed in.
*/
for (ii = 0; ii < ndim; ii++) r[ii] = a[ii];
return (1);
}
}


/*-----------------------------------------------------*/
void CrossVector (double a[3], double b[3], double r[3])
{

r[0] = a[1]*b[2] - b[1]*a[2];
r[1] = a[2]*b[0] - b[2]*a[0];
r[2] = a[0]*b[1] - b[0]*a[1];

return;
}

/*-----------------------------------------------------*/

double pythagorean_theorem(double a, double b)
{
return (sqrt ( (a*a) + (b*b) ));
}

/*-----------------------------------------------------*/

void
PopulateBaseCircle()
{
  // Retrieve the trimming coordinates
  RoseObject * cp1 = sm->fetchObject(0);  
  RoseObject * cp2 = sm->fetchObject(1);

  // Retrieve the included angle
  double angle = sm->fetchDouble(2);

  // Retrieve the schema in which to create the Basis Circle
  char * schema_instance = sm->fetchString(3);

  // Create a place holder for the Basis Circle
  RoseObject * circle = NULL;

  // Locate the design related to the retrieved schema name
  RoseDesign * d = find_schema_binding(schema_instance);
  if (!d)
  {
    fprintf(stderr, "\nPopulateBasisCircle: '%s' not a valid schema instance!\n", 
	    schema_instance);

    sm->returnObject(circle);
	return;
  }

  double p1[3];
  double p2[3];
  double p3[3];
  double a;
  double b;
  double radius;
  double z[3];
  double line[3];
  double offset[3];
  double unit_offset[3];
  double center[3];

  // Extract the coordinates of the first trimming point
  RoseObject * coords = cp1->getObject( "coordinates" );
  p1[0] = coords->getDouble();
  p1[1] = coords->getDouble( 1 );
  // Add code to determine the correct dimensionality
  p1[2] = 0.0;

  // Extract the coordinates of the second trimming point
  coords = cp2->getObject( "coordinates" );
  p2[0] = coords->getDouble();
  p2[1] = coords->getDouble( 1 );
  // Add code to determine the correct dimensionality
  p2[2] = 0.0;

//  printf( "\nEndpoints and bulge angle: %lf, %lf to %lf, %lf thru %lf degrees\n",
//	p1[0], p1[1], p2[0], p2[1], angle );

  /*Convert to radians */
//  angle = angle * (3.14159265358979323846/180.0);
  angle = TORAD(angle);


  /* find the midpoint of a line connecting p1 and p2 */
  /* a vector drawn from p1 to p2 will be perpendicular to a vector drawn
  * between the arc center and the midpoint of line p1p2 */
  p3[0] = p1[0] + ((p2[0] - p1[0])/2.0);
  p3[1] = p1[1] + ((p2[1] - p1[1])/2.0);
//printf("Midpoint of (p1,p2) = P3 is  %lf, %lf\n", p3[0], p3[1]);


  /* Get the distance between P1 and P3 */
  /* This can actually be done using left shift, but I did it this way for
  * clarity */
  a =  pythagorean_theorem ( (p3[0] - p1[0]), (p3[1] - p1[1]) );
//printf("a = %lf\n", a);

  /* we now have a right triangle with the length of 1 known side (P1P3) and * its complimentary angle (input angle/2). We can use the trig functions
  * to ascertain the length of the other sides.  The hypotenuse of the
  * triangle is the radius of the arc. */

  b = a/tan(angle/2.0);
//printf("b = %lf\n", b);

  radius  =  pythagorean_theorem (a, b);
//printf("radius = %lf\n", radius);


  /* Compute the arc center. This is done by crossing the Z vector into
  * the vector which falls on the line between p1 and P2.
  * A cross product returns a vector which is normal to the two input vectors
  */

  /* Get the vector from p1 to p2 */
  line[0] = p2[0] - p1[0];
  line[1] = p2[1] - p1[1];
  line[2] = 0.0;

  /* Get the Z vector based upon the input angle */
  z[0] = 0.0;
  z[1] = 0.0;
  z[2] = 1.0;

  CrossVector(z, line,offset);


  /* Unitize the offset vector */
  UnitizeVector(3,offset, unit_offset);

  circle = d->pnewInstance("circle");
  if (!circle)
  {
    fprintf(stderr,
	    "\nPopulateBasisCircle: Unable to create new CIRCLE instance\n" );
    exit(1);
  }
  circle->putString( "", "name" );
  circle->putDouble( radius, "radius" );

  RoseObject * a2p2d = d->pnewInstance("axis2_placement_2d");
  a2p2d->putString( "", "name" );

  RoseObject * pos = d->pnewInstance("axis2_placement");
  pos->putObject( a2p2d, "_axis2_placement_2d" );
  circle->putObject( pos, "position" );

  RoseObject * loc = d->pnewInstance("cartesian_point");
  loc->putString( "", "name" );
  ListOfDouble * agg = new ListOfDouble();

  /* Calculate the center point */
  center[0] = p3[0] + b * unit_offset[0];
  center[1] = p3[1] + b * unit_offset[1];

//  printf( "Centerpoint and radius: %lf, %lf   %lf\n", center[0], center[1], radius );

  agg->add( center[0] );
  agg->add( center[1] );

  loc->putObject( agg, "coordinates" );

  a2p2d->putObject( loc, "location" );

  sm->returnObject(circle);
}


// Should create a class for this since we will need to go back and forth.
// angle in radians.
// PopulateBaseCircle above should call this.
void
CalculateBaseCircle(double x1, double y1, double x2, double y2, double angle,
                    double *xc, double *yc, double *radius){
  double p1[3];
  double p2[3];
  double p3[3];
  double a;
  double b;
  double z[3];
  double line[3];
  double offset[3];
  double unit_offset[3];

  // Extract the coordinates of the first trimming point
  p1[0] = x1;
  p1[1] = y1;
  // Add code to determine the correct dimensionality
  p1[2] = 0.0;

  // Extract the coordinates of the second trimming point
  p2[0] = x2;
  p2[1] = y2;
  // Add code to determine the correct dimensionality
  p2[2] = 0.0;

  /* find the midpoint of a line connecting p1 and p2 */
  /* a vector drawn from p1 to p2 will be perpendicular to a vector drawn
  * between the arc center and the midpoint of line p1p2 */
  p3[0] = p1[0] + ((p2[0] - p1[0])/2.0);
  p3[1] = p1[1] + ((p2[1] - p1[1])/2.0);

  /* Get the distance between P1 and P3 */
  /* This can actually be done using left shift, but I did it this way for
  * clarity */
  a =  pythagorean_theorem ( (p3[0] - p1[0]), (p3[1] - p1[1]) );

  /* we now have a right triangle with the length of 1 known side (P1P3) and * its complimentary angle (input angle/2). We can use the trig functions
  * to ascertain the length of the other sides.  The hypotenuse of the
  * triangle is the radius of the arc. */

  b = a/tan(angle/2.0);

  *radius  =  pythagorean_theorem (a, b);

  /* Compute the arc center. This is done by crossing the Z vector into
  * the vector which falls on the line between p1 and P2.
  * A cross product returns a vector which is normal to the two input vectors
  */

  /* Get the vector from p1 to p2 */
  line[0] = p2[0] - p1[0];
  line[1] = p2[1] - p1[1];
  line[2] = 0.0;

  /* Get the Z vector based upon the input angle */
  z[0] = 0.0;
  z[1] = 0.0;
  z[2] = 1.0;

  CrossVector(z, line,offset);


  /* Unitize the offset vector */
  UnitizeVector(3,offset, unit_offset);

  /* Calculate the center point */
  *xc = p3[0] + b * unit_offset[0];
  *yc = p3[1] + b * unit_offset[1];
   }
