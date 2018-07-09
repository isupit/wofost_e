#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "wofost.h"
#include "extern.h"
#include "netcdf.h"

int GetMeteoData(char* path, char* dateString, char* station)
{
    int c, i, j, day, day_number, start_day;
    int year, month;
    int end;
    int sim_day;
    float A, B, Rad, Rainfall, VapourPressure;
    char file_1[100];
    char file_2[100];

    FILE *fq;
    //char dummy[100];
    char str_year[5];
    char str_rest[5];
    char str_i[1];

    if(sscanf(dateString, "%d-%d-%d", &year, &month, &start_day) != EOF)
    {
        /* tm_year is years since 1900 */
        simTime.tm_year = year - 1900;

        /* tm_months is months since January */
        simTime.tm_mon = month - 1;
        simTime.tm_mday = start_day;

        /* to start day number from 0 if omitted day numbering starts at -1 */
        simTime.tm_hour = 23;
        simTime.tm_min  = 59;
        simTime.tm_sec  = 59;
        mktime(&simTime);

        /* start day numbering  at 1*/
        day_number =  1; //simTime.tm_yday + 1;  
    }
  
    if (year < 9999)
    {
      sprintf(str_year,"%d",year);
      strcpy(file_1, path);
      strcat(file_1,station);

      j = year / 1000;
      sprintf(str_i,"%d",j);
      strcat(file_1, str_i);
      strcat(file_1,".");
      strcpy(file_2,file_1);

      sprintf(str_rest,"%3d", year - 1000 * j);
      strcat(file_1, str_rest);

      sprintf(str_rest,"%3d", year - 999 * j);
      strcat(file_2, str_rest);

    }
    else
    {
          fprintf(stderr, "Impossible year, check your data\n"); 
          exit(0);
    }


    /* open first year */
    if ((fq = fopen(file_1, "rt")) == NULL)
        {fprintf(stderr, "Cannot open 1st input file.\n"); return 0;}

    /* skip lines in the header */ 
    while ((c=fgetc(fq)) == '*') 
        while ((c=fgetc(fq)) != '\n');

    fscanf(fq,"%f %f %f %f %f", &Longitude, &Latitude, &Altitude,  &A, &B);

    AngstA = fabs(A);
    AngstB = fabs(B);

    end = leap_year(year);
    for (i=day_number;i<=end;i++)
    {    
        /* Set first index to 1 i.e. first simulation day */
        sim_day = i - day_number  +1;
        fscanf(fq,"%d %d %d %f %f %f %f %f %f", &Station, &Year, &day, &Rad,
                                          &Tmin[sim_day], 
                                          &Tmax[sim_day], 
                                          &VapourPressure,
                                          &Windspeed[sim_day], 
                                          &Rainfall);
        
        /* Transform Radiation from KJ m-2 d-1 to J m-2 d-1 */
        Radiation[sim_day] = 1000.*Rad;
        
        /* Transform Rain from mm d-1 to cm d-1 */
        Rain[sim_day] = 0.1 * Rainfall;
        
        /* Transform Vapour from kPa to hPa*/
        Vapour[sim_day] = 10. * VapourPressure;
  }
    fclose(fq);

    
    /* open second year */
    if ((fq = fopen(file_2, "rt")) == NULL)
        {fprintf(stderr, "Cannot open 2nd input file.\n"); return 0;}

    /* skip lines in the header */ 
    while ((c=fgetc(fq)) == '*') 
        while ((c=fgetc(fq)) != '\n');
       
    /* skip the line with the angstrom parameters, elevation, etc.*/
    while ((c=fgetc(fq)) != '\n');    
         
    
    end = leap_year(year + 1);
    for (i = 1; i<=end; i++)
    {
        sim_day++;
        fscanf(fq,"%d %d %d %f %f %f %f %f %f", &Station, &Year, &day, &Rad,
                                  &Tmin[sim_day], 
                                  &Tmax[sim_day], 
                                  &Vapour[sim_day],
                                  &Windspeed[sim_day], 
                                  &Rain[sim_day]);		
        /* Transform Radiation from KJ m-2 d-1 to J m-2 d-1 */
        Radiation[sim_day] = 1000.*Rad;
    }
    fclose(fq);
    
 return 1;
}


int GetEnsembles(float* qarray, float* tminarray, float* tmaxarray, float* swarray, float* windarray, float* preciparray,int N_ensembles){
        int ncid, varid;//

    /* The start and count arrays will tell the netCDF library where to read the data. */
    size_t start[2], count[2];

    /* Error handling. */
    int retval;

    /* Open the file. */
    if ((retval = nc_open("data/ensembles.nc", NC_NOWRITE, &ncid)))
        fprintf(stderr,"Ensembles Netcdf not opened.\n");

    /* Read the data. Since we know the contents of the file we know
     * that the data arrays in this program are the correct size to
     * hold one timestep. */
    
    count[0] = N_ensembles;
    count[1] = 732;
    start[0] = 0;
    start[1] = 0;
   
    if ((retval = nc_inq_varid(ncid, "q" , &varid)))
       fprintf(stderr,"Netcdf variable not found.\n");
    if ((retval = nc_get_vara_float(ncid, varid, start,count, &qarray[0])))
       fprintf(stderr,"Netcdf data not read.\n");

    if ((retval = nc_inq_varid(ncid, "tmin" , &varid)))
       fprintf(stderr,"Netcdf variable not found.\n");
    if ((retval = nc_get_vara_float(ncid, varid, start,count, &tminarray[0])))
       fprintf(stderr,"Netcdf data not read.\n");

    if ((retval = nc_inq_varid(ncid, "precip" , &varid)))
       fprintf(stderr,"Netcdf variable not found.\n");
    if ((retval = nc_get_vara_float(ncid, varid, start,count, &preciparray[0])))
       fprintf(stderr,"Netcdf data not read.\n");

    if ((retval = nc_inq_varid(ncid, "sw" , &varid)))
       fprintf(stderr,"Netcdf variable not found.\n");
    if ((retval = nc_get_vara_float(ncid, varid, start,count, &swarray[0])))
       fprintf(stderr,"Netcdf data not read.\n");

    if ((retval = nc_inq_varid(ncid, "tmax" , &varid)))
       fprintf(stderr,"Netcdf variable not found.\n");
    if ((retval = nc_get_vara_float(ncid, varid, start,count, &tmaxarray[0])))
       fprintf(stderr,"Netcdf data not read.\n");

    if ((retval = nc_inq_varid(ncid, "wind" , &varid)))
       fprintf(stderr,"Netcdf variable not found.\n");
    if ((retval = nc_get_vara_float(ncid, varid, start,count, &windarray[0])))
       fprintf(stderr,"Netcdf data not read.\n");

              /* Close the file. */
    if ((retval = nc_close(ncid)))
        fprintf(stderr,"File not closed.\n");

    return 0;
}
