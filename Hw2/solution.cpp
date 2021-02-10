#include <iostream>
#include <fstream>       // for ifstream
#include <cctype>        
#include <string>
#include <vector>
#include <sstream>      // std::stringstream
#include <algorithm>    // std::find
#include <chrono>
using namespace std;

static int stopID = 0;
int NO_HOPS; //max allowed hops
int bests_hop = 0; 
vector<pair<int,string>>names_and_ids; //GLOBAL vector of pairs (id and name)

struct metroStop {
  string metroStopName;
  metroStop *left;
  metroStop *right;
  int ID;
  
  metroStop(){};
  metroStop(string name, metroStop* leftMS) : left(leftMS), right(nullptr), metroStopName(name) , ID(stopID++){
    names_and_ids.push_back(pair<int,string>(ID,metroStopName));
  }
}; 

struct metroLine {
  string metroNumber;
  metroLine *next;  
  metroStop *metroStops;
  
  vector<double>distances;
  
  metroLine(){};
  metroLine(string name): next(nullptr), metroStops(nullptr), metroNumber(name) {}
};

metroLine* head = nullptr; //GLOBAL
vector<string> transfers; //GLOBAL

//This function takes a metroStop's ID, returns the pointer of it in linked-list, it's location in line and a pointer to it's line
//Note that you can also hold 3 values with pairs (and more of course)
pair<pair<metroStop*,int>,metroLine*> find_stop(int id){
  
  metroLine* curr = head;
  while(curr){
    int in_metro = -1;
    metroStop* ms= curr->metroStops;
    while(ms){
      in_metro++;
      if(ms->ID == id){
	return std::make_pair(pair<metroStop*,int>(ms,in_metro), curr);
      }
      ms = ms->right;
    }
    curr = curr->next;
  }
}

//This function calculates a cost from sequence a of IDs
void compare_and_save(vector<int>path, vector<int> &best_cost_path, int &best_cost, int hops){
  
  int cost = 0;
  
  for(int i = 0; i < path.size(); i++){
    pair<pair<metroStop*,int>,metroLine*>info = find_stop(path[i]);
    metroLine* curr = info.second;
    int in_metro = info.first.second;
    int id = path[i];
    int next = path[i+1];

    //If next stop's id = id+1 of current stop it's at it's right in line, else it's at it's left
    //But this may cause trouble, what else we need to check?
    if(next == id+1){ 
      cost += curr->distances[in_metro];
    }
    else if(next == id-1){
      cost += curr->distances[in_metro-1];
    }
  }
  
  if(cost < best_cost){
    best_cost = cost;
    bests_hop = hops;
    best_cost_path.clear();
    for(int i = 0; i < path.size(); i++){
      best_cost_path.push_back(path[i]);
    }
  }
}

void print_path(vector<int>path){
  
  pair<pair<metroStop*,int>,metroLine*> info = find_stop(path[0]);
  
  string prevLine = info.second->metroNumber;
  
  cout << "Best cost path: " << endl;
  cout << prevLine << ": ";
  
  for(int i = 0; i < path.size(); i++){
    pair<pair<metroStop*,int>,metroLine*>info = find_stop(path[i]);
    int id = path[i];
    int next = path[i+1];
    
    string currLine = info.second->metroNumber;
    
    if(prevLine != currLine){
      cout << "\n";
      cout << currLine << ": ";
    }
    
    cout << names_and_ids[path[i]].second << " ";
    
    if(next != id+1 || next != id-1){
      info = find_stop(path[i]);
      prevLine = currLine;
    }
  }
  std::cout << "\nHops: " << bests_hop << endl;
}


