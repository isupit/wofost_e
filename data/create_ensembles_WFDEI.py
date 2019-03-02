from numpy import *
from scipy.io.netcdf import *
import datetime
import sys
import os

N_ensembles=31
def get_closest_index(lats,lat):
        import operator
        lat_index, min_value = min(enumerate(abs(lats-lat)), key=operator.itemgetter(1))
        return lat_index
fs=os.listdir('/media/obbe/data1/work/data/WFDEI/regions/')
fs=[i for i in fs if i[-15:]=='Tmax_climate.nc']
for f in fs:
    file=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+f)
    curregion=f[:-15].split('/')[-1]
    lons=file.variables['lon'][:].copy()
    lats=file.variables['lat'][:].copy()
    curlat=float(sys.argv[1])
    curlon=float(sys.argv[2])
    if curlon>min(lons) and curlon<max(lons) and curlat>min(lats) and curlat<max(lats):
        region=curregion
        latidx=get_closest_index(curlat,lats)
        lonidx=get_closest_index(curlon,lons)
    file.close()

qfile=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+region+'Qair.nc')
tminfile=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+region+'Tmin.nc')
tmaxfile=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+region+'Tmax.nc')
swfile=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+region+'SWdown.nc')
windfile=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+region+'Wind.nc')
pfile=netcdf_file('/media/obbe/data1/work/data/WFDEI/regions/'+region+'Rainf.nc')
qout=[]
precipout=[]
windout=[]
tminout=[]
tmaxout=[]
swout=[]

for i in range(len(pfile.variables['time'][:])):
    t=str(qfile.variables['time'][i])
    d=datetime.datetime(int(t[:4]),int(t[4:6]),int(t[6:8]),12)
    doy=(d-datetime.datetime(d.year,1,1,12)).days+1
    if d.year<2012 and doy<366:
        q=(100*qfile.variables['Qair'][i,latidx,lonidx]/(0.622+0.378*qfile.variables['Qair'][i,latidx,lonidx]))
        swout.append(swfile.variables['SWdown'][i,latidx,lonidx]*24*3600/1000.0)
        tminout.append(tminfile.variables['Tair'][i,latidx,lonidx]-273.15)
        tmaxout.append(tmaxfile.variables['Tair'][i,latidx,lonidx]-273.15)
        qout.append(q)
        windout.append(windfile.variables['Wind'][i,latidx,lonidx])
        precipout.append(pfile.variables['Rainf'][i,latidx,lonidx]*24*3600)

q=[]
precip=[]
wind=[]
tmin=[]
tmax=[]
sw=[]

for i in range(N_ensembles):
    q.append(qout[365*i:365*i+732])
    tmin.append(tminout[365*i:365*i+732])
    tmax.append(tmaxout[365*i:365*i+732])
    wind.append(windout[365*i:365*i+732])
    sw.append(swout[365*i:365*i+732])
    precip.append(precipout[365*i:365*i+732])

q=array(q)
wind=array(wind)
tmin=array(tmin)
tmax=array(tmax)
sw=array(sw)
precip=array(precip)


def write_output(qout,tminout,tmaxout,windout,swout,precipout,filename):
        # open a new netCDF file for writing.
        ncfile = netcdf_file(filename,'w')

        ncfile.createDimension('ensembles',N_ensembles)
        ncfile.createDimension('time',732)

        # create time dimension (record, or unlimited dimension)
        # Define the coordinate variables. They will hold the coordinate
        # information, that is, the latitudes and longitudes.
        # Coordinate variables only given for lat and lon.

        nens = ncfile.createVariable('ensembles',dtype('float32').char,('ensembles',))
        tim = ncfile.createVariable('time',dtype('float32').char,('time',))

        # Assign units attributes to coordinate var data. This attaches a
        # text attribute to each of the coordinate variables, containing the
        # units.
        
        # write data to coordinate vars.
        nens[:] = arange(N_ensembles)
        tim[:] = arange(732)

        # create the pressure and temperature variables
        q = ncfile.createVariable('q',dtype('float32').char,('ensembles','time'))
        precip = ncfile.createVariable('precip',dtype('float32').char,('ensembles','time'))
        tmax = ncfile.createVariable('tmax',dtype('float32').char,('ensembles','time'))
        sw = ncfile.createVariable('sw',dtype('float32').char,('ensembles','time'))
        wind = ncfile.createVariable('wind',dtype('float32').char,('ensembles','time'))
        tmin = ncfile.createVariable('tmin',dtype('float32').char,('ensembles','time'))

        # write data to variables along record (unlimited) dimension.
        # same data is written for each record.
        q[:]=array(qout)
        tmin[:]=array(tminout)
        tmax[:]=array(tmaxout)
        wind[:]=array(windout)
        sw[:]=array(swout)
        precip[:]=array(precipout)

        ncfile.close()
        return 0


write_output(q,tmin,tmax,wind,sw,precip,'ensembles.nc')


