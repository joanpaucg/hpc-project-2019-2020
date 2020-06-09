#!/bin/bash

## Specifies the interpreting shell for the job.
#$ -S /bin/bash

## Specifies that all environment variables active within the qsub utility be exported to the context of the job.
#$ -V

## Specifies the parallel environment
#$ -pe smp 4

## Execute the job from the current working directory.
#$ -cwd

## The  name  of  the  job.
#$ -N threads_16_1000000_static_6000x4000

##send an email when the job ends
#$ -m e

##email addrees notification
#$ -M jcg23@alumnes.udl.cat

#$ -o static_16_1000000_6000_4000/ostatic_16_1000000_6000_4000
#$ -e static_16_1000000_6000_4000/estatic_16_1000000_6000_4000
##Passes an environment variable to the job
#$ -v  OMP_NUM_THREADS=16

## In this line you have to write the command that will execute your application.
./mandelbrot 6000 4000 1000000



