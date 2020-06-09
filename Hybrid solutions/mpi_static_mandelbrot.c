//
//  mandelbrot.c
//  
//
//  The Mandelbrot calculation is to iterate the equation
//  z = z*z + c, where z and c are complex numbers, z is initially
//  zero, and c is the coordinate of the point being tested. If
//  the magnitude of z remains less than 2 for ever, then the point
//  c is in the Mandelbrot set. In this code We write out the number of iterations
//  before the magnitude of z exceeds 2, or UCHAR_MAX, whichever is
//  smaller.//
//
//

#include <stdio.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <omp.h>

/*void color(int red, int green, int blue)
{
    fputc((char)red, stdout);
    fputc((char)green, stdout);
    fputc((char)blue, stdout);
}*/



int main (int argc, char* argv[])
{
    int rank, size;// id process and size of the communicator
    //int namelen; 
    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        // get current process id
    MPI_Comm_size (MPI_COMM_WORLD, &size);        // get number of processes
	int w = atoi(argv[1]), h = atoi(argv[2]), x, y; // width, height from arguments
	double pr, pi;
	double newRe, newIm, oldRe, oldIm;   //real and imaginary parts of new and old z
	double zoom = 1, moveX = -0.5, moveY = 0; //you can change these to zoom and change position
	int maxIterations = atoi(argv[3]);//after how much iterations the function should stop
	double start, elapsed;
	int rowsForWorker=h/size;// We got the number of rows that a Worker have to process
	int begin,end;
	
    //MPI_Get_processor_name(hostname, &namelen);   // get CPU name
	
	start = MPI_Wtime();
	
	
    //printf( "Hello world from process %d of %d (node %s)\n", rank, size, hostname );
	begin= rowsForWorker*rank;// first row to process the process
	end= rowsForWorker*rank+ rowsForWorker;//last row to process for that process
	typedef unsigned char pixelType[3];
	//pixelType *pixels=malloc(sizeof(pixelType)*h*w);
	
	FILE * sortida;// File to generate the intermediary results
	
	//printf("P6\n# CREATOR: Eric R. Weeks / mandel program\n");
    //printf("%d %d\n255\n",w,h);
	if(rank==size-1){
		end=end+h%size;// if is the last process we give the rest of the rows to process
		rowsForWorker=rowsForWorker+h%size;
	}
	pixelType *pixelsWorker=malloc(sizeof(pixelType)*rowsForWorker*w);
	//from the first row to the last row
	#pragma omp parallel for shared(pixelsWorker,moveX,moveY,zoom) private(x,y,pr,pi,newRe,newIm,oldRe,oldIm) schedule(static)
	for(y =begin ; y <end  ; y++){
		for(x = 0; x < w; x++)
        {
            //calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
            pr = 1.5 * (x - w / 2) / (0.5 * zoom * w) + moveX;
            pi = (y - h / 2) / (0.5 * zoom * h) + moveY;
            newRe = newIm = oldRe = oldIm = 0; //these should start at 0,0
            //"i" will represent the number of iterations
            int i;
            //start the iteration process
            for(i = 0; i < maxIterations; i++)
            {
                //remember value of previous iteration
                oldRe = newRe;
                oldIm = newIm;
                //the actual iteration, the real and imaginary part are calculated
                newRe = oldRe * oldRe - oldIm * oldIm + pr;
                newIm = 2 * oldRe * oldIm + pi;
                //if the point is outside the circle with radius 2: stop
                if((newRe * newRe + newIm * newIm) > 4) break;
            }
            
//            color(i % 256, 255, 255 * (i < maxIterations));
            if(i == maxIterations){
				pixelsWorker[((y%rowsForWorker)*w+x)][0]=(char)0;
				pixelsWorker[((y%rowsForWorker)*w+x)][1]=(char)0;
				pixelsWorker[((y%rowsForWorker)*w+x)][2]=(char)0;
			}
                
            else
            {
                double z = sqrt(newRe * newRe + newIm * newIm);
                int brightness = 256 * log2(1.75 + i - log2(log2(z))) / log2((double)maxIterations);
                //color(brightness, brightness, 255);
				pixelsWorker[((y%rowsForWorker)*w+x)][0]=(char)brightness;
				pixelsWorker[((y%rowsForWorker)*w+x)][1]=(char)brightness;
				pixelsWorker[((y%rowsForWorker)*w+x)][2]=(char)255;
            }
            
        }
	}
	//MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0){
		
		int proc=0;
		//fprintf(stderr, "Elapsed time: %.2lf seconds.\n", omp_get_wtime()-timeBegin);
		int i=0;
		int j=0;
		sortida= fopen("sortida.ppm","wb");
		fprintf(sortida, "P6\n# CREATOR: Eric R. Weeks / mandel program\n");
		fprintf(sortida, "%d %d\n255\n", w, h);
		
		for(proc=0;proc<size;proc++){
			if(proc!=0){
					//char c;
					
					char row[sizeof(pixelType)*w];
					pixelsWorker=malloc(sizeof(pixelType)*rowsForWorker*w);
					MPI_Recv(pixelsWorker, sizeof(pixelType)*rowsForWorker*w, MPI_CHAR, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);// Only waits for data for the slaves
				
				
			}
			for(i=0;i<rowsForWorker;i++){
				for(j=0;j<w;j++){
					fwrite(pixelsWorker[i*w+j],1,sizeof(pixelType),sortida);
				}
			}
			
		}
		elapsed = MPI_Wtime() - start;
		fprintf(stderr, "Elapsed time: %.2lf seconds.\n", elapsed);
		fclose(sortida);
		free(pixelsWorker);
	}else{
			MPI_Send(pixelsWorker, sizeof(pixelType)*rowsForWorker*w, MPI_CHAR, 0,0, MPI_COMM_WORLD);
		
		
	}
	
	
        
        
    MPI_Finalize();
	
    return 0;
}
