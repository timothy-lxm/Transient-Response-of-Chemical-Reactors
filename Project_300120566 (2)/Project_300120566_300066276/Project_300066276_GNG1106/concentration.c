/*------------------------------------------------------------------
File: concentration.c
GNG1106
Description: This is a project that calculates the Transient response
of coupled chemical reactors.
Regan Aubrey 300066276
Timothy Lam 300120566
December 8 2019

---------------------------------------------------------------------*/
#include <stdio.h>
#include <gng1106plplot.h>  // provides definitions for using PLplot library
#include <math.h>
#include <float.h>
#include <string.h>

// Some definitions
#define NUM_POINTS 100   // Number of points used for plotting
#define TIME_INITIAL 0
#define BINFILE "file.bin"
#define MAXRECORDS 5      //The max number of input records we will save in a file
#define TRUE 1
#define FALSE 0

typedef struct reactor_tag
{
    double v_1;
    double v_2;
    double v_3;
} REACTORS;

typedef struct concentration_tag
{
    double c_01;
    double c_03;
    double cr1[NUM_POINTS];
    double cr2[NUM_POINTS];
    double cr3[NUM_POINTS];
    double time_axis[NUM_POINTS];
    double time_final;
    double c1_0;
    double c2_0;
    double c3_0;
} CONCENTRATIONS;

typedef struct flow_rate_tag
{
    double Q_01;
    double Q_03;
    double Q_12;
    double Q_23;
    double Q_31;
    double Q_33;
} FLOW_RATES;

//This was defined to more efficiently save the user inputs to file
typedef struct user_input_tag
{
    double v1, v2, v3;
    double q01, q03, q12,q23, q31,q33;
    double c01, c03;
    double c10, c20, c30;
    double time_final;
} USER_INPUTS;

// function prototypes
void receiveUserInputs(REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr);
int testConstraints(FLOW_RATES *fPtr);
void calculateConcentrations(REACTORS *r, FLOW_RATES *f, CONCENTRATIONS *c);
void plotTable(CONCENTRATIONS *cPtr);
void storeFiles(REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr);
int retrieveFiles(REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr);
double getMinDouble(double [], int);
double getMaxDouble(double [], int);


/*---------------------------------------------------------------------
Function: main
Description: This function computes a set of points for plotting.
         It begins by gathering inputs from the user, then validates the
         inputs so the values comply with the equations, If the values are valid,
         they will be plotted on the output graph. The inputs can be saved to a file
         for future use. It will save up to 5 records.
------------------------------------------------------------------------*/
int main(void)
{
    REACTORS reactors;
    CONCENTRATIONS concentrations;
    FLOW_RATES flow_rates;

    if(!retrieveFiles(&reactors, &flow_rates,&concentrations))
    {
        //if there was no saved input chosen, aske the user for input
        do
        {
            receiveUserInputs(&reactors, &flow_rates,&concentrations);
        }
        while(testConstraints(&flow_rates)==FALSE);

        storeFiles(&reactors, &flow_rates,&concentrations);

    }

    calculateConcentrations(&reactors, &flow_rates,&concentrations);

    plotTable(&concentrations);

}

