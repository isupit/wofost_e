rom numpy import *
from scipy.io.netcdf import *

f=open('NL1.975')
f2=open('NL1.976')
f3=open('NL1.977')
N_ensembles=2

qout=[]
precipout=[]
windout=[]
tminout=[]
tmaxout=[]
swout=[]

a=f.readlines()
for i in a:
    if i[0]!="*" and len(i.split())==9:
        qout.append(float(i.split()[6]))
        tminout.append(float(i.split()[4]))
        tmaxout.append(float(i.split()[5]))
        precipout.append(float(i.split()[8]))
        swout.append(float(i.split()[3]))
        windout.append(float(i.split()[7]))
f.close()
a=f2.readlines()
for i in a:
    if i[0]!="*" and len(i.split())==9:
        qout.append(float(i.split()[6]))
        tminout.append(float(i.split()[4]))
        tmaxout.append(float(i.split()[5]))
        precipout.append(float(i.split()[8]))
        swout.append(float(i.split()[3]))
        windout.append(float(i.split()[7]))
f.close()
for i in range(732-len(qout)):
    qout.append(qout[-1])
    precipout.append(precipout[-1])
    swout.append(swout[-1])
    tmaxout.append(tmaxout[-1])
    tminout.append(tminout[-1])
    windout.append(windout[-1])

q=array(qout)
wind=array(windout)
tmin=array(tminout)
tmax=array(tmaxout)
sw=array(swout)
precip=array(precipout)

qout=[]
precipout=[]
windout=[]
tminout=[]
tmaxout=[]
swout=[]

for i in range(N_ensembles):
    qout.append(q)
    tminout.append(tmin+1*i)
    windout.append(wind)
    tmaxout.append(tmax-1*i)
    swout.append(sw)
    precipout.append(precip)

q=array(qout)
wind=array(windout)
tmin=array(tminout)
tmax=array(tmaxout)
sw=array(swout)
precip=array(precipout)


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


write_output(qout,tminout,tmaxout,windout,swout,precipout,'ensembles.nc')
