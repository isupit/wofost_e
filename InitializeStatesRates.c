#include "wofost.h"
#include <stdlib.h> 

/* ---------------------------------------------------------------*/
/*  function ResetEnsemble()                                      */
/*  Purpose: Reset the states and rates for each ensemble member, */
/*           the parameters do not change!                        */
/* ---------------------------------------------------------------*/

void InitializeStatesRates()
{
    Green *LeaveProperties = NULL;
    
    Crop->TSumEmergence = 0.;
    Crop->Emergence = 0; // Will be set in InitializeCrop
    Crop->GrowthDay = 0; // Will be set in InitializeCrop
    
    /* Crop development has not started yet*/
    Crop->RootDepth = 0.;
    Crop->RootDepth_prev = 0.;
    Crop->DevelopmentStage = 0.;
    Crop->DaysOxygenStress = 0; // No crop development therefore no oxygen stress

    /* No initial nutrient stress */
    Crop->NutrientStress = 1.;
    Crop->NPK_Indx =1;

    /* STATES */  
    /* Set the initial growth states to zero */
    Crop->st.roots   = 0.;
    Crop->st.stems   = 0.;
    Crop->st.leaves  = 0.;
    Crop->st.storage = 0.;
    Crop->st.LAIExp  = 0.;
    Crop->st.vernalization = 0.;
    
    Crop->N_st.Indx = 1.;
    Crop->P_st.Indx = 1.;
    Crop->K_st.Indx = 1.;
    
    /* Set the maximum nutrient concentration to zero */
    Crop->N_st.Max_lv = 0.;
    Crop->N_st.Max_ro = 0.;
    Crop->N_st.Max_st = 0.;
 
    Crop->P_st.Max_lv = 0.;
    Crop->P_st.Max_ro = 0.;
    Crop->P_st.Max_st = 0.;
 
    Crop->K_st.Max_lv = 0.;
    Crop->K_st.Max_ro = 0.;
    Crop->K_st.Max_st = 0.;
    
    /* Set the optimum concentration to zero */
    Crop->N_st.Optimum_lv = 0.;
    Crop->N_st.Optimum_st = 0.;
    
    Crop->P_st.Optimum_lv = 0.;
    Crop->P_st.Optimum_st = 0.;
    
    Crop->K_st.Optimum_lv = 0.;
    Crop->K_st.Optimum_st = 0.;
    
    /* Set the nutrient uptake states to zero */
    Crop->N_st.Uptake    = 0.;
    Crop->N_st.Uptake_lv = 0.;
    Crop->N_st.Uptake_st = 0.;
    
    Crop->P_st.Uptake    = 0.;
    Crop->P_st.Uptake_lv = 0.;
    Crop->P_st.Uptake_st = 0.;
    
    Crop->K_st.Uptake    = 0.;
    Crop->K_st.Uptake_lv = 0.;
    Crop->K_st.Uptake_st = 0.;
    
    /* Set the death states to zero */
    Crop->N_st.death_lv = 0.;
    Crop->N_st.death_ro = 0.;
    Crop->N_st.death_st = 0.;
    
    Crop->P_st.death_lv = 0.;
    Crop->P_st.death_ro = 0.;
    Crop->P_st.death_st = 0.;
    
    Crop->K_st.death_lv = 0.;
    Crop->K_st.death_ro = 0.;
    Crop->K_st.death_st = 0.;
    
    /* Set the initial nutrient states to zero*/
    Crop->N_st.leaves = 0.;
    Crop->N_st.stems  = 0.;
    Crop->N_st.roots  = 0.;
    Crop->N_st.storage= 0.;

    Crop->P_st.leaves = 0.;
    Crop->P_st.stems  = 0.;
    Crop->P_st.roots  = 0.;
    Crop->P_st.storage= 0.;

    Crop->K_st.leaves = 0.;
    Crop->K_st.stems  = 0.;
    Crop->K_st.roots  = 0.;
    Crop->K_st.storage= 0.;
    
    /* Free the leaves of this node. Loop until the last element in the */
    /* list and free each node */
    while (Crop->LeaveProperties)
    {
        LeaveProperties = Crop->LeaveProperties;
        Crop->LeaveProperties = Crop->LeaveProperties->next; 
        free(LeaveProperties);
        LeaveProperties = NULL;
    }
    free(Crop->LeaveProperties);
    LeaveProperties = NULL;
    
    /* Set state variables of the water balance are set to zero. */   
    
    WatBal->DaysSinceLastRain    = 0.;
    WatBal->WaterStress          = 1.;
    WatBal->SoilMaxRootingDepth  = 0.;
    
    WatBal->st.EvapWater         = 0.;
    WatBal->st.EvapSoil          = 0.;
    WatBal->st.Infiltration      = 0.;
    WatBal->st.Irrigation        = 0.;
    WatBal->st.Loss              = 0.;
    WatBal->st.Moisture          = 0.;
    WatBal->st.MoistureLOW       = 0.;
    WatBal->st.Percolation       = 0.;
    WatBal->st.Rain              = 0.;
    WatBal->st.RootZoneMoisture  = 0.;
    WatBal->st.Runoff            = 0.;
    WatBal->st.SurfaceStorage    = 0.;
    WatBal->st.Transpiration     = 0.;
    WatBal->st.WaterRootExt      = 0.;
       
    /* Set the soil nutrient states rates to zero */
    Site->st_N_tot = 0.;
    Site->st_P_tot = 0.;
    Site->st_K_tot = 0.;

    Site->st_N_mins = 0.;
    Site->st_P_mins = 0.;
    Site->st_K_mins = 0.;   
    
}