#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>

using namespace std;
int counter = 0;
int counter1 = 0;
int counter2 = 0;

class Cache{
private:
	int nRows;
	int nCols;
	int** tagA;
	int** dataA;
	int getRow(int);

public:
	Cache(int rows, int cols){
		nRows = rows;
		nCols = cols;
		tagA = (int**)malloc(nRows*sizeof(int*));
		dataA = (int**)malloc(nRows*sizeof(int*));
		for(int count=0; count<nRows; count++){
			tagA[count] = (int*)malloc((nCols-1)*sizeof(int));
			dataA[count] = (int*)malloc((nCols)*sizeof(int));
		}
	}

	int search(int);
	int remove(int);
	void insert(int);
	void access(int);
	void print(int**, int);
};

int Cache::getRow(int address){
	return address%nRows;
}

void Cache::insert(int address){
	int index = getRow(address);
	int tempIndex = 0;
	while(tempIndex < nCols-1){
		if(tagA[index][tempIndex] == 0){
			tagA[index][tempIndex] = 1;
			tempIndex = tempIndex*2 + 1;
		}
		else{
			tagA[index][tempIndex] = 0;
			tempIndex = tempIndex*2 + 2;
		}
	}
	dataA[index][tempIndex-(nCols-1)] = address;
}

void Cache::access(int address){
	int nRow = getRow(address);
	int tempIndex = nCols + search(address);
	while(tempIndex != 0){
		if(tempIndex%2 == 0){
			tempIndex = (tempIndex/2)-1;
			if(tagA[nRow][tempIndex] == 1)
				tagA[nRow][tempIndex] = 0;
		}
		if(tempIndex%2 != 0){
			tempIndex = tempIndex/2;
			if(tagA[nRow][tempIndex] == 0)
				tagA[nRow][tempIndex] = 1;
		}
	}
}

int Cache::search(int address){
	int index = getRow(address);
	for(int count=0; count<nCols; count++)
		if(dataA[index][count] == address)
			return count;
	return -1;
}
int Cache::remove(int address){
	int index = getRow(address);
	dataA[index][search(address)] = -1;
}

void accessX(Cache l1, Cache l2, Cache l2R, int address);

int main(int argc, char* argv[]){
	ifstream file;
	file.open(argv[1]);
	counter = 0;
	Cache* l1 = new Cache(8, 128);
	Cache* l2 = new Cache(16, 128);
	Cache* l2R = new Cache(8, 128);
	int tempVal;

	while(1){
		if(file.eof())
			break;
		file>>hex>>tempVal;
		accessX(*l1, *l2, *l2R, tempVal);
	}
	file.close();
	cout<<counter<<" "<<counter1<<" "<<counter2<<endl;
}

void accessX(Cache l1, Cache l2, Cache l2R, int address){
	int l1Hit = l1.search(address);
	if(l1Hit == -1){
		int l2Hit = l2.search(address);
		if(l2Hit != -1){
			l2.remove(address);
			l2R.insert(address);
			counter++;
			counter1++;
		}
		else{
			int l2RHit = l2R.search(address);
			if(l2RHit == -1){
				l2.insert(address);
			}
			else{
				l2R.access(address);
				counter++;
				counter2++;
			}
		}
		l1.insert(address);
	}
	else{
		l1.access(address);
	}
}
