#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "wofost.h"
#include "extern.h"
#include <time.h>
#include "netcdf.h"

int main() {
    FILE *ifp;
    FILE **output;
    
    SimUnit *Grid = NULL;
    SimUnit *initial = NULL;
       
    int Emergence;
    int Start;
    int N_ensembles;
    int print_all;
    int max_N_ensembles=0;
    int CycleLength   = 300;
    int count;
    float Tsumemergenceinput;
    float Tsum1input;
    float Tsum2input;
    
    char path[100];
    char cropfile[100];
    char soilfile[100];
    char sitefile[100];
    char management[100];
    char dateString [100];
    char place[15];
    
    char cf[100], sf[100], mf[100], site[100];
  
    Step = 1.;    
    
    ifp = fopen("list.txt", "r");

    if (ifp == NULL) 
    {
        fprintf(stderr, "Can't open input list.txt\n");
        exit(1);
    }
    
    count = 0;
    while (fscanf(ifp,"%7s %11s %7s %12s %10s %10s %2s %d %d %d %d %f %f %f" ,
            path, cf, sf, mf, site, dateString, place, &Start, &Emergence, &N_ensembles, &print_all, &Tsumemergenceinput,&Tsum1input,&Tsum2input)
            != EOF) 
    {    
        strncpy(cropfile, path, 98);
        strncat(cropfile, cf, 98);

        strncpy(soilfile, path, 98);
        strncat(soilfile, sf, 98);

        strncpy(management, path, 98);
        strncat(management, mf, 98);

        strncpy(sitefile, path, 98);
        strncat(sitefile, site, 98);
        
	if (N_ensembles>max_N_ensembles){max_N_ensembles=N_ensembles;}
        /* count the number of output files */
        /* number is the index number of the list of file pointers */
        if (initial == NULL) 
        {
            Grid = initial =  malloc(sizeof(SimUnit));
            GetCropData(Grid->crp   = malloc(sizeof(Plant)), cropfile); 
            GetSiteData(Grid->ste   = malloc(sizeof(Field)), sitefile);
            GetManagement(Grid->mng = malloc(sizeof(Management)), management);
            GetSoilData(Grid->soil  = malloc(sizeof(Soil)), soilfile);
            strncpy(Grid->cropfile,cropfile, 98);
            strncpy(Grid->soilfile,soilfile, 98);
            strncpy(Grid->management,management, 98);
            strncpy(Grid->sitefile,sitefile, 98);
            
            Grid->start = Start;
            Grid->ensembles = N_ensembles;
            Grid->file = count++;
            strcpy(Grid->name,cf);
            Grid->emergence = Emergence;
            Grid->next = NULL; 
        }
        else 
        {
            Grid->next = malloc(sizeof(SimUnit));
            Grid = Grid->next;
            GetCropData(Grid->crp   = malloc(sizeof(Plant)), cropfile); 
            GetSiteData(Grid->ste   = malloc(sizeof(Field)), sitefile);
            GetManagement(Grid->mng = malloc(sizeof(Management)), management);
            GetSoilData(Grid->soil  = malloc(sizeof(Soil)), soilfile);
            
            strncpy(Grid->cropfile,cropfile, 98);
            strncpy(Grid->soilfile,soilfile, 98);
            strncpy(Grid->management,management, 98);
            strncpy(Grid->sitefile,sitefile, 98);
            Grid->start = Start;            // Start day (=day number)
            Grid->ensembles = N_ensembles;
            Grid->file  = count++;          // number of elements in Grid carousel
            strcpy(Grid->name,cf);          // Crop file name
            Grid->emergence = Emergence;    // Start the simulations at emergence (1) or at sowing (0)
            Grid->next = NULL;
        }
    }
    
    /* Close the input file */
    fclose(ifp);

    /* Allocate memory for the meteorogical ensembles */
   float* q_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);
   float* wind_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);
   float* tmin_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);
   float* tmax_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);
   float* sw_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);
   float* precip_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);

    /* Read ensemble data */
    GetEnsembles(q_ensembles,tmin_ensembles,tmax_ensembles,sw_ensembles,wind_ensembles,precip_ensembles,max_N_ensembles);

    
    /* Set Grid back to the initial address */
    Grid = initial;   
    
    /* Get the meteodata */
    GetMeteoData(path, dateString, place);
    int i;
    int ens=0;
    
    /* Allocate memory for the file pointers */
    output = malloc(sizeof(**output) * --count);

    //tempcrop = malloc(sizeof(SimUnit));
    
    /* Open the output file */
        output[0] = fopen("ensembles_output.txt", "w");
    
    
        /* Go back to the beginning of the list */
        Grid = initial;
	while (Grid)
        {
		for (ens=0;ens<Grid->ensembles;ens++){
            GetCropData(Grid->crp, Grid->cropfile); 
            GetSiteData(Grid->ste, Grid->sitefile);
            GetManagement(Grid->mng, Grid->management);
            GetSoilData(Grid->soil, Grid->soilfile);
		for (i=1;i<732;i++){
			Tmin[i]=tmin_ensembles[ens*732+i];
			Tmax[i]=tmax_ensembles[ens*732+i];
			Radiation[i]=1000.0*sw_ensembles[ens*732+i];
			Rain[i]=0.1*precip_ensembles[ens*732+i];
			Vapour[i]=10.0*q_ensembles[ens*732+i];
			Windspeed[i]=wind_ensembles[ens*732+i];}

            /* Get data, states and rates from the Grid structure and */
            /* put them in the place holders */
            Crop      = Grid->crp;
            WatBal    = Grid->soil;
            Mng       = Grid->mng;
            Site      = Grid->ste;
            Start     = Grid->start;
            Emergence = Grid->emergence;
	    //printf("Crop: %f %f %f\n",Grid->crp->prm.TSumEmergence,Grid->crp->prm.TempSum1,Grid->crp->prm.TempSum2);
	    
	    if (Tsum1input>0){
	    Grid->crp->prm.TempSum1=Tsum1input;}
	    if (Tsum2input>0){
	    Grid->crp->prm.TempSum1=Tsum2input;}
	    if (Tsumemergenceinput>0){
	    Grid->crp->prm.TSumEmergence=Tsumemergenceinput;}



    for (Day = 1; Day < 732; Day++)
    {        
        
        Temp = 0.5 * (Tmax[Day] + Tmin[Day]);
        DayTemp = 0.5 * (Tmax[Day] + Temp);
        
        Astro();
        CalcPenman();
        
            
            if (Day >= Start && Crop->Emergence == 0)
            {
                if (EmergenceCrop(Emergence))
                {                 
                    /* Initialize */
                    InitializeCrop();
                    InitializeWatBal();
                    InitializeNutrients(); 
                }
            }
            
            if (Day >= Start && Crop->Emergence == 1)
            {   
                if (Crop->DevelopmentStage <= Crop->prm.DevelopStageHarvest && Crop->GrowthDay < CycleLength) 
                {
                    /* Rate calculations */
                    EvapTra();
                    RateCalulationWatBal();
                    RateCalcultionNutrients();
                    RateCalculationCrop();
                    
                    /* Calculate LAI and DVS */
                    Crop->st.LAI = LeaveAreaIndex();
                    Crop->DevelopmentStage = GetDevelopmentStage();
                                       
                if (print_all || Crop->DevelopmentStage >= Crop->prm.DevelopStageHarvest){
                    fprintf(output[0],"%d %d %4d %7.1f %7.1f %7.1f %7.2f %7.2f %7.2f %7.3f %7.2f %7.1f\n",
        		ens,Grid->file, 
                        Day,Crop->st.stems,Crop->st.leaves,Crop->st.storage,
                        Crop->st.LAI,Crop->DevelopmentStage,WatBal->WaterStress,
                        WatBal->st.Moisture,WatBal->rt.Infiltration,Rain[Day]);
		}
                    
                    /* State calculations */
                    IntegrationWatBal();
                    IntegrationNutrients();
                    IntegrationCrop();
                    
                    /* Update the number of days that the crop has grown*/
                    Crop->GrowthDay++;
                }
            }

            /* Store the daily calculations in the Grid structure */
            //Grid->crp  = Crop;
            //Grid->soil = WatBal;
            //Grid->mng  = Mng;
            //Grid->ste  = Site;

        /* Update time */
        //simTime.tm_mday++;
        //mktime(&simTime);
        }
		}
    
        Grid = Grid->next;
    }    
    
    /* Return to the beginning of the list */
     Grid = initial;
    

    /* Close the output files and free the allocated memory */
    fclose(output[0]);
    free(output);

    /* Go back to the beginning of the list */
    Grid = initial;
    Clean(Grid);

    return 0;
}


