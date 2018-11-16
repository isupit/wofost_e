#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "wofost.h"
#include "extern.h"
#include <time.h>
#include <netcdf.h>

int main() {
    FILE *ifp;
    FILE **output;
    FILE *result;
    
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
        
	if (N_ensembles>max_N_ensembles) {max_N_ensembles=N_ensembles;}
        /* count the number of output files */
        /* number is the index number of the list of file pointers */
        
        if (initial == NULL)
        {
            Grid = initial =  malloc(sizeof(SimUnit));
        }
        else
        {
            Grid->next = malloc(sizeof(SimUnit));
            Grid = Grid->next;
        }
        
        GetCropData(Grid->crp   = malloc(sizeof(Plant)), cropfile); 
        GetSiteData(Grid->ste   = malloc(sizeof(Field)), sitefile);
        GetManagement(Grid->mng = malloc(sizeof(Management)), management);
        GetSoilData(Grid->soil  = malloc(sizeof(Soil)), soilfile);
        
        strcpy(Grid->name,cf);              // Crop file name
        Grid->start     = Start;            // Start day (=day number)
        Grid->ensembles = N_ensembles;
        Grid->file      = count++;          // number of elements in Grid carousel
        Grid->emergence = Emergence;        // Start the simulations at emergence (1) or at sowing (0)
        Grid->next      = NULL;
        
        /* overload the TSUM1, TSUM2 and TSUME */
        if (Tsum1input > 0) Grid->crp->prm.TempSum1 = Tsum1input;
        if (Tsum2input > 0) Grid->crp->prm.TempSum2 = Tsum2input;
        if (Tsumemergenceinput > 0) Grid->crp->prm.TSumEmergence = Tsumemergenceinput;              
    }
    
    /* Close the input file */
    fclose(ifp);

    /* Allocate memory for the meteorogical ensembles */
    float* q_ensembles      = malloc(sizeof(float) * 732 * max_N_ensembles);
    float* wind_ensembles   = malloc(sizeof(float) * 732 * max_N_ensembles);
    float* tmin_ensembles   = malloc(sizeof(float) * 732 * max_N_ensembles);
    float* tmax_ensembles   = malloc(sizeof(float) * 732 * max_N_ensembles);
    float* sw_ensembles     = malloc(sizeof(float) * 732 * max_N_ensembles);
    float* precip_ensembles = malloc(sizeof(float) * 732 * max_N_ensembles);

    /* Read ensemble data */
    GetEnsembles(q_ensembles,tmin_ensembles,tmax_ensembles,sw_ensembles,wind_ensembles,precip_ensembles,max_N_ensembles);

    
    /* Get the meteodata */
    //GetMeteoData(path, dateString, place);
    int i;
    int ens=0;
    
    /* Allocate memory for the file pointers */
    output = malloc(sizeof(**output) * --count);
    
    /* Open the output file */
    result = fopen("ensembles_output.txt", "w");
    
    /* Go back to the beginning of the list */
    Grid = initial;
    while (Grid)
    {
        /* Get data, states and rates from the Grid structure and */
        /* put them in the place holders */
        Crop      = Grid->crp;
        WatBal    = Grid->soil;
        Mng       = Grid->mng;
        Site      = Grid->ste;
        Start     = Grid->start;
        
        //printf("Crop: %f %f %f\n",Grid->crp->prm.TSumEmergence,Grid->crp->prm.TempSum1,Grid->crp->prm.TempSum2);
        
	for (ens=0; ens<Grid->ensembles; ens++)
        {    
            /* set the Emergence for each member */
            Emergence = Grid->emergence;
            
            /*  reset the states and rates  */
            InitializeStatesRates();
            
            /* Set the ensemble meteo data*/
            for (i=1;i<732;i++)
            {
		Tmin[i]=tmin_ensembles[ens*732+i];
		Tmax[i]=tmax_ensembles[ens*732+i];
		Radiation[i]=1000.0*sw_ensembles[ens*732+i];
		Rain[i]=0.1*precip_ensembles[ens*732+i];
		Vapour[i]=10.0*q_ensembles[ens*732+i];
		Windspeed[i]=wind_ensembles[ens*732+i];
            }


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
                        printf("%3d %4d %4d\n", ens, Grid->file, Day);
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
                        /* Calculate the evapotranspiration */
                        EvapTra();
                        
                        /* Set the rates to zero*/
                        RatesToZero();
                        
                        /* Rate calculations */
                        RateCalulationWatBal();
                        RateCalcultionNutrients();
                        RateCalculationCrop();

                        /* Calculate LAI and DVS */
                        Crop->st.LAI = LeaveAreaIndex();
                        Crop->DevelopmentStage = GetDevelopmentStage();

                        if (print_all || Crop->DevelopmentStage >= Crop->prm.DevelopStageHarvest)
                        {
                            fprintf(result,"%3d %3d %4d %7.1f %7.1f %7.1f %7.2f %7.2f %7.2f %7.3f %7.2f %7.1f\n",
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
            }
        }
        Grid = Grid->next;
    }    
    
    /* Return to the beginning of the list */
     Grid = initial;
    
    /* Close the output files and free the allocated memory */
    fclose(result);
    free(output);

    /* Go back to the beginning of the list */
    Grid = initial;
    Clean(Grid);

    /* Clean the meteo ensemble */
    free(q_ensembles);
    free(wind_ensembles);
    free(tmin_ensembles);  
    free(tmax_ensembles);
    free(sw_ensembles);    
    free(precip_ensembles);
    
    return 0;
}


