/*
 gcc SHORTfft3c.c cpuidc.c -lm -lrt -O3 -march=armv8-a  -o shortfft64 
 version armv8 64 Bit FFT Benchmark Version 3c.0
 gcc SHORTfft3c.c cpuidc.c -lm -lrt -O3 -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4 -o shortfft32
 change version to SHORT armv7 32 Bit FFT Benchmark 
*/


 #include <stdio.h>       
 #include <stdlib.h>
 #include <math.h>
 #include <time.h>
 #include "cpuidh.h"

// ************* GLOBAL VARIABLES **********

#define TsizeL2     13  // cache size parameter 13 = 8K FFT 128 KB SP 256
                        // KB DP when segmenting data to fit in L2 cache
                        // for fftSP() and fftDP()
#define TsizeLL     14  // cache size parameter TsizeL2 + 1

double   cDataDP[8388608]; // 8 MW 64 MB;
int      RAMunit = 1048576;
double   startTest;

double   *cSourceDP;
double   *cExpDP;
double   *dExpDP;   
double   *dTable;

float    *cDataSP;
float    *cSourceSP;
float    *cExpSP;
float    *dExpSP;
int      *table;  

float    *pData;
double   *pDatad;

float  ExpReal1SP;
float  ExpImag1SP;
float  ExpReal2SP;
float  ExpImag2SP;
float  ExpReal3SP;
float  ExpImag3SP;
float  ExpReal4SP;
float  ExpImag4SP;
 

 float  PISP = (float)3.14159265;
 double PIDP = 3.14159265358979323846;

 int  nSP;
 int  nDP;

 char    resultchars[1200];
 
//  ************** PROTOPTYPES *****************

void bitrev(int);
void createExpTableSP(int n, int sign);
void createExpTableDP(int n, int sign);
void createComplexDataDP (int n, double synthFreqI, double synthFreqQ);
void createComplexDataSP (int n, float synthFreqI, float synthFreqQ);

void fftSP(int t, int n); 
void fftDP(int t, int n);
void fftSP2(int t,int n); 
void fftDP2(int t, int n);
FILE    *outfile;


void runIt();


void main()
{
    int i, g; 
    char version[80] = " SHORT armv8 64 Bit FFT Benchmark ";

    start_time();
    startTest = startSecs;    
    outfile = fopen("Short-FFT-tests.txt","a+");
    if (outfile == NULL)
    {
        printf ("Cannot open results file \n\n");
        printf(" Press Enter\n");
        g  = getchar();
        exit (0);
    }
    local_time();
    printf("\n");
    printf(" ###################################################\n\n");
    printf("  %s%s\n", version, timeday);
    printf(" Milliseconds for 30 fft measurements\n\n");
    
    fprintf(outfile, "\n");
    fprintf(outfile, " ###################################################\n\n");
    fprintf(outfile, "  %s%s\n", version, timeday);
    fprintf(outfile, " Milliseconds for 30 fft measurements\n\n");

    runIt();

    startSecs = startTest;
    end_time();
    printf(" Running time %10.3f milliseconds\n\n", secs * 1000.0);
    fprintf(outfile, " Running time %10.3f milliseconds\n\n", secs * 1000.0);
    fclose(outfile);
}


