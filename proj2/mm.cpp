#include <iostream>
#include <cstdlib>
#include <fstream>
#include <mpi.h>

using namespace std;

#define TAGX 1
#define TAGY 2
#define TAGD 3


enum DStates {
	s = 0,
	p,
	g
};

// dalsi syn se pocita jako L=2n+1 P=2n+2

int main(int argc, char **argv){
	int numProc;
	int myProc;

	MPI_Status stat; 
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
	int left = 2 * myProc + 1, right = 2 * myProc + 2;
	fstream fin;

	int X, Y, C = 0;
	DStates d, dl, dr;
	int counter = 0;
	bool isx = true;
	int half = (numProc)/2;
	double start,end;
	if(myProc == 0){
		int num;
		fin.open("numbers");
		while(fin.good()){
			
			num = fin.get();
			if(fin.eof())
				break;

			if (num == '\n'){
				isx = false;
				counter = 0;
				continue;
			}
			num -=  '0';
			if(isx){
				MPI_Send(&num, 1, MPI_INT, half + counter, TAGX, MPI_COMM_WORLD);
			} else {
				MPI_Send(&num, 1, MPI_INT, half + counter, TAGY, MPI_COMM_WORLD);
			}
			counter++;
		}
		fin.close();
	} 
	
	if(myProc >= half){
		MPI_Recv(&X, 1, MPI_INT, 0, TAGX, MPI_COMM_WORLD, &stat);
		MPI_Recv(&Y, 1, MPI_INT, 0, TAGY, MPI_COMM_WORLD, &stat);
		//cout << myProc << ": " << X << " " << Y <<endl;
		// predpocet d_i
		if( X == 1 && Y == 1){
			d = g;
		} else if( X == 0 && Y == 0){
			d = s;
		} else {
			d = p;
		}
	}

	// upSweep
	if(myProc < half){
		if(left < numProc){
			MPI_Recv(&dl, 1, MPI_INT, left, TAGD, MPI_COMM_WORLD, &stat);	
		}

		if(right < numProc){
			MPI_Recv(&dr, 1, MPI_INT, right, TAGD, MPI_COMM_WORLD, &stat);
		} else {
			dr = s;
		}

		if(dl == s){
			d = s;
		} else if( dl == p){
			d = dr;
		} else {
			d = g;
		}
	}

	if(myProc != 0){
		if(myProc % 2 == 0){
			MPI_Send(&d, 1, MPI_INT, (myProc-2)/2, TAGD, MPI_COMM_WORLD);	
		} else {
			MPI_Send(&d, 1, MPI_INT, (myProc-1)/2, TAGD, MPI_COMM_WORLD);
		}
	}
	// upSweep end

	// downSweep
	if(myProc == 0){
		d = p;	
		if(right < numProc){
			MPI_Send(&d, 1, MPI_INT, right, TAGD, MPI_COMM_WORLD);
		}

		if(left < numProc){
			if(dr == p){
				dl = d;
			} else {
				dl = dr;
			}
			MPI_Send(&dl, 1, MPI_INT, left, TAGD, MPI_COMM_WORLD);
		}
	} else {
		if(myProc % 2 == 0){ // pravy = parent + pravy
			MPI_Recv(&d, 1, MPI_INT,(myProc-2)/2 , TAGD, MPI_COMM_WORLD, &stat);
		} else { // levy = parent
			MPI_Recv(&d, 1, MPI_INT,(myProc-1)/2 , TAGD, MPI_COMM_WORLD, &stat);	
		}
		if(right < numProc){
			MPI_Send(&d, 1, MPI_INT, right, TAGD, MPI_COMM_WORLD);
		}

		if(left < numProc){
			if(dr == p){
				dl = d;
			} else {
				dl = dr;
			}
			MPI_Send(&dl, 1, MPI_INT, left, TAGD, MPI_COMM_WORLD);		
		}
	}
	// downSweep end

	if(myProc >= half && d == g){
		C = 1;
	} 
	
	if(myProc >= half){ // output
		cout << myProc << ":" << (X + Y + C) % 2 << endl;
		
		if(myProc == half){
			if((X + Y + C) > 1){ // overflow
				cout << "overflow" << endl;
			}
		}
	}

	MPI_Finalize();
	return 0;
}
