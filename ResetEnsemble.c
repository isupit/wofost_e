#include "wofost.h"

/* ---------------------------------------------------------------*/
/*  function ResetEnsemble()                                      */
/*  Purpose: Reset the states and rates for each ensemble member, */
/*           the parameters do not change!                        */
/* ---------------------------------------------------------------*/

void ResetEnsemble(Plant *CROP, Soil *SOIL)
{
    /* Crop development has not started yet*/
    CROP->RootDepth = 0.;
    CROP->RootDepth_prev = 0.;
    CROP->DevelopmentStage = 0.;
    CROP->DaysOxygenStress = 0; // No crop development therefore no oxygen stress

    /* No initial nutrient stress */
    CROP->NutrientStress = 1.;
    CROP->NPK_Indx =1;


    /* STATES */  
    /* Set the initial growth states to zero */
    CROP->st.roots   = 0.;
    CROP->st.stems   = 0.;
    CROP->st.leaves  = 0.;
    CROP->st.storage = 0.;
    CROP->st.LAIExp  = 0.;
    CROP->st.vernalization = 0.;

    /* Set the initial nutrient states to zero*/
    CROP->N_st.leaves = 0.;
    CROP->N_st.stems  = 0.;
    CROP->N_st.roots  = 0.;
    CROP->N_st.storage= 0.;

    CROP->P_st.leaves = 0.;
    CROP->P_st.stems  = 0.;
    CROP->P_st.roots  = 0.;
    CROP->P_st.storage= 0.;

    CROP->K_st.leaves = 0.;
    CROP->K_st.stems  = 0.;
    CROP->K_st.roots  = 0.;
    CROP->K_st.storage= 0.;

    /* Set the maximum nutrient concentration to zero at initialization */
    CROP->N_st.Max_lv = 0.;
    CROP->N_st.Max_st = 0.;
    CROP->N_st.Max_ro = 0.;

    CROP->P_st.Max_lv = 0.;
    CROP->P_st.Max_st = 0.;
    CROP->P_st.Max_ro = 0.;

    CROP->K_st.Max_lv = 0.;
    CROP->K_st.Max_st = 0.;
    CROP->K_st.Max_ro = 0.;

    /* Set the initial optimal leave concentrations to zero */
    CROP->N_st.Optimum_lv = 0;
    CROP->N_st.Optimum_st = 0;

    CROP->P_st.Optimum_lv = 0;
    CROP->K_st.Optimum_st = 0;

    CROP->K_st.Optimum_lv = 0;
    CROP->K_st.Optimum_st = 0;

    /* No nutrient stress at initialization */
    CROP->NPK_Indx  = 1.;
    CROP->N_st.Indx = 1.;
    CROP->P_st.Indx = 1.;
    CROP->K_st.Indx = 1.;

     /* Set the initial uptake states to zero*/
    CROP->N_st.Uptake    = 0.;
    CROP->N_st.Uptake_lv = 0.;
    CROP->N_st.Uptake_st = 0.;
    CROP->N_st.Uptake_ro = 0.;

    CROP->P_st.Uptake    = 0.;
    CROP->P_st.Uptake_lv = 0.;
    CROP->P_st.Uptake_st = 0.;
    CROP->P_st.Uptake_ro = 0.;

    CROP->K_st.Uptake    = 0.;
    CROP->K_st.Uptake_lv = 0.;
    CROP->K_st.Uptake_st = 0.;
    CROP->K_st.Uptake_ro = 0.;

    /* No nutrient losses at initialization */
    CROP->N_st.death_lv = 0.;
    CROP->N_st.death_st = 0.;
    CROP->N_st.death_ro = 0.;

    CROP->P_st.death_lv = 0.;
    CROP->P_st.death_st = 0.;
    CROP->P_st.death_ro = 0.;

    CROP->K_st.death_lv = 0.;
    CROP->K_st.death_st = 0.;
    CROP->K_st.death_ro = 0.;
    
    /* Set state variables of the water balance are set to zero. */   
    SOIL->st.EvapWater         = 0.;
    SOIL->st.EvapSoil          = 0.;
    SOIL->st.Infiltration      = 0.;
    SOIL->st.Irrigation        = 0.;
    SOIL->st.Loss              = 0.;
    SOIL->st.Moisture          = 0.;
    SOIL->st.MoistureLOW       = 0.;
    SOIL->st.Percolation       = 0.;
    SOIL->st.Rain              = 0.;
    SOIL->st.RootZoneMoisture  = 0.;
    SOIL->st.Runoff            = 0.;
    SOIL->st.SurfaceStorage    = 0.;
    SOIL->st.Transpiration     = 0.;
    SOIL->st.WaterRootExt      = 0.;
    
    SOIL->rt.EvapWater         = 0.;
    SOIL->rt.EvapSoil          = 0.;   
    SOIL->rt.Infiltration      = 0.;
    SOIL->rt.Irrigation        = 0.;
    SOIL->rt.Loss              = 0.;
    SOIL->rt.Moisture          = 0.;
    SOIL->rt.MoistureLOW       = 0.;
    SOIL->rt.Percolation       = 0.;
    SOIL->rt.RootZoneMoisture  = 0.;
    SOIL->rt.Runoff            = 0.;
    SOIL->rt.SurfaceStorage    = 0.;
    SOIL->rt.WaterRootExt      = 0.;
    SOIL->rt.RootZoneMoisture  = 0.;
}