/*-----------------------------------------------------------------------
Function: receiveUserInputs
Parameters:
    REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr
Return:  void
Description:  requests the user inputs, initial contraints will be applied to ensure
            the project requested input constraints were verified.
            The values will be stored in the structures defined and declared in the
            calling function.
------------------------------------------------------------------------*/
void receiveUserInputs(REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr)
{
    int pass=TRUE;


    printf("Enter C01:\n");
    scanf("%lf",&cPtr->c_01);
    printf("Enter C03:\n");
    scanf("%lf",&cPtr->c_03);
    printf("Enter C10:\n");
    scanf("%lf",&cPtr->c1_0);
    printf("Enter C20:\n");
    scanf("%lf",&cPtr->c2_0);
    printf("Enter C30:\n");
    scanf("%lf",&cPtr->c3_0);
    do
    {
        printf("Enter the final time tf:\n");
        scanf("%lf",&cPtr->time_final);

        if(cPtr->time_final<=0)
        {
            printf("Sorry, time has to be greater than zero\n");
            pass = FALSE;
        }
        else
            pass=TRUE;
    }

    while (pass==FALSE);

    do
    {
        printf("Enter V1:\n");
        scanf("%lf",&rPtr->v_1);
        if(rPtr->v_1<=0)
        {
            printf("Sorry, V1 has to be greater than zero\n");
            pass = FALSE;
        }
        else
            pass=TRUE;
    }

    while (pass==FALSE);

    do
    {
        printf("Enter V2:\n");
        scanf("%lf",&rPtr->v_2);
        if(rPtr->v_2<=0)
        {
            printf("Sorry, V2 has to be greater than zero\n");
            pass = FALSE;
        }
        else
            pass= TRUE;
    }
    while (pass == FALSE);

    do
    {
        printf("Enter V3:\n");
        scanf("%lf",&rPtr->v_3);
        if(rPtr->v_3<=0)
        {
            printf("Sorry, V3 has to be greater than zero\n");
            pass = FALSE;
        }
        else
            pass = TRUE;
    }
    while (pass==FALSE);

    printf("Enter Q01:\n");
    scanf("%lf",&fPtr->Q_01);
    printf("Enter Q03:\n");
    scanf("%lf",&fPtr->Q_03);
    printf("Enter Q12:\n");
    scanf("%lf",&fPtr->Q_12);
    printf("Enter Q23:\n");
    scanf("%lf",&fPtr->Q_23);
    printf("Enter Q31:\n");
    scanf("%lf",&fPtr->Q_31);
    printf("Enter Q33:\n");
    scanf("%lf",&fPtr->Q_33);

    return;
}

/*-----------------------------------------------------------------------
Function: calculateConcentrations
Parameters:
    REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr
Return:  void
Description:  Based on the number of points and the time for the reaction,
            the x axis increments are calculated(delta t), then we step through
            each of the concentrations step by step because each concentration
            relies on the other concentrations.
------------------------------------------------------------------------*/
void calculateConcentrations(REACTORS *r, FLOW_RATES *f, CONCENTRATIONS *c)
{
    double inc;
    int ix;

    c->time_axis[0]=0;
    c->cr1[0]=c->c1_0; //assign the user input initial value to the first array position
    c->cr2[0]=c->c2_0;
    c->cr3[0]=c->c3_0;
    inc = (c->time_final)/(NUM_POINTS-1);
    for(ix=1; ix<NUM_POINTS; ix++)
    {
        c->cr1[ix]= (c->cr1[ix-1]+(((f->Q_01*c->c_01)+(f->Q_31*c->cr3[ix-1])-(f->Q_12*c->cr1[ix-1]))/r->v_1)*inc);
        c->cr2[ix]=(c->cr2[ix-1]+(((f->Q_12*c->cr1[ix])-(f->Q_23*c->cr2[ix-1]))/r->v_2)*inc);
        c->cr3[ix]=(c->cr3[ix-1]+(((f->Q_03*c->c_03)+(f->Q_23*c->cr2[ix-1])-(f->Q_31*c->cr3[ix-1])+(f->Q_33*c->cr3[ix-1]))/r->v_3)*inc);
        c->time_axis[ix]=c->time_axis[ix-1]+inc;
    }
}