void runIt()
{
  int j, k, n, t, f,g, sign;
  float synthFreq;
  int size[11];

  double msTotal;

   cSourceDP  = cDataDP + RAMunit*2;
   cExpDP     = cDataDP + RAMunit*4;
   dExpDP     = cDataDP + RAMunit*5;
   dTable     = cDataDP + RAMunit*6;

   cDataSP    = (float  *)cDataDP;
   cSourceSP  = (float  *)cSourceDP;
   cExpSP     = (float  *)cExpDP;
   dExpSP     = (float  *)dExpDP;
   table      = (int    *)dTable;

  for (f=0; f<50; f++)
  {
    for (g=0; g<10; g++)
    {
      msTotal = 0.0;
      int tt=10;
      for (t=10; t<15; t++)  // 21
      {
        k = t-10;
        n = (int)pow(2,(double)tt);
        nSP = n; 
        size[k] = n/1024;  
        bitrev (t); 
        sign = -1; 
        createExpTableSP(n, sign);
        synthFreq = (float)n/16;
    
        for(j=0;j<3;j++)
        {
            createComplexDataSP(n, synthFreq, synthFreq);
            start_time(); 
            if (tt < TsizeLL)
            {    
               fftSP2 (tt, n);
            }
            else
            {
               fftSP (tt, n);
            } 
            end_time();
            msTotal = msTotal + secs * 1000.0;     
        }
      }   
      for (t=10; t<15; t++)  // 21
      {
        k = t-10;
        n = (int)pow(2,(double)tt);
        nSP = n; 
        size[k] = n/1024;  
        bitrev (t); 
        sign = -1; 
        createExpTableSP(n, sign);
        synthFreq = (float)n/16;
    
        for(j=0;j<3;j++)
        {
            createComplexDataSP(n, synthFreq, synthFreq);
            start_time(); 
            if (tt < TsizeLL)
            {    
               fftSP2 (tt, n);
            }
            else
            {
               fftSP (tt, n);
            } 
            end_time();
            msTotal = msTotal + secs * 1000.0;     
        }
      }
      printf("%6.1f", msTotal);
      fprintf(outfile, "%6.1f", msTotal);   
    }
    printf("\n");
    fprintf(outfile,"\n");
  }
    printf("\n");
    fprintf(outfile, "\n");
 
}


