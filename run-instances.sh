#!/bin/bash

RUN=./app.out 

INPUT_DIR=instances
OUTPUT_DIR=results

LIMIT_TIME=360

NUM_INSTANCES=15
SMALL_N=14
MEDIUM_N=28
LARGE_N=56


# mkdir -p $OUTPUT_DIR/small
# mkdir -p $OUTPUT_DIR/medium
# mkdir -p $OUTPUT_DIR/large

# echo "Executando instâncias pequenas..."

# for i in $(seq 1 $NUM_INSTANCES); do
#     INSTANCE=$INPUT_DIR/small/small_${SMALL_N}_$i.txt
    
#     OUT_FILE=$OUTPUT_DIR/small/small_${SMALL_N}_$i.txt
    
#     echo "Rodando $INSTANCE"
    
#     time $RUN -f $INSTANCE -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX $LIMIT_TIME -output $OUT_FILE
# done

LIMIT_TIME=1800
echo "Executando instâncias médias..."

for i in $(seq 1 $NUM_INSTANCES); do
    INSTANCE=$INPUT_DIR/medium/medium_${MEDIUM_N}_$i.txt
    
    OUT_FILE=$OUTPUT_DIR/medium/medium_${MEDIUM_N}_$i.txt
    
    echo "Rodando $INSTANCE"
    
    time $RUN -f $INSTANCE $OUT_FILE -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX $LIMIT_TIME -output $OUT_FILE
done


LIMIT_TIME=3600
echo "Executando instâncias grandes..."

for i in $(seq 1 $NUM_INSTANCES); do
    INSTANCE=$INPUT_DIR/large/large_${LARGE_N}_$i.txt
    
    OUT_FILE=$OUTPUT_DIR/large/large_${LARGE_N}_$i.txt
    
    echo "Rodando $INSTANCE"
    
    time $RUN -f $INSTANCE $OUT_FILE -K 2 -T 10 -maxNotImproviment 3000 -seed 13 -TMAX $LIMIT_TIME -output $OUT_FILE
done