/*-------------------------------------------------
 Function: plotTable()

 Parameters:
   CONCENTRATIONS *cPtr
 Return value: none.
 Description: Initializes the plot.  The following values
              in the referenced structure are used to setup
              the plot:
              cPtr->cr1,cPtr->cr2,cPtr->cr3 point to arrays holding y axis values
              cPtr->time_axis pointer to x axis array

-------------------------------------------------*/
void plotTable(CONCENTRATIONS *cPtr)
{

    //Need to adapt to the 3 graphs!!

    // Variable declaration
    double minFx1, minFx2, minFx3, minFx, maxFx1, maxFx2, maxFx3, maxFx;  // Minimum and maximum values of f(x)
    // Setup plot configuration
    plsdev("wingcc");  // Sets device to wingcc - CodeBlocks compiler
    // Initialize the plot
    plinit();
    // Configure the axis and labels
    plwidth(3);          // select the width of the pen
    minFx1 = getMinDouble(cPtr->cr1, NUM_POINTS);
    minFx2 = getMinDouble(cPtr->cr2,NUM_POINTS);
    minFx3 = getMinDouble(cPtr->cr3,NUM_POINTS);
    minFx = minFx1;
    if (minFx1>minFx2)
    {
        minFx = minFx2;
    }
    if (minFx>minFx3)
    {
        minFx = minFx3;
    }

    maxFx1 = getMaxDouble(cPtr->cr1, NUM_POINTS);
    maxFx2 = getMaxDouble(cPtr->cr2,NUM_POINTS);
    maxFx3 = getMaxDouble(cPtr->cr3,NUM_POINTS);
    maxFx = maxFx1;
    if (maxFx<maxFx2)
    {
        maxFx = maxFx2;
    }
    if (maxFx < maxFx3)
    {
        maxFx = maxFx3;
    }
    plenv(cPtr->time_axis[0],cPtr->time_axis[NUM_POINTS-1],
          minFx, maxFx, 0, 0);
    plcol0(GREEN);           // Select color for labels
    pllab("time", "Concentration", "Change in in Concentration vs Time (C1-Blue C2-Red C3-Yellow)");
    // Plot the function.
    plcol0(BLUE);    // Color for plotting curve
    plline(NUM_POINTS, cPtr->time_axis, cPtr->cr1);
    plcol0(RED);    // Color for plotting curve
    plline(NUM_POINTS, cPtr->time_axis, cPtr->cr2);
    plcol0(YELLOW);    // Color for plotting curve
    plline(NUM_POINTS, cPtr->time_axis, cPtr->cr3);
    plend();

}



/*-----------------------------------------------------------------------
Function: receiveUserInputs
Parameters:
    REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr
Return:  void
Description:  requests the user inputs, evaluates them against constraint
              equations: q01 + q31 - q12 = 0;
                         Q12-Q23 = 0;
                         Q03 + Q23 -Q31 -Q33 =0;
                         Q01 + Q03 - Q33 =0;
            If all pass then  the values will be saved to the file for future use
            if there is space(we save 5). Then we return True if successful
            or false if the user aborts.
------------------------------------------------------------------------*/
void storeFiles(REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr)
{
    FILE* fp;
    int numsaved=0;
    USER_INPUTS to_be_saved,dummy_store;
    char savetofile;
    int i;

    fp = fopen(BINFILE,"rb");
    //should catch if no file exists
    if (fp !=NULL)
    {
        for (i=0; i< MAXRECORDS; i++)
        {
            fread(&dummy_store, sizeof(USER_INPUTS),1,fp);
            if(!feof(fp))
                numsaved++;
        }
        fclose(fp);
    }

    if(numsaved<5)
    {
        printf("Would you like to save these values to file?\n");
        scanf(" %c", &savetofile );
        if ((savetofile=='y') | (savetofile=='Y'))
        {
            to_be_saved.v1 = rPtr->v_1;
            to_be_saved.v2 = rPtr->v_2;
            to_be_saved.v3 = rPtr->v_3;

            to_be_saved.q01 = fPtr->Q_01;
            to_be_saved.q03 = fPtr->Q_03;
            to_be_saved.q12 = fPtr->Q_12;
            to_be_saved.q23 = fPtr->Q_23;
            to_be_saved.q31 = fPtr->Q_31;
            to_be_saved.q33 = fPtr->Q_33;

            to_be_saved.c01 = cPtr->c_01;
            to_be_saved.c03 = cPtr->c_03;
            to_be_saved.c10 = cPtr->c1_0;
            to_be_saved.c20 = cPtr->c2_0;
            to_be_saved.c30 = cPtr->c3_0;
            to_be_saved.time_final = cPtr->time_final;
            fp = fopen(BINFILE,"ab");
            fwrite(&to_be_saved, sizeof(USER_INPUTS),1,fp);
            fclose(fp);
        }
    }


}