void fftSP2 (int t, int n)
{
  float  inverse_n;
  int   d, d2;
  int   index;
  int   point;
  int   j; 
  int   jinc;
  float  temp1, temp2;
  int k, jk, jkd;
  float  tempReal, tempImag;

  inverse_n = (float)1.0 / (float)n;
  
    for (index=0; index<n; index++)
    {
            point = table[index];
            if (point >= index)
            {
              temp1  = cSourceSP[index*2]     * inverse_n;
              temp2  = cSourceSP[(index*2)+1] * inverse_n;
              cSourceSP[index*2]     = cSourceSP[point*2]     * inverse_n;
              cSourceSP[(index*2)+1] = cSourceSP[(point*2)+1] * inverse_n;
              cSourceSP[point*2]     = temp1;
              cSourceSP[(point*2)+1] = temp2;
            }
            
    }

    d = 1;
    for(index=1;index<=t;index++)
    {
        d2 = d + d;
        jinc = 2;
        if (d > 2) jinc = 4;
        for (j=0; j<d; j=j+jinc)
        {

            ExpReal1SP = cExpSP[(n / d2) * j * 2];
            ExpImag1SP = cExpSP[(n / d2) * j * 2+1];
            ExpReal2SP = cExpSP[(n / d2) * (j+1)*2];
            ExpImag2SP = cExpSP[(n / d2) * (j+1)*2+1];

            if (d > 2)
            { 
                ExpReal3SP = cExpSP[(n / d2) * (j+2)*2];
                ExpImag3SP = cExpSP[(n / d2) * (j+2)*2+1];    
                ExpReal4SP = cExpSP[(n / d2) * (j+3)*2];
                ExpImag4SP = cExpSP[(n / d2) * (j+3)*2+1];
            }

            if (d == 1)
            {
                for(k=0;k<n;k=k+d2)
                {
                      jk           = (j + k) * 2;
                      jkd          = (j + k + d) * 2;
                      tempReal   = (cDataSP[jkd]   * ExpReal1SP) - (cDataSP[jkd+1] * ExpImag1SP);
                      tempImag   = (cDataSP[jkd+1] * ExpReal1SP) + (cDataSP[jkd]   * ExpImag1SP);
                      cDataSP[jkd]  = cDataSP[jk] - tempReal;
                      cDataSP[jkd+1]  = cDataSP[jk+1] - tempImag;
                      cDataSP[jk]   = cDataSP[jk] + tempReal;
                      cDataSP[jk+1]   = cDataSP[jk+1] + tempImag;
                }
            }
            else if (d == 2)
            {
                for(k=0;k<n;k=k+d2)
                {
                      jk           = (j + k) * 2;
                      jkd          = (j + k + d) * 2;
                      tempReal   = (cDataSP[jkd]   * ExpReal1SP) - (cDataSP[jkd+1] * ExpImag1SP);
                      tempImag   = (cDataSP[jkd+1] * ExpReal1SP) + (cDataSP[jkd]   * ExpImag1SP);
                      cDataSP[jkd]  = cDataSP[jk] - tempReal;
                      cDataSP[jkd+1]  = cDataSP[jk+1] - tempImag;
                      cDataSP[jk]   = cDataSP[jk] + tempReal;
                      cDataSP[jk+1]   = cDataSP[jk+1] + tempImag;
        
                      tempReal   = (cDataSP[jkd+2] * ExpReal2SP) - (cDataSP[jkd+3] * ExpImag2SP);
                      tempImag   = (cDataSP[jkd+3] * ExpReal2SP) + (cDataSP[jkd+2] * ExpImag2SP);
                      cDataSP[jkd+2]  = cDataSP[jk+2] - tempReal;
                      cDataSP[jkd+3]  = cDataSP[jk+3] - tempImag;
                      cDataSP[jk+2]   = cDataSP[jk+2] + tempReal;
                      cDataSP[jk+3]   = cDataSP[jk+3] + tempImag;        
                }
            }
            else
            {
                for(k=0;k<n;k=k+d2)
                {
                      jk           = (j + k) * 2;
                      jkd          = (j + k + d) * 2;
                      tempReal   = (cDataSP[jkd]   * ExpReal1SP) - (cDataSP[jkd+1] * ExpImag1SP);
                      tempImag   = (cDataSP[jkd+1] * ExpReal1SP) + (cDataSP[jkd]   * ExpImag1SP);
                      cDataSP[jkd]  = cDataSP[jk] - tempReal;
                      cDataSP[jkd+1]  = cDataSP[jk+1] - tempImag;
                      cDataSP[jk]   = cDataSP[jk] + tempReal;
                      cDataSP[jk+1]   = cDataSP[jk+1] + tempImag;
        
                      tempReal   = (cDataSP[jkd+2] * ExpReal2SP) - (cDataSP[jkd+3] * ExpImag2SP);
                      tempImag   = (cDataSP[jkd+3] * ExpReal2SP) + (cDataSP[jkd+2] * ExpImag2SP);
                      cDataSP[jkd+2]  = cDataSP[jk+2] - tempReal;
                      cDataSP[jkd+3]  = cDataSP[jk+3] - tempImag;
                      cDataSP[jk+2]   = cDataSP[jk+2] + tempReal;
                      cDataSP[jk+3]   = cDataSP[jk+3] + tempImag;
        
                      tempReal   = (cDataSP[jkd+4] * ExpReal3SP) - (cDataSP[jkd+5] * ExpImag3SP);
                      tempImag   = (cDataSP[jkd+5] * ExpReal3SP) + (cDataSP[jkd+4] * ExpImag3SP);
                      cDataSP[jkd+4]  = cDataSP[jk+4] - tempReal;
                      cDataSP[jkd+5]  = cDataSP[jk+5] - tempImag;
                      cDataSP[jk+4]   = cDataSP[jk+4] + tempReal;
                      cDataSP[jk+5]   = cDataSP[jk+5] + tempImag;
        
                      tempReal   = (cDataSP[jkd+6] * ExpReal4SP) - (cDataSP[jkd+7] * ExpImag4SP);
                      tempImag   = (cDataSP[jkd+7] * ExpReal4SP) + (cDataSP[jkd+6] * ExpImag4SP);
                      cDataSP[jkd+6]  = cDataSP[jk+6] - tempReal;
                      cDataSP[jkd+7]  = cDataSP[jk+7] - tempImag;
                      cDataSP[jk+6]   = cDataSP[jk+6] + tempReal;
                      cDataSP[jk+7]   = cDataSP[jk+7] + tempImag;
                }
            }
        }
        d = d2;
    }
} // fftSP2

