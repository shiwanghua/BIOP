ulimit -s unlimited
#chmod 777 linuxRB cmakeRB
#./cmakeRB
expID=$1

for((bits=0;bits<=7;bits++));  #���Ų���ʡ
do  
expID=$expID+1
#./cmakeRB $expID $bits
echo Experiment $expID \done. #$(expr $i \* 3 + 1); 
done