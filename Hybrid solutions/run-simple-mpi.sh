#!/bin/bash

## Specifies the interpreting shell for the job.
#$ -S /bin/bash

## Specifies that all environment variables active within the qsub utility be exported to the context of the job.
#$ -V

## Execute the job from the current working directory.
#$ -cwd

## Parallel programming environment (mpich) to instantiate and number of computing slots.
#$ -pe mpich 2

##Passes an environment variable to the job
#$ -v  OMP_NUM_THREADS=4

## The  name  of  the  job.
#$ -N jcg23-test

## Send an email at the start and the end of the job.
#$ -m be

#$ -o dynamic_4t_2p_10000_600x400


MPICH_MACHINES=$TMPDIR/mpich_machines
cat $PE_HOSTFILE | awk '{print $1":"$2}' > $MPICH_MACHINES


## In this line you have to write the command that will execute your application
mpiexec -f $MPICH_MACHINES -n $NSLOTS ./mpi_dynamic_mandelbrot 600 400 10000


rm -rf $MPICH_MACHINES