//This is the actual pathfinder()
// I choose to keep track of "path" as a sequence of ID's of visited stops
// Another option could be to have a string path and a double cost variable, adding them with each call
// Give attention to parameters, it's crucial to have "track keeping" variables such as path as "call by address" because otherwise you will end up modifying a shared variable across all calls of this function. Indeed, best_cost_path and best_cost are call by reference because we want to modify them and make visible to all calls.  
void recursive_calls(metroLine* curr, metroStop* ms, vector<int>path, vector<int> visited, vector<int> &best_cost_path, int &best_cost, string msname1, string msname2, int hops){
  //curr=current metroLine ms= current metroStop, path is empty, visited are marked as 0, only the starting point is 1
    //best_cost_path and best_cost are empty
    //ms1= curr stopName, ms2= destination stopName

  //we add id's to path vector
  path.push_back(ms->ID); //Add current stop's ID to path
  visited[ms->ID] = 1; //Mark current stop as visited
  
  if(ms->metroStopName == msname2){ //You found the target stop, try to update the best cost path
    compare_and_save(path, best_cost_path, best_cost, hops);
    return; // If you not return after finding a path your function will probably continue to search
    //Note that our function is void, so we are not checking previous call's return values nor returning anything. We update a global variable if we found a path then just halt execution.
  }
  
  if(ms->left != nullptr && !visited[ms->left->ID]){ //If there is an unvisited stop at the left of the current stop and it is unvisited, call the function with it
    recursive_calls(curr,ms->left,path,visited,best_cost_path,best_cost,msname1,msname2,hops);
  }
  
  if(ms->right != nullptr && !visited[ms->right->ID]){ //If there is an unvisited stop at the right of the current stop and it is unvisited, call the function with it
    recursive_calls(curr,ms->right,path,visited,best_cost_path,best_cost,msname1,msname2,hops);
  }
  //Scanned the line, could not find


  //HOPPING
  //If current stop is a transfer stop, find other lines that contains a stop with the same name.
  // But there could be more than one choice, so push possible stops into a vector and we will call this function for all them. This is for example at the M1B Zeytinburnu, will try T1 Zeytinburnu then 34G Zeytinburnu regardless of the result of T1 Zeytinburnu, therefore will search all possible paths
  if(find(transfers.begin(), transfers.end(), ms->metroStopName) != transfers.end() && hops < NO_HOPS){
    metroLine* check = head;
    vector<pair<metroLine*,metroStop*>> ways;
    while(check){
      metroStop* mst = check->metroStops;
      while(mst){
	if(mst->metroStopName == ms->metroStopName && curr->metroNumber != check->metroNumber){ //transfers
	  if(!visited[mst->ID])
	    ways.push_back(pair<metroLine*,metroStop*>(check,mst)); //You can construct a pair on the fly. This will make things easier
	}
	mst = mst->right;
      }
      check = check->next;
    }
    
    //cout << "Decide to hop at " << curr->metroNumber << " " << ms->metroStopName << " pos: " << ways.size() <<endl;
    
    for(int w = 0; w < ways.size(); w++){ //Call this function for all possible transfers
     // cout << "Hopping: " << ms->metroStopName << " " << ways[w].second->metroStopName << endl; 
      recursive_calls(ways[w].first,ways[w].second,path,visited,best_cost_path,best_cost,msname1,msname2,++hops);
    }
  }
  //HOPPING
}
	

//This is preparation for patfinder()
int pathfinder(){
        
  vector<int>best_cost_path;
  int best_cost = 100000; 
  
  string msname1, msname2;

  cout << "Where are you now?" << endl;
  cin >> msname1;

  cout << "Where do you want to go?" << endl;
  cin >> msname2;

  if(msname1 == msname2) {
    cout << "You are a lucky person. You are at where you want to be. Look around..." << endl;
    return 1;
  }

  auto start = std::chrono::steady_clock::now(); //timer
  vector<pair<metroLine*,metroStop*>> start_points; //vector of pairs (metroLine* and metroStop*) to store all possible start points

  //Check all of linked list structure and determine the possible starts
  //This is necessary since starting stop could be a transfer stop
  metroLine* curr = head;
  while(curr){
    metroStop *ms = curr->metroStops; 
    while(ms){
      if(ms->metroStopName == msname1){ //ADD ALL THE STARTING POINTS TO START_POINTS VECTOR AS PAIR
	start_points.push_back(pair<metroLine*,metroStop*>(curr,ms));
      }
      ms = ms->right;
    }
    curr = curr->next;
  }

  cout << "Possible Starts: " << endl;
  for(int i = 0; i < start_points.size(); i++){
    cout << start_points[i].first->metroNumber << " "  //first is the *metroLine
	 << start_points[i].second->metroStopName << endl; //second is the *metroStop
  }

  //Main solver
  //Start the search for each possible starting point
  for(int i = 0; i < start_points.size(); i++){
    int hops = 0; //for each start point, we search an optimal path, make global hops =0
    metroLine* curr = start_points[i].first; //get the currLine
    metroStop* ms = start_points[i].second; //get the currStop
    
    
    //stopID is 0 (static global int), all elements of unvisited are 0
    vector<int>visited(stopID); //Since each starting point is a different search, we need to reset these arrays
    vector<int>path; //new path

    for(int j = 0; j < start_points.size(); j++){
      visited[start_points[j].second->ID] = 1; //Mark your starting points visited to avoid unnecessary transfers at the beginning of the search
    //visited vector[start_points[j].metroStop->ID] =1 (we marked the starting point as visited by assigning 1)
    }
    recursive_calls(curr,ms,path,visited,best_cost_path,best_cost,msname1, msname2,hops); //Start search for each starting point
  }

  if(best_cost == 100000){
    cout << "No path found between " << msname1 << " and " << msname2 << endl;
  }
  else{
    print_path(best_cost_path);
    cout << "Cost: " << best_cost << std::endl;
  }

  auto end = std::chrono::steady_clock::now();                  
  std::chrono::duration<double> elapsed_seconds = end-start;
  cout << "Elapsed time: " << elapsed_seconds.count() << " seconds" << endl;
}

