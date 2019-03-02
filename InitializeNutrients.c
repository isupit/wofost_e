#include <stdio.h>
#include <stdlib.h>
#include "wofost.h"
#include "extern.h"

/* --------------------------------------------------------------------*/
/*  function NutrientsInitialize()                                     */
/*  Purpose: Initialization of nutrient parameters                     */
/* --------------------------------------------------------------------*/

void InitializeNutrients()
{
    float day_fl;
    
    /* Initial maximum N concentration in plant organs per kg biomass [kg N kg-1 dry biomass]   */
    Crop->N_st.Max_lv = Afgen(Crop->prm.N_MaxLeaves, &(Crop->DevelopmentStage));
    Crop->N_st.Max_st = Crop->prm.N_MaxStems * Crop->N_st.Max_lv;
    Crop->N_st.Max_ro = Crop->prm.N_MaxRoots * Crop->N_st.Max_lv;
    Crop->N_st.Max_so = 0.;
        
    /* Initial maximum N concentration in plant organs [kg N ]           */
    Crop->N_st.leaves = Crop->N_st.Max_lv * Crop->st.leaves;
    Crop->N_st.stems  = Crop->N_st.Max_st * Crop->st.stems;
    Crop->N_st.roots  = Crop->N_st.Max_ro * Crop->st.roots;
    Crop->N_st.storage = 0.;
       
    /* Initial maximum P concentration in plant organs per kg biomass [kg N kg-1 dry biomass]   */
    Crop->P_st.Max_lv = Afgen(Crop->prm.P_MaxLeaves, &(Crop->DevelopmentStage));
    Crop->P_st.Max_st  = Crop->prm.P_MaxStems * Crop->P_st.Max_lv;
    Crop->P_st.Max_ro  = Crop->prm.P_MaxRoots * Crop->P_st.Max_lv;
    Crop->P_st.Max_so = 0.;
           
    /* Initial maximum P concentration in plant organs [kg N ] */
    Crop->P_st.leaves = Crop->P_st.Max_lv * Crop->st.leaves;
    Crop->P_st.stems  = Crop->P_st.Max_st * Crop->st.stems;
    Crop->P_st.roots  = Crop->P_st.Max_ro * Crop->st.roots;
    Crop->P_st.storage = 0.;
                  
    /* Initial maximum K concentration in plant organs per kg biomass [kg N kg-1 dry biomass]    */
    Crop->K_st.Max_lv = Afgen(Crop->prm.K_MaxLeaves, &(Crop->DevelopmentStage));
    Crop->K_st.Max_st  = Crop->prm.K_MaxStems * Crop->K_st.Max_lv;
    Crop->K_st.Max_ro  = Crop->prm.K_MaxRoots * Crop->K_st.Max_lv;
    Crop->K_st.Max_so = 0.;
           
    /* Initial maximum k concentration in plant organs [kg N ] */
    Crop->K_st.leaves = Crop->K_st.Max_lv * Crop->st.leaves;
    Crop->K_st.stems  = Crop->K_st.Max_st * Crop->st.stems;
    Crop->K_st.roots  = Crop->K_st.Max_ro * Crop->st.roots;
    Crop->K_st.storage = 0.;
        
    
    day_fl = (float)Day;
    
     /* Set the soil nutrient states */
    Site->st_N_tot = Afgen(Mng->N_Fert_table, &day_fl) * Afgen(Mng->N_Uptake_frac, &day_fl) + Mng->N_Mins;
    Site->st_P_tot = Afgen(Mng->P_Fert_table, &day_fl) * Afgen(Mng->P_Uptake_frac, &day_fl) + Mng->P_Mins;
    Site->st_K_tot = Afgen(Mng->K_Fert_table, &day_fl) * Afgen(Mng->K_Uptake_frac, &day_fl) + Mng->K_Mins;

    Site->st_N_mins = Mng->N_Mins;
    Site->st_P_mins = Mng->P_Mins;
    Site->st_K_mins = Mng->K_Mins;
        
}     
