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
plant_end=int(plant_end)+100
print plant_start,plant_end,plantdoy,cropyield,cropfraction

def simulation(t_em,t1,t2):
    f=open('list.txt','w')
    for i in range(plant_start,plant_end):
        f.write('data/ '+cropstringstemp[cropindex]+' ec1.new manage-1.dat site-3.sit 1975-1-1 NL '+str(i)+' 0 31 1 '+str(t_em)+' '+str(t1)+' '+str(t2)+'\n')
    f.close()
    subprocess.check_output(['./wofost'])
    
def get_lai_correlation(lai,lai_m,lai_m_doy):
    anom=[]
    maxlai=0
    date_max_lai=0
    ks=lai.keys()
    ks.sort()
    for k in ks:
        if lai[k]>maxlai:
            maxlai=lai[k]
            date_max_lai=k
    for k in ks:
        if lai[k]>0.5*maxlai:
            halfmaxlai=k
            break
    for k in ks:
        if lai[k]<0.5*maxlai and k>date_max_lai:
            halfmaxlai2=k
            break
    for i in range(len(lai_m_doy)):
        d=lai_m_doy[i]
        #if d>halfmaxlai and d<halfmaxlai2: #date_max_lai:
        if d>halfmaxlai and d<date_max_lai:
            if d in lai.keys():
                anom.append(abs(lai_m[i]-lai[d]))
    return mean(anom)

def check_simulated_lai(plot):
    f=open('ensembles_output.txt')
    a=f.readlines()
    
    lai_model={}
    dmp_model={}
    production_model={}
    lai_real={}
    dmp_real={}
    start_date_model={}
    
    for i in range(31):
        lai_model[i]=[]
        dmp_model[i]=[]
        production_model[i]=[]
        start_date_model[i]=[]
        for j in range(int(a[-1].split()[1])+1):
            lai_model[i].append([])
            dmp_model[i].append([])
            production_model[i].append([])
            start_date_model[i].append([])
    
    for i in a:
        lai_model[int(i.split()[0])][int(i.split()[1])].append(float(i.split()[6]))
        start_date_model[int(i.split()[0])][int(i.split()[1])].append(int(i.split()[2]))
        dmp_model[int(i.split()[0])][int(i.split()[1])].append(float(i.split()[3])+float(i.split()[4])+float(i.split()[5]))
        production_model[int(i.split()[0])][int(i.split()[1])].append(float(i.split()[5]))
    f.close()
    production=[]
    for i in range(len(production_model[0])):
        temp=[]
        for j in range(len(production_model)):
            temp.append(max(production_model[j][i]))
        production.append(mean(temp))

    
    f=open('data/lai.txt')
    a=f.readlines()
    for i in a:
        k=i.split()[0][4:8]
        if k=='0229':
            k='0228'
        if k in lai_real:
            lai_real[k].append(float(i.split()[1]))
        else:
            lai_real[k]=[float(i.split()[1])]
    f.close()
    
    f=open('data/dmp.txt')
    a=f.readlines()
    for i in a:
        if i.split()[0][4:]=='0229':
            i[4:8]='0228'
        if i.split()[0][4:] in dmp_real:
            dmp_real[i.split()[0][4:]].append(float(i.split()[1]))
        else:
            dmp_real[i.split()[0][4:]]=[float(i.split()[1])]
    f.close()
    
    lai={}
    laiplot=[]
    dmp=[]
    lai_doy=[]
    dmp_doy=[]
    dmp_keys=dmp_real.keys()
    lai_keys=lai_real.keys()
    lai_keys.sort()
    dmp_keys.sort()
    for k in lai_keys:
        doy=(datetime.date(2018,int(k[:2]),int(k[2:]))-datetime.date(2018,1,1)).days
        lai_doy.append(doy)
        lai[doy]=mean(lai_real[k])
        laiplot.append(mean(lai_real[k]))
    for k in dmp_keys:
        dmp.append(mean(dmp_real[k]))
        doy=(datetime.date(2018,int(k[:2]),int(k[2:]))-datetime.date(2018,1,1)).days
        dmp_doy.append(doy)
    lai_errors=zeros([len(start_date_model),len(start_date_model[0])])
    for i in range(len(start_date_model)):
        for j in range(len(start_date_model[0])):
            lai_errors[i,j]=get_lai_correlation(lai,lai_model[i][j],start_date_model[i][j])
    err=mean(lai_errors,0)
    if plot:
        dmptemp=[]
        laitemp=[]
        for d in range(len(dmp_model[0][list(err).index(nanmin(err))])):
                dmpsum=0
                laisum=0
                for i in range(31):
                    try:
                        dmpsum+=dmp_model[i][list(err).index(nanmin(err))][d]
                        laisum+=lai_model[i][list(err).index(nanmin(err))][d]
                    except:
                        pass
                dmptemp.append(dmpsum/31.0)
                laitemp.append(laisum/31.0)
        dmpp=array(dmptemp)
        laip=array(laitemp)
        dmpplot=0.2*(dmpp[2:]-dmpp[:-2])
        plt.subplot(221)
        plt.plot(arange(len(lai_errors[0]))+start_date_model[0][0][0],mean(lai_errors,0))
        plt.xlim([120,210])
        plt.xlabel('Planting Doy of year')
        plt.ylabel('LAI Error')
        plt.subplot(223)
        plt.plot(arange(len(lai_errors[0]))+start_date_model[0][0][0],production)
        plt.xlim([120,210])
        plt.xlabel('Planting Doy of year')
        plt.ylabel('Yield')
        plt.subplot(222)
        plt.plot(lai_doy,laiplot)
        plt.plot(start_date_model[0][list(err).index(nanmin(err))],laip)
        #plt.plot(start_date_model[0][list(err).index(nanmin(err))],lai_model[0][list(err).index(nanmin(err))])
        plt.xlabel('Doy of year')
        plt.ylabel('LAI')
        plt.legend(['Model','Measurement'])
        plt.subplot(224)
        plt.plot(dmp_doy,dmp)
        plt.plot(start_date_model[0][list(err).index(nanmin(err))][2:],dmpplot)
        plt.ylim([0,max(dmp)])
        plt.xlabel('Doy of year')
        plt.ylabel('DMP')
        plt.legend(['Model','Measurement'])
        plt.show()
    return list(err).index(nanmin(err))+start_date_model[0][0][0],nanmin(err),list(production).index(nanmax(production))+start_date_model[0][0][0],nanmax(production),production[list(err).index(nanmin(err))]