//this is a helper function to print a single metro line
void printMetroLine(const metroLine* ml) {
  cout << ml->metroNumber << ": ";
  
  metroStop* ms = ml->metroStops;
  while(ms) {
    cout << ms->metroStopName;
    ms = ms->right;
    if(ms) {
      cout << " <-> ";
    }
  }
  cout << endl;
}

void printMetroLines() {
  cout << endl;
  metroLine* curr = head;
  while(curr) {
    printMetroLine(curr);
    curr = curr->next;
  }
  cout << endl;
}



void printMetroLineDetailed(const metroLine* ml) {
  cout << ml -> metroNumber << ": "; //first cout metroLine num

  int trans = 0;
  metroStop* ms = ml->metroStops; 
  while(ms) {
    cout << ms->metroStopName;
    double dist = ml->distances[trans];
    ms = ms->right;
    if(ms){
      cout << "<-" << dist << "->";
    }
    trans++;
  }
  cout << endl;
}


void printMetroLinesDetailed(){
  cout << endl;
  metroLine* curr = head;
  while(curr){ //while there is a metroLine
    printMetroLineDetailed(curr);
    curr = curr->next;
  }
  cout << endl;
}



metroLine* findMetroLine(const string name) {
  metroLine* currML = head;
  while(currML && (currML->metroNumber != name)) {
    currML = currML->next;
  }
  return currML; //if currML does not exist, it will return nullptr
}

metroStop* findMetroStop(const string name, const metroLine* currML) {
  metroStop* currMS = currML->metroStops;
  while(currMS && (currMS->metroStopName != name)) {
    currMS = currMS->right;    
  }
  return currMS; //currMS= nullptr if metroStop does not exists, else return the currMS
}

//If you want to add metro line :)
int addMetroLine() {
  
  string mlname;
  cout << "Enter the name of the new metro line (0 for exit)." << endl;
  cin >> mlname;
  
  while(findMetroLine(mlname)) {
    cout << "Metro line already exists: enter a new one (0 for exit)" << endl;
    cin >> mlname;
  }
  
  if(mlname == "0") {
    return 0;
  }
  
  metroLine* currML;
  currML = new metroLine(mlname);
  
  if(currML) { 
    string msname = " "; 
    metroStop* prevMS = nullptr;
    metroStop* currMS = nullptr;
    
    while(msname != "0") {
      cout << "Enter the name of the next metro stop (enter 0 to complete)" << endl;
      cin >> msname;
      
      if(findMetroStop(msname, currML)) {
	cout << "Metro stop already exists in the line" << endl;
      } else if(msname != "0") {
	currMS = new metroStop(msname, prevMS);
	if(!currML->metroStops)
	  currML->metroStops = currMS;
	if(prevMS)
	  prevMS->right = currMS;
	prevMS = currMS;
      }
    }
    
    if(currML->metroStops != nullptr) {
      
      char confirm = ' ';
      while(tolower(confirm) != 'y' && tolower(confirm) != 'n') {
	
	cout << "The metro line information is shown below" << endl;
	printMetroLineDetailed(currML);
	
	cout << "Are you sure? Enter (y/Y) for yes (n/N) for no?" << endl;
	cin >> confirm;
	
	if(tolower(confirm) == 'y') {
	  currML->next = head;
	  head = currML;
	} else if(tolower(confirm) == 'n') {
	  while(prevMS) {
	    currMS = prevMS->left;
	    delete prevMS;
	    prevMS = currMS;
	  }
	  delete currML;
	  return 0;
	}
      }
    } else {
      cout << "You are not allowed to add an empty metro line" << endl;
      return 0;
    }
  }
  
  printMetroLines();
  return 1;
}


