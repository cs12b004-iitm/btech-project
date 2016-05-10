#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>

using namespace std;
int counter = 0;
int counter1 = 0;
int counter2 = 0;
int nCores = 8;

class Cache{
private:
	int nRows;
	int nCols;
	int** tagA;
	int** dataA;
	bool** stateA;
	int getRow(int);

public:
	Cache(int rows, int cols){
		nRows = rows;
		nCols = cols;
		tagA = (int**)malloc(nRows*sizeof(int*));
		dataA = (int**)malloc(nRows*sizeof(int*));
		stateA = (bool**)malloc(nRows*sizeof(bool*));
		for(int count=0; count<nRows; count++){
			tagA[count] = (int*)malloc((nCols-1)*sizeof(int));
			dataA[count] = (int*)malloc((nCols)*sizeof(int));
			stateA[count] = (bool*)malloc((nCols)*sizeof(bool));
		}
		for(int count1=0; count1<nRows; count1++)
			for(int count2=0; count2<nCols; count2++)
				stateA[count1][count2] = false;
	}

	int search(int);
	int remove(int);
	void insert(int);
	void access(int);
	bool isValid(int);
	void invalidate(int);
	void print(int**, int);
};

bool Cache::isValid(int address){
	int nRow = getRow(address);
	int nCol = search(address);
	return stateA[nRow][nCol];
}

void Cache::invalidate(int address){
	int nRow = getRow(address);
	int nCol = search(address);
	stateA[nRow][nCol] = false;
}

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
	stateA[index][tempIndex-(nCols-1)] = true;
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
	stateA[index][search(address)] = false;
}

void accessX(Cache* l1, int currCore, Cache l2, Cache l2R, int address);

int main(int argc, char* argv[]){
	ifstream file;
	int tempRows = 8;
	for(int count=1; count<argc; count++){
		if(argv[count][0] == '-'){
			switch(argv[count++][1]){
				case 'N':
					nCores = atoi(argv[count]);
					break;
				case 'F':
					file.open(argv[count]);
					break;
				case 'a':
					tempRows = atoi(argv[count]);
					break;
				default:
					cout<<"Invalid Command."<<argv[count]<<endl;
			}
		}
		else{
			cout<<"Invalid Command. Using default Values."<<endl;
		}
	}
	
	counter = 0;
	vector<Cache> l1;
	l1.reserve(nCores);
	for(int count=0; count<nCores; count++)
		l1.push_back(Cache(8,128));
	Cache* l2 = new Cache(tempRows, 128);
	Cache* l2R = new Cache(tempRows, 128);
	int tempVal;
	int currCore = 0;

	while(1){
		if(file.eof())
			break;
		file>>hex>>tempVal;
		accessX(&l1[0], currCore, *l2, *l2R, tempVal);
		currCore++;
		currCore = currCore%nCores;
	}
	cout<<counter<<" "<<counter1<<" "<<counter2<<endl;
}

void accessX(Cache* l1, int currCore, Cache l2, Cache l2R, int address){
	int l1Hit = l1[currCore].search(address);
	if(l1Hit == -1 || !l1[currCore].isValid(address)){
		if(!l1[currCore].isValid(address))
			l1[currCore].remove(address);
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
		l1[currCore].insert(address);
	}
	else{
		l1[currCore].access(address);
		for(int count=0; count<nCores; count++){
			if(count != currCore)
				l1[count].invalidate(address);
		}
	}
}
