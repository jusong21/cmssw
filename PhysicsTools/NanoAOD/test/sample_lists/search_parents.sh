#dasgoclient --query="dataset=/*hdamp*/*SUL*NanoAOD*v9*/NANO*"
while read line; do
    echo "$line" >> tmp.txt
    DATASET_TYPE=${line:(-4)}
    if [ ${DATASET_TYPE} == "USER" ]; then
        echo "Private Datasets!"
        dasgoclient --query="parent dataset=$line instance=prod/phys03" >> tmp.txt
	else
    	dasgoclient --query="parent dataset=$line" >> tmp.txt
	fi
done < $1