void calculate_transfers(){
  //For each stop of each line, check if stop is unique, if not it's a transfer
  metroLine* currLine = head;
  
  while(currLine){
    metroStop* currStop = currLine->metroStops;
    while(currStop){
      
      metroLine* checkLine = head; //for each metroStop on a line, created checkLine which points to head metroLine
      while(checkLine){
	metroStop* checkStop = checkLine->metroStops;
	while(checkStop){
	  string curr = currStop->metroStopName;
	  string check = checkStop->metroStopName;
	  string cl = currLine->metroNumber;
	  string chl = checkLine->metroNumber;
	  
	  if((curr == check) && (cl != chl)){ //if metroStop names are equal and they are on different line, add to transfers
	    if(std::find(transfers.begin(), transfers.end(), check) == transfers.end()){
	      transfers.push_back(curr);
	    }
	  }
	  checkStop = checkStop->right;
	}
	checkLine = checkLine->next;
      }
      currStop = currStop->right;
    }
    currLine = currLine->next;
  }
}


void head_add(metroLine* line, int cost1){
  line->distances.insert(line->distances.begin(),cost1);
}

void between_add(metroLine* line, metroStop* prevms, int cost1, int cost2){
  pair<pair<metroStop*, int>,metroLine*> info = find_stop(prevms->ID);
  int in_metro = info.first.second;
  
  line->distances.insert(line->distances.begin()+in_metro,cost1);
  line->distances.insert(line->distances.begin()+in_metro+1,cost2);
}

void end_add(metroLine* line, metroStop* prevms, int cost1, int cost2){
  line->distances.push_back(cost1);
}

int addMetroStop() {
  string mlname;
  cout << "Enter the name of the metro line to insert a new metro stop (0 for main menu)" << endl;
  cin >> mlname;
  
  metroLine* currML = findMetroLine(mlname); //got the address of currML 
  if(!currML) {
    cout << "Metro line cannot be found. Going back to previous menu." << endl;
    return 0;
  }
  
  cout << "The metro line information is shown below" << endl;
  printMetroLineDetailed(currML); //sent the address of currML
  
  string msname;
  cout << "Enter the name of the new metro stop" << endl;
  cin >> msname;
  if(findMetroStop(msname, currML)) {
    cout << "Metro line already exists. Going back to previous menu." << endl;
    return 0;
  }  
  
  string pmsname;
  cout << "Enter the name of the previous metro stop for the new one (0 to put the new one as the first metro stop)" << endl;
  cin >> pmsname;

  int cost1,cost2;
  cout << "Enter new cost 1" << endl;
  cin >> cost1;

  cout << "Enter new cost 2" << endl;
  cin >> cost2;
  
  metroStop* prevMS = nullptr;
  if(pmsname != "0") {
    while((pmsname != "0") && ((prevMS = findMetroStop(pmsname, currML)) == nullptr)) { //get the prevMS correctly
      cout << "Metro stop does not exist. Typo? Enter again (0 for main menu)" << endl;  
      cin >> pmsname;
      if(pmsname == "0") {
	return 0;
      }
    }
  }
  
  metroStop* currMS = new metroStop(msname, prevMS); //new metroStop's previous is prevMS, name is msname
  if(!prevMS) { //if previous stop does not exist, currMS is the beginning metroStop which is currML->metroStops
    currMS->right = currML->metroStops;
    currML->metroStops->left = currMS;
    currML->metroStops = currMS;
    head_add(currML, cost1); //insert only cost 1
  } else { //new metroStop (currMS is not the beginning metroStop)
    metroStop* nextMS = prevMS->right;
    
    currMS->right = nextMS;
    currMS->left = prevMS;
    
    prevMS->right = currMS;
    if(nextMS) { //currMS is not the beginning nor the end
      nextMS->left = currMS;
      between_add(currML,prevMS,cost1,cost2);
    }
    else{end_add(currML,prevMS,cost1,cost2);} //currMS is the last stop
  }
  
  printMetroLinesDetailed();
  calculate_transfers();
  return 1;
}

