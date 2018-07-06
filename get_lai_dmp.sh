lat=$1
lon=$2

bash /home/obbe/work/analysis/copernicus_land/get_all_LAI.sh $lat $lon | gawk '{print $1,$5/30}' > data/lai.txt
bash /home/obbe/work/analysis/copernicus_land/get_all_DMP.sh $lat $lon | gawk '{print $1,$3/100}' > data/dmp.txt

