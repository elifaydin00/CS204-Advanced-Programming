#include <iostream>
#include <string>
#include "strutils.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip> 

using namespace std;

struct coordinates{ //to store the inequality pairs of coordinates
	int x1;
	int y1;

	int x2;
	int y2;
};

struct location{ //to store the location of unknowns on the matrix
	int x;
	int y;
};

void openFile(const string& fileType, ifstream& inFile){ //this function will force to get a valid fileName from the user, and open the file
    string fName;
    cout << "Enter the " << fileType << " file: "; //"board" or "constraint"
    cin >> fName;
    inFile.open(fName.c_str());
    while (inFile.fail()){
        cout << "File failed to open!" << endl;
        cout << "Enter the " << fileType << " file: ";
        cin >> fName;
        inFile.open(fName.c_str());
    }
}

void readFile1(ifstream& inFile, vector<vector<int>> &puzzle, int & size){ //this function reads the board data from the file and stores it into a matrix
	string line;
	int count=0;
	int num;
	string matrix_size;
	string str;
	vector<int> temp;
	while(getline(inFile,line)){	
		if (line!=""){
			 if (count==0){
				 istringstream iss(line);
				 iss>>matrix_size; //first line would contain the size of puzzle
				 size=atoi(matrix_size);
				 count++;
			 }
			 else{ //fill the vector! 
				 stringstream ss(line);
				 temp.clear();
				 while(ss>>str){
					 if(str!="X"){
						 num=atoi(str);
						 temp.push_back(num);	
					 }
					 else{ //instead of "X", integer -1 will be replaced to the vector
						 temp.push_back(-1);
					 }
				 }
				 puzzle.push_back(temp);
			 }
		} 
	}

}

void readFile2(ifstream & inFile,vector<coordinates> & pairs, int & size ){ //this function reads the constraint data from the file and stores it into a vector
	string line1;
	int count=0;
	string constr_size;
	coordinates temp;
	int x1,y1,x2,y2;
	while(getline(inFile,line1)){
		if (line1!=""){
			istringstream iss(line1);
			if (count==0){			 
				 iss>>constr_size; //first line would contain the size of the constrains
				 size=atoi(constr_size);
				 count++;
			}
			else{ //store the constrains on a vector<coordinates>
				iss>>x1>>y1>>x2>>y2; //substract -1 because the index of the given matrixes starts with 1
				temp.x1=x1-1;
				temp.y1=y1-1;
				temp.x2=x2-1;
				temp.y2=y2-1;
				pairs.push_back(temp);
			}
		}
	}
}

void Print(const vector<vector<int>> & mat)
{
  for (int j=0; j < mat.size(); j++)
    {   
      for (int k=0; k < mat[0].size(); k++)
        {   
	  cout << setw(4) << mat[j][k];
        }
      cout << endl;
    }
}

void printConstr(const vector<coordinates> & constr){
	for(int i=0;i<constr.size();i++){
		cout<<constr[i].x1<<constr[i].y1<<constr[i].x2<<constr[i].y2<<endl;
	}
}

void checkSize(const vector<vector<int>> & puzzle,const int & size,bool & format){ //this function checks the size of board file
	int realSize=puzzle.size();
	if(realSize!=size){
		cout<<"File contains more lines than size"<<endl;
		format=false;
	}
}

void checkSize2(const vector<coordinates> & pairs, const int & size, bool & format){ //this function checks the size of the constraint file
	int vec_size=pairs.size();
	if(vec_size>size){
		cout<<"There are more constraints then previously stated"<<endl;
		format=false;
	}
	else if(vec_size<size){
		cout<<"There are less constraints then previously stated"<<endl;
		format=false;
	}
}

void checkRange(const vector<vector<int>> & puzzle,const int & size, bool & format){ //Each value of board, except the unknown values, must be between 1 and n.
	for(int i=0;i<puzzle.size();i++){
		for(int j=0;j<puzzle[0].size();j++){
			if(!((puzzle[i][j] >=1) && (puzzle[i][j] <=size))){
				if(puzzle[i][j]!=-1){
					cout<<"Value "<<puzzle[i][j]<<" is out of matrix size"<<endl;
					format=false;
					break;
				}
			}
		}
	}
}