void fftSP (int t, int n)
{
    float  inverse_n;
    int   index;
    int   d = 0;
    int   d2;
    int   point, Nsize, Tsize, seg, NumSeg, offset, RevSeg;
    int   j;
    int  k, jinc;
    int   jk, jkd;
    float  tempReal, tempImag;

    inverse_n = (float)1.0 / (float)n;

    if(t >= TsizeL2)
       Tsize = TsizeL2;
    else
        Tsize = t;

    Nsize  = (int)pow((double)2,(double)Tsize);
    NumSeg = (int) n / Nsize;

    //************************************************************
    // Break the work up into NumSeg work units of size TsizeL2 
    //************************************************************

    for(seg=0;seg<NumSeg;seg++)
    {
        RevSeg = 0;
        for(k=0;k<(t-Tsize);k++)
                RevSeg = (RevSeg << 1) | ((seg >> k) & 0x1);

        pData = cDataSP+(RevSeg*Nsize*2);
        offset = seg * 2;

        // bit reverse each set
        for (index=0; index<Nsize; index++)
        {
            point = table[index];
            if (point >= index)
            {
                pData[index*2]     = cSourceSP[(point*2*NumSeg)+offset]   * inverse_n;
                pData[(index*2)+1] = cSourceSP[(point*2*NumSeg)+offset+1] * inverse_n;       
                pData[point*2]     = cSourceSP[(index*2*NumSeg)+offset]   * inverse_n;
                pData[(point*2)+1] = cSourceSP[(index*2*NumSeg)+offset+1] * inverse_n;
            }
        }

        // transform each set
        d = 1;
        for(index=1;index<=Tsize;index++)
        {
            d2 = d + d;
            jinc = 2;
            if (d > 2) jinc = 4;
            for (j=0; j<d; j=j+jinc)
            {

                ExpReal1SP = dExpSP[(Nsize / d2) * j * 2];
                ExpImag1SP = dExpSP[(Nsize / d2) * j * 2+1];
                ExpReal2SP = dExpSP[(Nsize / d2) * (j+1)*2];
                ExpImag2SP = dExpSP[(Nsize / d2) * (j+1)*2+1];

                if (d > 2)
                { 
                    ExpReal3SP = dExpSP[(Nsize / d2) * (j+2)*2];
                    ExpImag3SP = dExpSP[(Nsize / d2) * (j+2)*2+1];    
                    ExpReal4SP = dExpSP[(Nsize / d2) * (j+3)*2];
                    ExpImag4SP = dExpSP[(Nsize / d2) * (j+3)*2+1];
                }

                if (d == 1)
                {
                    for(k=0;k<Nsize;k=k+d2)
                    {
                      jk      = (j + k) * 2;
                      jkd = (j + k + d) * 2;
                      tempReal   = (pData[jkd]   * ExpReal1SP) - (pData[jkd+1] * ExpImag1SP);
                      tempImag   = (pData[jkd+1] * ExpReal1SP) + (pData[jkd]   * ExpImag1SP);
                      pData[jkd]  = pData[jk] - tempReal;
                      pData[jkd+1]  = pData[jk+1] - tempImag;
                      pData[jk]   = pData[jk] + tempReal;
                      pData[jk+1]   = pData[jk+1] + tempImag;
                    }
                }
                else if (d == 2)
                {
                    for(k=0;k<Nsize;k=k+d2)
                    {
                      jk      = (j + k) * 2;
                      jkd = (j + k + d) * 2;
                      tempReal   = (pData[jkd]   * ExpReal1SP) - (pData[jkd+1] * ExpImag1SP);
                      tempImag   = (pData[jkd+1] * ExpReal1SP) + (pData[jkd]   * ExpImag1SP);
                      pData[jkd]  = pData[jk] - tempReal;
                      pData[jkd+1]  = pData[jk+1] - tempImag;
                      pData[jk]   = pData[jk] + tempReal;
                      pData[jk+1]   = pData[jk+1] + tempImag;
        
                      tempReal   = (pData[jkd+2] * ExpReal2SP) - (pData[jkd+3] * ExpImag2SP);
                      tempImag   = (pData[jkd+3] * ExpReal2SP) + (pData[jkd+2] * ExpImag2SP);
                      pData[jkd+2]  = pData[jk+2] - tempReal;
                      pData[jkd+3]  = pData[jk+3] - tempImag;
                      pData[jk+2]   = pData[jk+2] + tempReal;
                      pData[jk+3]   = pData[jk+3] + tempImag;
                    }
                }
                else
                {
                    for(k=0;k<Nsize;k=k+d2)
                    {
                      jk      = (j + k) * 2;
                      jkd = (j + k + d) * 2;
                      tempReal   = (pData[jkd]   * ExpReal1SP) - (pData[jkd+1] * ExpImag1SP);
                      tempImag   = (pData[jkd+1] * ExpReal1SP) + (pData[jkd]   * ExpImag1SP);
                      pData[jkd]  = pData[jk] - tempReal;
                      pData[jkd+1]  = pData[jk+1] - tempImag;
                      pData[jk]   = pData[jk] + tempReal;
                      pData[jk+1]   = pData[jk+1] + tempImag;
        
                      tempReal   = (pData[jkd+2] * ExpReal2SP) - (pData[jkd+3] * ExpImag2SP);
                      tempImag   = (pData[jkd+3] * ExpReal2SP) + (pData[jkd+2] * ExpImag2SP);
                      pData[jkd+2]  = pData[jk+2] - tempReal;
                      pData[jkd+3]  = pData[jk+3] - tempImag;
                      pData[jk+2]   = pData[jk+2] + tempReal;
                      pData[jk+3]   = pData[jk+3] + tempImag;
        
                      tempReal   = (pData[jkd+4] * ExpReal3SP) - (pData[jkd+5] * ExpImag3SP);
                      tempImag   = (pData[jkd+5] * ExpReal3SP) + (pData[jkd+4] * ExpImag3SP);
                      pData[jkd+4]  = pData[jk+4] - tempReal;
                      pData[jkd+5]  = pData[jk+5] - tempImag;
                      pData[jk+4]   = pData[jk+4] + tempReal;
                      pData[jk+5]   = pData[jk+5] + tempImag;
        
                      tempReal   = (pData[jkd+6] * ExpReal4SP) - (pData[jkd+7] * ExpImag4SP);
                      tempImag   = (pData[jkd+7] * ExpReal4SP) + (pData[jkd+6] * ExpImag4SP);
                      pData[jkd+6]  = pData[jk+6] - tempReal;
                      pData[jkd+7]  = pData[jk+7] - tempImag;
                      pData[jk+6]   = pData[jk+6] + tempReal;
                      pData[jk+7]   = pData[jk+7] + tempImag;
                    }
                }                            
            }
            d = d2;
        }
    }

    //***********************************************************
    // This will recombine the above work units (if necessary) 
    //  Only run when t > TsizeL2 eg TsizeL2 = 13 run at size 16384
    //**********************************************************

   for(index=TsizeL2+1; index<=t; index++)
   {
      d2 = d + d;
      for (j=0; j<d; j=j+4)
      {
        ExpReal1SP = cExpSP[(n / d2 * j)*2];
        ExpImag1SP = cExpSP[(n / d2 * j)*2+1];
        ExpReal2SP = cExpSP[(n / d2 * (j+1))*2];
        ExpImag2SP = cExpSP[(n / d2 * (j+1))*2+1];
        ExpReal3SP = cExpSP[(n / d2 * (j+2))*2];
        ExpImag3SP = cExpSP[(n / d2 * (j+2))*2+1];
        ExpReal4SP = cExpSP[(n / d2 * (j+3))*2];
        ExpImag4SP = cExpSP[(n / d2 * (j+3))*2+1];

        for(k=0;k<n;k=k+d2)
        {
          jk      = (j + k) * 2;
          jkd = (j + k + d) * 2;
          tempReal   = (cDataSP[jkd]   * ExpReal1SP) - (cDataSP[jkd+1] * ExpImag1SP);
          tempImag   = (cDataSP[jkd+1] * ExpReal1SP) + (cDataSP[jkd]   * ExpImag1SP);
          cDataSP[jkd]  = cDataSP[jk] - tempReal;
          cDataSP[jkd+1]  = cDataSP[jk+1] - tempImag;
          cDataSP[jk]   = cDataSP[jk] + tempReal;
          cDataSP[jk+1]   = cDataSP[jk+1] + tempImag;

          tempReal   = (cDataSP[jkd+2] * ExpReal2SP) - (cDataSP[jkd+3] * ExpImag2SP);
          tempImag   = (cDataSP[jkd+3] * ExpReal2SP) + (cDataSP[jkd+2] * ExpImag2SP);
          cDataSP[jkd+2]  = cDataSP[jk+2] - tempReal;
          cDataSP[jkd+3]  = cDataSP[jk+3] - tempImag;
          cDataSP[jk+2]   = cDataSP[jk+2] + tempReal;
          cDataSP[jk+3]   = cDataSP[jk+3] + tempImag;

          tempReal   = (cDataSP[jkd+4] * ExpReal3SP) - (cDataSP[jkd+5] * ExpImag3SP);
          tempImag   = (cDataSP[jkd+5] * ExpReal3SP) + (cDataSP[jkd+4] * ExpImag3SP);
          cDataSP[jkd+4]  = cDataSP[jk+4] - tempReal;
          cDataSP[jkd+5]  = cDataSP[jk+5] - tempImag;
          cDataSP[jk+4]   = cDataSP[jk+4] + tempReal;
          cDataSP[jk+5]   = cDataSP[jk+5] + tempImag;

          tempReal   = (cDataSP[jkd+6] * ExpReal4SP) - (cDataSP[jkd+7] * ExpImag4SP);
          tempImag   = (cDataSP[jkd+7] * ExpReal4SP) + (cDataSP[jkd+6] * ExpImag4SP);
          cDataSP[jkd+6]  = cDataSP[jk+6] - tempReal;
          cDataSP[jkd+7]  = cDataSP[jk+7] - tempImag;
          cDataSP[jk+6]   = cDataSP[jk+6] + tempReal;
          cDataSP[jk+7]   = cDataSP[jk+7] + tempImag;
        }
      }
      d = d2;
   }

} // fftSP




