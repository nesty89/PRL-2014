#include <iostream>
#include <fstream>
#include <mpi.h>

using namespace std;

// zprava s X
#define TAGX 0
// zprava s Y
#define TAGY 1
// zprava zaslani X na index C
#define TAGC 2
// zprava nesouci Z pro shiftovani
#define TAGZ 3

int main(int argc, char **argv){
	int numProc;
	int myProc;

	MPI_Status stat; 
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
	
	// registry
	int C = 1;
	int X = -1, Y = -1, Z = -1;
	
	// pocet procesoru bez mastera
	int n = numProc-1;

	fstream fin;
	// pomocna promenna pro identifikaci pri stejnych hodnotach vstupu 
	int ID;

	// nacteni cisel ze souboru a zasilani X a Y na prislusna mista
	if(myProc == 0){
		fin.open("numbers", ios::in);
		X = 1;
		while(fin.good()){
			Y = fin.get();
			if(!fin.eof()){
				cout << Y << " ";
				MPI_Send(&Y, 1, MPI_INT, 1, TAGY, MPI_COMM_WORLD);
				if(X <= n){
					MPI_Send(&Y, 1, MPI_INT, X++ , TAGX, MPI_COMM_WORLD);
				}
			}
		}
		cout << endl;
		fin.close();	
	} 
	// prvnich n kroku z 2n
	for (int k = 1; k <= n; k++){
		// procesor 0 je master toto nedela
		if(myProc == 0){
			break;
		}

		// zvysovani hodnoty registru C
		if(Y >= 0 && X >= 0 && X >= Y){
			// pokud X > Y nebo jsou stejne a jsem v kroku porovnani, 
			// ktery je nizsi nez moje poradi prideleni X
			if( X > Y || (X == Y && ID > k - myProc)){
				C++;
			}
		}

		// predavani hodnot
		if(Y >= 0 && myProc != n){
			MPI_Send(&Y, 1, MPI_INT, myProc + 1 , TAGY, MPI_COMM_WORLD);
		}
		if(k >= myProc && myProc != 1){
			MPI_Recv(&Y, 1, MPI_INT, myProc - 1, TAGY, MPI_COMM_WORLD, &stat);
		}
		// ziskani X
		if(myProc == k){
			MPI_Recv(&X, 1, MPI_INT, 0, TAGX, MPI_COMM_WORLD, &stat);
			ID = k;
		}
		// jsem jednicka prijmam Y jako prvni
		if (myProc == 1){
			MPI_Recv(&Y, 1, MPI_INT, 0, TAGY, MPI_COMM_WORLD, &stat);	
		}
	}

	// druhych n kroku z 2n
	for  (int k = 1; k <= n; k++){
		// procesor 0 je master toto nedela
		if(myProc == 0){
			break;
		}

		// zvysovani hodnoty registru C
		if(k <= myProc && Y >= 0 && X >= 0 && X >= Y){
			// pokud X > Y nebo jsou stejne a jsem v kroku porovnani, 
			// ktery je nizsi nez moje poradi prideleni X
			if( X > Y || (X == Y && ID >  n + k - myProc)){
				C++;
			}
		}

		// preposilani hodnot
		if(myProc != n && k <= myProc){
			MPI_Send(&Y, 1, MPI_INT, myProc + 1 , TAGY, MPI_COMM_WORLD);
		}

		if(k < myProc ){
			MPI_Recv(&Y, 1, MPI_INT, myProc - 1, TAGY, MPI_COMM_WORLD, &stat);
		}
	}
  
    // pokud nejsem 0 tak zasilam svoji hodnotu X procesoru dle C
	if( myProc != 0){
		MPI_Send(&X, 1, MPI_INT, C , TAGC, MPI_COMM_WORLD);
		// MPI_ANY_SOURCE jsem nenasyta a beru cokoliv prijde
		MPI_Recv(&Z, 1, MPI_INT, MPI_ANY_SOURCE, TAGC, MPI_COMM_WORLD, &stat);
	}

	// shiftovani doleva (v algoritmu je doprava, ale ze zadani se radi od nejmensiho po nejvetsi)
	for (int i = 0; i < n; i++){
		if(myProc > 0){
			MPI_Send(&Z, 1, MPI_INT,  myProc - 1, TAGZ, MPI_COMM_WORLD);
		}
		if(myProc < n){
			MPI_Recv(&Z, 1, MPI_INT, myProc + 1, TAGZ, MPI_COMM_WORLD, &stat);
			// master vypisuje
			if(myProc == 0){
				cout << Z << endl;
			}
		}
	}

	MPI_Finalize();
	return 0;
		
}