void checkRange2(const vector<coordinates> & pairs, const int & size, bool & format){ //Values of the coordinates must be between 0 and n-1.
	for(int i=0;i<pairs.size();i++){
		if(!(((pairs[i].x1>=0) && (pairs[i].x1<=size-1)) && ((pairs[i].y1>=0) && (pairs[i].y1<=size-1)) && ((pairs[i].x2>=0) && (pairs[i].x2<=size-1)) && ((pairs[i].y2>=0) && (pairs[i].y2<=size-1)))){
			cout<<"No such index"<<endl;
			format=false;
			break;
		}
	}
}

void checkRepetition(const vector<vector<int>> & puzzle, bool & format){ //check if there is any repetition before solving the puzzle
	if (puzzle.size() != puzzle[0].size()) {
		format = false;
	}
	else {
		//scan the rows
		for (int i = 0; i < puzzle.size(); i++) {
			for (int j = 0; j < puzzle.size() - 1; j++) {
				for (int k = j + 1; k < puzzle.size(); k++) {
					if (puzzle[i][j] == puzzle[i][k]) {
						if (puzzle[i][j] != -1) {
							cout << "There is a value repeating in same row" << endl;
							format = false;
							break;
						}
					}
				}
			}
		}

		//scan the columns
		for (int i = 0; i < puzzle.size(); i++) {
			for (int j = 0; j < puzzle.size() - 1; j++) {
				for (int k = j + 1; k < puzzle.size(); k++) {
					if (puzzle[j][i] == puzzle[k][i]) {
						if (puzzle[j][i] != -1) {
							cout << "There is a value repeating in same column" << endl;
							format = false;
							break;
						}
					}
				}
			}
		}
	}
}

void doesBoardMatch(const vector<vector<int>> & puzzle, const vector<coordinates> & pairs, bool & format){ //check if the board matches with the constrain file before solving the puzzle
	for(int i=0;i<pairs.size();i++){
		if(puzzle[(pairs[i].x1)][(pairs[i].y1)]<=puzzle[(pairs[i].x2)][(pairs[i].y2)]){
			if(puzzle[(pairs[i].x1)][(pairs[i].y1)]!=-1 && puzzle[(pairs[i].x2)][(pairs[i].y2)]!=-1){ //puzzle is not solved yet (there are no -1's)
				cout<<"Constraints do not match board"<<endl;
				format=false;
				break;
			}			
		}
	}
}

void findUnknowns(const vector<vector<int>> & puzzle, vector<location> & unknowns){ //this function will find the unknowns location and store them on a vector
	location temp;
	for(int i=0;i<puzzle.size();i++){
		for(int j=0;j<puzzle.size();j++){
			if(puzzle[i][j]==-1){ //found the unknown
				temp.x=i;
				temp.y=j;
				unknowns.push_back(temp);
			}
		}
	}
}



