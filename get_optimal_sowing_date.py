import subprocess
from scipy.stats import scoreatpercentile
from scipy.io.netcdf import *
from numpy import *
import matplotlib.pyplot as plt
import datetime
import os
import sys

datadir='/media/obbe/obbe-personal/data/WFDEI/'
#zfile=netcdf_file(datadir+'WFD-land-lat-long-z.nc','r')
#lat=float(sys.argv[1])
#lon=float(sys.argv[2])
lat=12
lon=0
cropindex=4

#Crops:
#cropindex=int(sys.argv[3])
cropstrings=['sorghum','chickpea','cotton','gr_nut','maiz','millet','mungbean','ricehy_8','sorghum','soybean','sugrcane','swpotato','tobacco','whtfra']
cropstringstemp=['sorghu.cab','whtfra.w41','maiz.w41','cotton.w41','mag201.cab']
cropcalstrings=['Sorghum','none','Cotton','Groundnuts','Maize','Millet','none','Rice','Sorghum','Soybeans','none','Sweet.Potatoes','none','Wheat']
cropfracstring=['sorghum','chickpea','cotton','groundnut','maize','millet','none','rice','sorghum','soybean','sugarcane','sweetpotato','tobacco','wheat']


# Timer data:
timer_file=netcdf_file('/media/obbe/obbe-personal/data/crop_calendar/'+cropcalstrings[cropindex]+'.crop.calendar.fill.nc')

# Yield data:
crop_fraction_file=netcdf_file('/media/obbe/obbe-personal/data/agriculture/'+cropfracstring[cropindex]+'_5min.nc')
crop_fraction=crop_fraction_file.variables['cropdata'][0,0]
crop_yield=crop_fraction_file.variables['cropdata'][0,1]
croplats=crop_fraction_file.variables['latitude'][:]
croplons=crop_fraction_file.variables['longitude'][:]


def get_timer_data(lat,lon,timer_file):
    plant=timer_file.variables['plant'][get_closest_index(timer_file.variables['latitude'][:],lat),get_closest_index(timer_file.variables['longitude'][:],lon)]
    plant_start=timer_file.variables['plant.start'][get_closest_index(timer_file.variables['latitude'][:],lat),get_closest_index(timer_file.variables['longitude'][:],lon)]
    plant_end=timer_file.variables['plant.end'][get_closest_index(timer_file.variables['latitude'][:],lat),get_closest_index(timer_file.variables['longitude'][:],lon)]
    return plant,plant_start,plant_end


def get_closest_index(array,value):
    idx = (abs(array-value)).argmin()
    return idx

def get_height(latidx,lonidx):
    return 100
    for i in range(len(zfile.variables['Z'][:])):
        if zfile.variables['Grid_lat'][i]==latidx and zfile.variables['Grid_lon'][i]==lonidx:
            return zfile.variables['Z'][i]

cropfraction=amax(crop_fraction[get_closest_index(croplats,lat+0.5):get_closest_index(croplats,lat-0.5),get_closest_index(croplons,lon-0.5):get_closest_index(croplons,lon+0.5)])
cropyield=amax(crop_yield[get_closest_index(croplats,lat+0.5):get_closest_index(croplats,lat-0.5),get_closest_index(croplons,lon-0.5):get_closest_index(croplons,lon+0.5)])
plantdoy,plant_start,plant_end=get_timer_data(lat,lon,timer_file)
plant_end=int(plant_end+100)
print plant_start,plant_end,plantdoy,cropyield,cropfraction
f=open('list.txt','w')
for i in range(plant_start,plant_end):
    f.write('data/ '+cropstringstemp[cropindex]+' ec1.new manage-1.dat site-3.sit 1975-1-1 NL '+str(i)+' 0 31 0\n')
f.close()
subprocess.check_output(['./wofost'])
f=open('ensembles_output.txt')
a=f.readlines()
yields=[]
lai=[]
stems=[]
for i in range(int(plant_end-plant_start)):
    yields.append([])
    stems.append([])
    lai.append([])
for i in a:
    num=int(i.split()[1])
    yields[num].append(float(i.split()[5]))
    lai[num].append(float(i.split()[4]))
    stems[num].append(float(i.split()[3]))

means=[mean(i) for i in yields]
print [d for (i,d) in zip(means,range(plant_start,plant_end)) if i==max(means)],max(means)
plt.plot(range(plant_start,plant_end),[mean(i) for i in yields],'k')
plt.plot(range(plant_start,plant_end),[scoreatpercentile(i,25) for i in yields],'k--')
plt.plot(range(plant_start,plant_end),[scoreatpercentile(i,75) for i in yields],'k--')
plt.plot(range(plant_start,plant_end),[std(i) for i in yields],'k-.')
plt.plot(range(plant_start,plant_end),[mean(i) for i in lai],'r')
plt.plot(range(plant_start,plant_end),[mean(i) for i in stems],'b')
#plt.plot(range(plant_start,plant_end),[mean(i) for i in stems],'r')
#plt.plot(range(plant_start,plant_end),[mean(i) for i in lai],'b')