int deleteMetroLineWithName(string mlname) {
  metroLine* currML = head;
  metroLine* prevML = nullptr;
  while(currML && currML->metroNumber != mlname) {
    prevML = currML;
    currML = currML->next;
  }
  //currML is found or it is nullptr
  if(currML) { //currML (line to be deleted found)
    metroStop* prevMS = nullptr;
    metroStop* currMS = currML->metroStops;
    while(currMS) {
      prevMS = currMS;
      currMS = currMS->right;
      delete prevMS;
    }
    
    if(prevML) { //if there is a prevML, connect it with currML->next
      prevML->next = currML->next;
      delete currML;
    } else { //currML was the first line, head should be updated
      head = currML->next;
      delete currML;
    }
  } else {
    return 0;
  }
  
  return 1;
}

int deleteMetroLine() {
  string mlname;
  cout << "Enter the name of the metro line to delete" << endl;
  cin >> mlname;
  
  if(!findMetroLine(mlname)) {
    cout << "Metro line cannot be found. Going back to previous menu." << endl;
    return 0;
  } 
  
  deleteMetroLineWithName(mlname);
  
  printMetroLines();
  return 1;
}

void head_delete(metroLine* line){
  
  vector<int> copy;
  for(int i = 1; i < line->distances.size();i++){
    copy.push_back(line->distances[i]);
  }
  
  line->distances.clear();
  
  for(int i = 0; i < copy.size(); i++){
    line->distances.push_back(copy[i]);
  }
}

void between_delete(metroLine* line, metroStop* prevms){
  cout << "Between delete" << endl;
  pair<pair<metroStop*, int>,metroLine*> info = find_stop(prevms->ID);
  int in_metro = info.first.second;
  
  int cost1 = line->distances[in_metro+1];
  int cost2 = line->distances[in_metro];
  
  line->distances.erase(line->distances.begin()+in_metro);
  line->distances.erase(line->distances.begin()+in_metro-1);
  line->distances.insert(line->distances.begin()+in_metro,cost1+cost2);
}

void end_delete(metroLine* line, metroStop* prevms){
  line->distances.pop_back();
}

int deleteMetroStop() {
  string mlname;
  cout << "Enter the name of the metro line to delete a new metro stop (0 for main menu)" << endl;
  cin >> mlname;
  
  metroLine* currML = findMetroLine(mlname);
  if(!currML) {
    cout << "Metro line cannot be found. Going back to previous menu." << endl;
    return 0;
  } 
  
  cout << "The metro line information is shown below" << endl;
  printMetroLineDetailed(currML);
  
  metroStop* currMS = nullptr;
  string msname;
  cout << "Enter the name of the metro stop to delete (0 for main menu)" << endl;
  cin >> msname;
  while((currMS = findMetroStop(msname, currML)) == nullptr) {
    cout << "Metro stop cannot be found. Enter the name of the metro stop to delete (0 for main menu)" << endl;
    cin >> msname;  
    
    if(msname == "0") { 
      return 0;
    }
  }

  metroStop* prevMS = currMS->left;
  metroStop* nextMS = currMS->right;

    
  if(!prevMS && !nextMS) { //delete the whole line
    deleteMetroLineWithName(mlname);
  }
  else { //no need to delete whole line. cases 1)MS in the beginning 2)MS in between 3)MS in tail
    if(!prevMS) { //if the currMS is the first MS
      currML->metroStops = nextMS;
      currML->metroStops->left = nullptr;
      head_delete(currML); //delete first cost
    } else { //currMS is not on the beginning. cases 1)between 2)tail
      prevMS->right = nextMS;
      if(!prevMS->right) //currMS is on the tail
	end_delete(currML, prevMS); //delete last cost
    }
    
    if(nextMS && prevMS) {
      nextMS->left = prevMS;
      between_delete(currML, prevMS); //delete 2 costs
    } 
    
    delete currMS; 
  }
  
  printMetroLinesDetailed();
  calculate_transfers();
  return 1;
}

bool consistencyCheck() {
  metroLine* currML = head; //a global variable
  while(currML) {
    metroStop* currMS = currML->metroStops;
    while(currMS) {
      metroStop* rightMS = currMS->right;
      if(rightMS && rightMS->left != currMS) {
	cout << "Inconsistency for " 
	     << currML->metroNumber 
	     << " " 
	     << currMS->metroStopName << endl;
	return false;
      }
      currMS = currMS->right;
    }
    currML = currML->next;
  }
  return true;
}