void solvePuzzle(vector<location> & unknowns, const vector<coordinates> & pairs, vector<vector<int>> & puzzle, bool & solved){
	int maxSum=0; //this is the maximum sum that can a row, or column can take on a matrix
	int length=puzzle.size();
	for(int i=1;i<=length;i++){ //let size of puzzle be 3, then maxSum = 1+2+3 = 6
		maxSum+=i; 
	}
	int index=0;
	while(unknowns.size()>0){ //force to fill X while there are exist an unknown on the vector
		bool flag=true;

		//unknown X's location on the puzzle
		int x=unknowns[index].x; 
		int y=unknowns[index].y;

		bool countXrow=false;
		bool countXcol=false;
		int sumRow=0;
		int sumCol= 0;

		//scan the row for unknown (-1)
		for(int i=0;i<puzzle.size();i++){
			if(puzzle[x][i]==-1){ //found X on the specific row
				if(!countXrow){ //first time we see X on row
					countXrow=true;
				}
				else{ //there are more than one X in the row
					countXrow=false;
					break;
				}
			}
			else{ //x not found, update the sumRow
				sumRow+=puzzle[x][i];
			}
		}

		if(countXrow){ //there is only one X on the row, it must be unique one!
			puzzle[x][y]= maxSum- sumRow;
			unknowns.erase(unknowns.begin()+index); //erase the unknown index from the vector
			index=0; 
			flag=false;
		}

		else{ //there is more than one X on the row, scan the column
			for(int i=0;i<puzzle.size();i++){
				if(puzzle[i][y]==-1){
					if(!countXcol){ //first time wee se X in column
						countXcol=true;
					}
					else{ //more than one X in column
						countXcol=false;
						break;
					}
				}
				else{ //x is not found, update the sumCol
					sumCol+=puzzle[i][y];
				}
			}

			if(countXcol){ //there is only one X on the column, it must be unique one!
				puzzle[x][y]= maxSum- sumCol;
			    unknowns.erase(unknowns.begin()+index); //erase the unknown index from the vector
			    index=0; 
			    flag=false;
			}
			else{ //multiple X values (check constraint to decide what to put)
				vector<vector<int>> possValues;
				for(int i=0;i<pairs.size();i++){ //check the constraint pairs
					vector<int> temp; //for each constraint, we will have possible values
					int x1=pairs[i].x1;
					int y1=pairs[i].y1;
					int x2=pairs[i].x2;
					int y2=pairs[i].y2;
					int biggerThan, smallerThan;
					if((x==x1 && y==y1) && (puzzle[x2][y2]!=-1)){ //found unknowns location on the constraint pairs
						biggerThan=puzzle[x2][y2]; //X is bigger than puzzle[x2][y2]
						for(int i=biggerThan+1;i<=puzzle.size();i++){
							temp.push_back(i);
						}
						possValues.push_back(temp);
					}
					else if((x==x2 && y==y2) && (puzzle[x1][y1]!=-1)){ //found unknowns location on the constraint pairs
						smallerThan=puzzle[x1][y1]; //X is smaller than puzzle[x1][y1]
						for(int i=1;i<=smallerThan-1;i++){
							temp.push_back(i);
						}
						possValues.push_back(temp);
					}
				}
				//we checked all the constraint pairs, now look at the possible values
				for(int i=0;i<possValues.size();i++){
					if(possValues[i].size()==1){
						puzzle[x][y]=possValues[i][0];
						unknowns.erase(unknowns.begin()+index); //erase the unknown index from the vector
			            index=0; 
			            flag=false;
					}
				}
				if(flag){ //for some reason, gives assertion fail
					index = index + 1;
										
					if(index==unknowns.size()){
						solved=false;
						break;
					}
				}
			}
		}		
	}
}


int main(){

	ifstream boardFile, constFile;

	//open the files
	openFile("board", boardFile);
	openFile("constraint",constFile);

	//read data from files
	vector<vector<int>> puzzle; //created the puzzle consisting of a matrix
	vector<coordinates> pairs; //created this vector to store the pairs of coordinates of the constraint (x1 y1 x2 y2)

	int size_matrix; //size of matrix
	int size_constr; //size of the constraints
	readFile1(boardFile,puzzle,size_matrix);
	readFile2(constFile,pairs,size_constr);
	
	
	//Before solving the puzzle, do a file check
	bool format=true;


	checkSize(puzzle,size_matrix,format);
	checkSize2(pairs,size_constr,format);
	checkRange(puzzle, size_matrix,format);
	checkRange2(pairs,size_matrix,format); 
	checkRepetition(puzzle,format);
	doesBoardMatch(puzzle,pairs,format);
		
	
	if(format==true){
		cout<<"Files are in correct format"<<endl;

		//solve the puzzle, file format is correct
	
		vector<location> unknowns; //find the unknowns and its locations.Store them on a vector.
		findUnknowns(puzzle,unknowns);
		if(unknowns.size()==0){ //there are no unknowns, puzzle is already solved. Print the puzzle
			Print(puzzle);
		}

		else{ //there are unknowns, solve it
			bool solved=true;
			solvePuzzle(unknowns,pairs,puzzle,solved);
			if(solved){
				Print(puzzle);
			}
			else{
				cout<<"X’s can have more than 1 value"<<endl;
			}
		}
	}
	return 0;
}