// create data to avoid errors

void createComplexDataSP (int n, float synthFreqI, float synthFreqQ)
{
  float  constI, constQ;
  float  rindex;
  int   index;
 
  constI = ((float)2.0  * PISP * synthFreqI) / (float)n;
  constQ = ((float)2.0  * PISP * synthFreqQ) / (float)n;

  for (index=0; index<n; index++)
  {
    rindex = (float)index;
    cSourceSP[index*2]     = (float)cos (constI * rindex);
    cSourceSP[(index*2)+1] = (float)sin (constQ * rindex);
  }
}

// create exponent table

void createExpTableSP (int n, int sign)
{
    int   index, Nsize;
    float const0;
    float rindex;
    float fsign;
    float pi2;
    
    fsign = (float)sign;
    pi2 = (float)2.0 * PISP;  
    const0 = (pi2 * fsign)  / (float)n;
    for (index=0; index<n/2; index++)
    {
        rindex = (float)index;
        cExpSP[index*2]   = (float)cos (const0 * rindex);
        cExpSP[index*2+1] = (float)sin (const0 * rindex);
    }
    Nsize = (int)pow((double)2,(double)TsizeL2);
    if(n < Nsize) Nsize = n;
    const0 = (pi2 * fsign)  / (float)Nsize;
    for (index=0; index<Nsize/2; index++)
    {
        rindex = (float)index;
        dExpSP[index*2]      = (float)cos (const0 * rindex);
        dExpSP[(index*2)+1]  = (float)sin (const0 * rindex);
    }
}



// bit reverse table

void bitrev (int t)
{
  int index;
  int point;
  int m;
  int p;
  int Nsize, Tsize;

  if(t > TsizeL2)
         Tsize = TsizeL2;
  else
          Tsize = t;

  Nsize = (int)pow((double)2,(double)Tsize);

  for (index=0; index<Nsize; index++)
  {
    p = 0;
    m = index;
    for (point=0; point<Tsize; point++)
    {
      p = p + p;
      p = p + (m & 1);
      m = m >> 1;
    }
    table[index] = p;
  }

} // bit reverse table


