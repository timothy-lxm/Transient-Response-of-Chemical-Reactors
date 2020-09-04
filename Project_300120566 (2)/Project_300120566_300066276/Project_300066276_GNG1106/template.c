/*------------------------------------------------------------------
File: template.c
Description: Template that demonstrates creating a plot.
---------------------------------------------------------------------*/
#include <stdio.h>
#include <gng1106plplot.h>  // provides definitions for using PLplot library
#include <math.h>
#include <float.h>

// Some definitions
#define NUM_POINTS 100   // Number of points used for plotting
#define XINC 0.1         // for incrementing x when computing points
#define X_IX 0           // Row index for storing x values
#define FX_IX 1          // Row index for storing f(x) values

// function prototypes
double calcFx(double);
void plot(int numPoints, double points[][numPoints]);
double getMinDouble(double [], int);
double getMaxDouble(double [], int);
/*---------------------------------------------------------------------
Function: main
Description: This function computes a set of points for plotting.   The
             function calcFx is called to computes the value of f(x) .
	     Modify calcFx to compute the values of f(x) desired.
	     In this template, the sin function is used.
------------------------------------------------------------------------*/
void main(void)
{
    // Variable declarations
    double points[2][NUM_POINTS]; // row 0 contains values of x
                                  // row 1 contains values of f(x)
    int ix;  // for indexing points, i.e. columns of 2D array
    double x; // for incrementing the value of x
    // Setup loop that computes the points and stores in 2D array
    // A determinant loop is used to increment x, calculate f(x)
    // and store the values in the 2D array.
    x = 0.0;
    for(ix = 0; ix < NUM_POINTS; ix = ix + 1)
    {
       points[X_IX][ix] = x;           // Save x value
       points[FX_IX][ix] = calcFx(x);  // Calculate and save f(x) value
       x = x + XINC;                   // update x value for next iteration
    }
    // Call plot function to plot
    plot(NUM_POINTS, points);
}

/*-----------------------------------------------------------------------
Function: calcFx
Parameters:
    x - the value of x
Return:  The value of f(x)
Description:  Calculates the value of f(x).  In this case f(x) is the
              sin function, that is returns the value of sin(x).
------------------------------------------------------------------------*/
double calcFx(double x)
{
    // Variable declations
    double fx;
    // Instructions
    fx = sin(x);
    return(fx);
}

/*-------------------------------------------------
 Function: plot()
 Parameters:
    numPoints: number of points in the array, i.e. number of columns
    points: reference to 2D array
 Return value: none.
 Description: Initializes the plot.  The following values
              in the referenced structure are used to setup
              the plot:
	         points[X_IX][0], points[X_IX][nbrPoints-1]   range of horizontal axis
                 minFx, maxFx  - vertical axis range
	      Note the that the values of minFx and maxFx are determined with
	      the functions getMinDouble and getMaxDouble.  The values in the row
	      X_IX are assumed to be sorted in increasing order.
              Then plots the curve accessed using the contents of each row,
	      that is,  points[X_IX] and points[FX_IX] which are both references
	      to 1D arrays.
-------------------------------------------------*/
void plot(int numPoints, double points[][numPoints])
{
    // Variable declaration
    double minFx, maxFx;  // Minimum and maximum values of f(x)
    // Setup plot configuration
    plsdev("wingcc");  // Sets device to wingcc - CodeBlocks compiler
    // Initialize the plot
    plinit();
    // Configure the axis and labels
    plwidth(3);          // select the width of the pen
    minFx = getMinDouble(points[FX_IX], numPoints);
    maxFx = getMaxDouble(points[FX_IX], numPoints);
    plenv(points[X_IX][0],points[X_IX][numPoints-1],
	      minFx, maxFx, 0, 0);
    plcol0(GREEN);           // Select color for labels
    pllab("x", "f(x)", "Plot of f(x) versus x");
        // Plot the function.
    plcol0(BLUE);    // Color for plotting curve
    plline(numPoints, points[X_IX], points[FX_IX]);
    plend();
}

/*-------------------------------------------------
 Function: getMinDouble
 Parameters:
    arr: reference to array of double values
    n: number of elements in the array
 Return value: the smallest value found in the array.
 Description: Finds the smallest value in the array.
              Uses a determinate loop to traverse the array
	      to test each value in the array.
-------------------------------------------------*/
double getMinDouble(double arr[], int n)
{
   // Variable declarations
   double min;  // for storing minimum value
   int ix;      // indexing into an array
   // Instructions
   min = DBL_MAX;  // most positive value for type double
   for(ix = 0; ix < n; ix = ix + 1)
   {
       if(min > arr[ix]) min = arr[ix];
   }
   return(min);
}

/*------------------------- ------------------------
 Function: getMaxDouble
 Parameters:
    arr: reference to array of double values
    n: number of elements in the array
 Return value: the largest value found in the array.
 Description: Finds the largest value in the array.
              Uses a determinate loop to traverse the array
	      to test each value in the array.
-------------------------------------------------*/
double getMaxDouble(double arr[], int n)
{
   // Variable declarations
   double max;  // for storing maximum value
   int ix;      // indexing into an array
   // Instructions
   max = -DBL_MAX;  // most negative value for type double
   for(ix = 0; ix < n; ix = ix + 1)
   {
       if(max < arr[ix]) max = arr[ix];
   }
   return(max);
}