void printMainMenu() {
  cout << endl;
  cout <<"I***********************************************I"<<endl
       <<"I               0 - EXIT PROGRAM                I"<<endl
       <<"I               1 - PRINT LINES                 I"<<endl
       <<"I               2 - ADD METRO STOP              I"<<endl
       <<"I               3 - DELETE METRO STOP           I"<<endl
       <<"I               4 - PATH FINDER                 I"<<endl
       <<"I***********************************************I"<<endl
       <<">>";
  cout << endl;
}  

void processMainMenu() {
  char input;
  do{
    if(!consistencyCheck()) {
      cout << "There are inconsistencies. Exit." << endl; 
      return;
    }
    printMainMenu();
    cout << "Please enter your option " << endl;
    cin >> input;
    switch (input) {
    case '0':
      cout << "Thanks for using our software" << endl;
      return;
    case '1':
      printMetroLinesDetailed();
      break;
    case '2':
      addMetroStop();
      break;
    case '3':
      deleteMetroStop();
      break;
    case '4':
      pathfinder();
      break;
    default:
      cout << "Invalid option: please enter again" << endl;
    }
  } while(true);
}

void add_distances(string name, vector<double> distance_copy){
  metroLine* to_add = findMetroLine(name);
  //add every element in distance_copy to that metroLine->distances
  for(int i = 0; i < distance_copy.size(); i++){
    to_add -> distances.push_back(distance_copy[i]);
  }
}

int main() {
  string filename;
  cout << "Enter the name of metro line data file: ";
  cin >> filename;
  ifstream input(filename);
  
  while ( input.fail() ) {   
    cout << "Could not open file: " << filename << endl;
    cout << "Enter the name of metro line data file: ";
    cin >> filename;
    input.open(filename);
  }
  cout << "Processing data file" << endl;
  
  metroLine* prevML = nullptr;
  metroLine* currML = nullptr;  
  
  //read a line from the file
  string line;
  while(getline(input, line)) {
    cout << line <<endl;
    stringstream ls(line);
    
    string ml;
    getline(ls, ml, ','); 
    //create the metro line
    currML = new metroLine(ml); 
    
    //read the metro stops	
    string ms;      
    metroStop* prevMS = nullptr;
    metroStop* currMS = nullptr;  
    while(getline(ls, ms, ',')) { 
      ms.erase(std::remove(ms.begin(), ms.end(), '#'), ms.end());
      if(ms != "#\r") {
	//create a new metro stop for each word separated by commas
	currMS = new metroStop(ms, prevMS);
	if(!currML->metroStops) { //if there is no metroStop on the line
	  currML->metroStops = currMS;
	}
	if(prevMS) prevMS->right = currMS; //if there is a prevMS, it's right is currMS
	prevMS = currMS; //shift the prevMS
      }
    } 
    
    //if there is a metro stop in the metro line add it to the table
    if(currMS) {
      if(!head) //if no head metroLine
	head = currML; //head is the currML
      if(prevML) //if there is a prevML
	prevML->next = currML; //prevML's next is currML
      prevML = currML; //shift the prevML
    } 
  }

  
  cout << "Enter the name of cost file: ";
  cin >> filename;
  ifstream distance(filename);

  while(distance.fail() ){
    cout << "Could not open file: " << filename << endl;
    cout << "Enter the name of distances file: ";
    cin >> filename;
    distance.open(filename);
  }
  cout << "Processing data file" << endl;

  double dist;
  string name;
  string val;
  while(getline(distance, line)){
    cout << line << endl;
    
    stringstream ds(line);
    getline(ds,name,',');

    vector<double>distance_copy;

    while(getline(ds,val,',')){
      dist = stod(val); //string to double
      distance_copy.push_back(dist);
    }
    add_distances(name, distance_copy);
  }

  calculate_transfers(); //Calculate transfers before calling pathfinder so you can check quickly later
  cout << "Transfer stops: " << endl;
  
  for(int i = 0; i < transfers.size(); i++){
  cout << transfers[i] << endl; 
  }

  cout << "Enter maximum number of hops" << endl;
  cin >> NO_HOPS;

  metroLine* curr = head; //curr points to the address of head

 /* while(curr != nullptr){
    cout << "Line: " << curr -> metroNumber << " "; //print the metroLine Num
    cout << "stop ids: "; 
    metroStop* stop = curr->metroStops;
    while(stop){
      cout << stop->ID << " "; //print the ID's (0,1,2,3,4,5....)
      stop = stop->right;
    }
    curr = curr->next;
    cout << endl;
  }*/
  processMainMenu();
  return 1;
}