/*-----------------------------------------------------------------------
Function: receiveUserInputs
Parameters:
    REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr
Return:  void
Description:  requests the user inputs, evaluates them against constraint
              equations: q01 + q31 - q12 = 0;
                         Q12-Q23 = 0;
                         Q03 + Q23 -Q31 -Q33 =0;
                         Q01 + Q03 - Q33 =0;
            If all pass then  the values will be saved to the file for future use
            if there is space(we save 5). Then we return True if successful
            or false if the user aborts.
------------------------------------------------------------------------*/
int retrieveFiles(REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr)
{
    FILE* fp;
    int numsaved=0;
    USER_INPUTS saved[MAXRECORDS];
    //FILE* fp;
    int record_choice=0;
    int i;

    fp = fopen(BINFILE,"rb");
    //should catch if no file exists
    if (fp !=NULL)
    {
        for (i=0; i< MAXRECORDS; i++)
        {
            fread(&saved[i], sizeof(USER_INPUTS),1,fp);
            if(!feof(fp))
                numsaved++;
        }
        fclose(fp);
    }

    if (numsaved>0)
    {
        printf("You have the following saved testcases:\n");
        for(i=0; i< numsaved; i++)
        {
            printf("Record %d \n",i+1);
            printf("==========\n");
            printf("Volumes V1: %lf V2: %lf  V3: %lf\n", saved[i].v1,saved[i].v2,saved[i].v3);
            printf("Q Values  Q01: %lf Q03: %lf  Q12: %lf  Q23: %lf  Q31: %lf  Q33: %lf\n",
                   saved[i].q01,saved[i].q03,saved[i].q12,saved[i].q23,saved[i].q31,saved[i].q33);
            printf("Concentrations C01: %lf C03: %lf  C10: %lf  C20: %lf C30: %lf\n",
                   saved[i].c01,saved[i].c03,saved[i].c10,saved[i].c20,saved[i].c30);
            printf("Time Final tf: %lf\n", saved[i].time_final);
        }
        printf("Would you like to used one of the saved testcases? Enter the record number, or 0 if not.\n");
        scanf(" %d",&record_choice);
        if((record_choice >=0) & (record_choice <=numsaved))
        {
            rPtr->v_1=saved[record_choice-1].v1;
            rPtr->v_2=saved[record_choice-1].v2;
            rPtr->v_3=saved[record_choice-1].v3;

            fPtr->Q_01=saved[record_choice-1].q01;
            fPtr->Q_03=saved[record_choice-1].q03;
            fPtr->Q_12=saved[record_choice-1].q12;
            fPtr->Q_23=saved[record_choice-1].q23;
            fPtr->Q_31=saved[record_choice-1].q31;
            fPtr->Q_33=saved[record_choice-1].q33;

            cPtr->c_01=saved[record_choice-1].c01;
            cPtr->c_03=saved[record_choice-1].c03;
            cPtr->c1_0=saved[record_choice-1].c10;
            cPtr->c2_0=saved[record_choice-1].c20;
            cPtr->c3_0=saved[record_choice-1].c30;
            cPtr->time_final=saved[record_choice-1].time_final;

        }
    }
    return record_choice;
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
        if(min > arr[ix])
            min = arr[ix];
    }
    return(min);
}

/*-------------------------------------------------
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
        if(max < arr[ix])
            max = arr[ix];
    }
    return(max);
}
/*-----------------------------------------------------------------------
Function: receiveUserInputs
Parameters:
    REACTORS *rPtr, FLOW_RATES *fPtr, CONCENTRATIONS *cPtr
Return:  void
Description:  requests the user inputs, evaluates them against constraint
              equations: q01 + q31 - q12 = 0;
                         Q12-Q23 = 0;
                         Q03 + Q23 -Q31 -Q33 =0;
                         Q01 + Q03 - Q33 =0;
            If all pass then  the values will be saved to the file for future use
            if there is space(we save 5). Then we return True if successful
            or false if the user aborts.
------------------------------------------------------------------------*/
int testConstraints(FLOW_RATES *fPtr)
{

    int pass=TRUE;

    if (fPtr->Q_01+fPtr->Q_31-fPtr->Q_12!=0.0)
    {
        printf("Sorry, that doesn't satisfy Q01 + Q31 - Q12 = 0\n");
        pass = FALSE;
    }

    if (fPtr->Q_12-fPtr->Q_23!=0.0)
    {
        printf("Sorry, that doesn't satisfy Q12 - Q23 = 0\n");
        pass = FALSE;
    }

    if (fPtr->Q_03+fPtr->Q_23-fPtr->Q_31 -fPtr->Q_33 !=0.0)
    {
        printf("Sorry, that doesn't satisfy Q03 + Q23 -Q31 -Q33 =0\n");
        pass = FALSE;
    }

    if (fPtr->Q_01+fPtr->Q_03-fPtr->Q_33 !=0.0)
    {
        printf("Sorry, that doesn't satisfy Q01 + Q03 - Q33 =0\n");
        pass = FALSE;
    }
    return (pass);
}