def do_one_step(good,done,tem,t1,t2,stepsize):
    if not (tem,t1,t2) in done.keys():
        simulation(tem,t1,t2)
        a,b,c,d,e=check_simulated_lai(0)
        done[(tem,t1,t2)]=b
        if abs(a-c)<5:
            good[(tem,t1,t2)]=b
    best=10000
    superbest=10000
    current=done[(tem,t1,t2)]
    best_config=(-999,-999,-999)
    super_best_config=(-999,-999,-999)
    for tem_temp in range(tem-stepsize,tem+2*stepsize,stepsize):
        if not (tem_temp,t1,t2) in done.keys():
            simulation(tem_temp,t1,t2)
            a,b,c,d,e=check_simulated_lai(0)
            done[(tem_temp,t1,t2)]=b
            print tem_temp,t1,t2,a,c,b,d,e
            if b<best and b<current and d>cropyield*1000:
                best=b
                best_config=(tem_temp,t1,t2)
            if abs(a-c)<5:
                good[(tem_temp,t1,t2)]=b
                if b<best and b<current and d>cropyield*1000:
                    superbest=b
                    super_best_config=(tem_temp,t1,t2)
    for t1_temp in range(t1-stepsize,t1+2*stepsize,stepsize):
        if not (tem,t1_temp,t2) in done.keys():
            simulation(tem,t1_temp,t2)
            a,b,c,d,e=check_simulated_lai(0)
            done[(tem,t1_temp,t2)]=b
            print tem,t1_temp,t2,a,c,b,d,e
            if b<best and b<current and d>cropyield*1000:
                best=b
                best_config=(tem,t1_temp,t2)
            if abs(a-c)<5:
                good[(tem,t1_temp,t2)]=b
                if b<best and b<current and d>cropyield*1000:
                    superbest=b
                    super_best_config=(tem,t1_temp,t2)
    for t2_temp in range(t2-stepsize,t2+2*stepsize,stepsize):
        if not (tem,t1,t2_temp) in done.keys():
            simulation(tem,t1,t2_temp)
            a,b,c,d,e=check_simulated_lai(0)
            done[(tem,t1,t2_temp)]=b
            print tem,t1,t2_temp,a,c,b,d,e
            if b<best and b<current and d>cropyield*1000:
                best=b
                best_config=(tem,t1,t2_temp)
            if abs(a-c)<5:
                good[(tem,t1,t2_temp)]=b
                if b<best and b<current and d>cropyield*1000:
                    superbest=b
                    super_best_config=(tem,t1,t2_temp)
    if super_best_config[0]>0:
        return super_best_config,done,good
    return best_config,done,good

def get_tuned_temps(tem,t1,t2):
    stepsize=100
    done={}
    good={}
    simulation(tem,t1,t2)
    a,b,c,d,e=check_simulated_lai(0)
    done[(tem,t1,t2)]=b
    if abs(a-c)<5:
        good[(tem,t1,t2)]=b
    while stepsize>10:
        while 1:
            new_config,done,good=do_one_step(good,done,tem,t1,t2,stepsize)
            old_config=(tem,t1,t2)
            if new_config[0]<0:
                print "No better solution found\n"
                break
            print new_config,done[new_config]
            tem=new_config[0]
            t1=new_config[1]
            t2=new_config[2]
        stepsize=int(stepsize/2.0)
        tem=old_config[0]
        t1=old_config[1]
        t2=old_config[2]
    return good,done




