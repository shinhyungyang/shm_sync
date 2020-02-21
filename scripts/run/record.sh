BENCHNAME="Taubenfeld"
E=100
ITER=10000000
TOPLEVEL=`git rev-parse --show-toplevel `
# identify whether it belongs to locks or nonblocking or tsx
BENCHDIR="${TOPLEVEL}/benchmark/locks/${BENCHNAME}" # TBD

if [ ! -d ${BENCHDIR} ];
then
  echo "ERROR: Failed to find ${BENCHNAME}"
  exit 1
fi
export LC_TIME=C
DATE=`date +%B%d_%Y_%T`
RECORD_DIR="${TOPLEVEL}/data/${DATE}"
mkdir -p ${RECORD_DIR}
echo ${BENCHDIR} > ${RECORD_DIR}/README # README info TBD

for a in {1..28} #TBD
do
  OUTFILE=${RECORD_DIR}/"${a}_${E}.out"
  /opt/python/3.6.5/bin/python3.6 ./run.py -e ${E} -t ${a} -i ${ITER} ${BENCHNAME}  > ${OUTFILE} # TBD
done
