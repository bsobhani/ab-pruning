#include <iostream>
#include <vector>
#include <string>

using namespace std;

int numMoves;
int numInvoke;

char other_type(char c){
  if (c == 'X'){
    return 'O';
  }
  if (c == 'O'){
    return 'X';
  }
  cout << "Invalid type\n";
  return '\0';
}

int type_to_int(char c){
  if (c == 'X'){
    return 1;
  }
  if (c == 'O'){
    return -1;
  }
  cout << "Error: type_to_int\n";
  return 0;
}

class Grid{
	
public:
  vector<char> squares;
  void set(int index, char c){
    if (squares[index] != '_'){
      cout << "Warning: Setting a non-blank square.\n";
    }
    squares[index] = c;
  }
  char get(int index){
    return squares[index];
  }
  string to_string(){
    string s = "";
    for (int i = 0; i < 9; ++i){
      s = s + get(i);
    }
    return s;
  }
  int moves_remaining(){
    int count = 0;
    for (int i = 0; i < 9; ++i){
      if (squares[i] == '_'){
	count++;
      }
    }
    return count;
  }
  int check_(char c){
    if (squares[0] == c && squares[1] == c && squares[2] == c){
      return 1;
    }
    if (squares[3] == c && squares[4] == c && squares[5] == c){
      return 1;
    }
    if (squares[6] == c && squares[7] == c && squares[8] == c){
      return 1;
    }

    if (squares[0] == c && squares[3] == c && squares[6] == c){
      return 1;
    }
    if (squares[1] == c && squares[4] == c && squares[7] == c){
      return 1;
    }
    if (squares[2] == c && squares[5] == c && squares[8] == c){
      return 1;
    }

    if (squares[0] == c && squares[4] == c && squares[8] == c){
      return 1;
    }
    if (squares[2] == c && squares[4] == c && squares[6] == c){
      return 1;
    }
    return 0;
  }
  int check(){
    int cx = check_('X');
    int co = check_('O');

    //cout << cx;

    if (cx == 1 && co == 1){
      cout << "Error\n";
      return 0;
    }

    if (cx == 0 && co == 0){
      return 0;
    }

    if (cx == 1){
      return 1;
    }

    if (co == 1){
      return -1;
    }
    return -2;

  }
  Grid();
  Grid(string);
  void print(){
    cout << squares[0] << squares[1] << squares[2] << "\n"
	 << squares[3] << squares[4] << squares[5] << "\n"
	 << squares[6] << squares[7] << squares[8];
  }
};

Grid::Grid(){
  for (int i = 0; i < 9; ++i){
    squares.push_back('_');
  }
}
Grid::Grid(string s){
  if (s.size() != 9){
    cout << "String is of incorrect size.\n";
  }
  for (int i = 0; i < 9; ++i){
    squares.push_back(s[i]);
  }
}

int alphaCuts=0;
int betaCuts=0;

vector< vector< int > > order;
vector< vector< int > > move_cut_count;
vector< vector< char > > transposes;
vector< int > transpose_results;
int is_transpose(vector<char> a, vector<char> b){
  vector<char> b90 = b;
  b90[0] = b[6]; b90[1] = b[3]; b90[2] = b[0];
  b90[3] = b[7]; b90[4] = b[4]; b90[5] = b[1];
  b90[6] = b[8]; b90[7] = b[5]; b90[8] = b[2];
  int flag = 1;
  for (int i = 0; i < 9; ++i){
    if (b90[i] != a[i]){
      flag=0;
    }
  }
  if (flag == 1){
    return 1;
  }

  vector<char> b180 = b;
  b180[0] = b[8]; b180[1] = b[7]; b180[2] = b[6];
  b180[3] = b[5]; b180[4] = b[4]; b180[5] = b[3];
  b180[6] = b[2]; b180[7] = b[1]; b180[8] = b[0];

  for (int i = 0; i < 9; ++i){
    if (b180[i] != a[i]){
      flag = 0;
    }
  }

  if (flag == 1){
    return 1;
  }

  vector<char> b270 = b;
  b270[0] = b[2]; b270[1] = b[5]; b270[2] = b[8];
  b270[3] = b[1]; b270[4] = b[4]; b270[5] = b[7];
  b270[6] = b[0]; b270[7] = b[3]; b270[8] = b[6];

  for (int i = 0; i < 9; ++i){
    if (b270[i] != a[i]){
      return 0;
    }
  }

  return 1;

}

class GridTreeNode{
public:
  class PruningData{
  public:
    int value;
    int a;
    int b;
    PruningData(){
      a = -1;
      b = 1;
    }
  } pruningData;
  char type;
  GridTreeNode* source;
  vector<GridTreeNode*> dests;
  Grid grid;
  GridTreeNode* expand_one();
  void expand_all();
  void print_alpha_cut(){
    grid.print();
    cout << "  Alpha Cut\n\n";
  }
  void print_beta_cut(){
    grid.print();
    cout << "  Beta Cut\n\n";
  }
  int result(){
    int c = grid.check();
    //cout << "a"<<c;
    if (c != 0){
      //cout << c;
      return c;
    }
    expand_all();
    if (dests.size() == 0){
      //grid.print();
      return 0;
    }
    int tie_flag = 0;
    for (int i = 0; i < dests.size(); ++i){
      int r = dests[i]->result();
      if (r == type_to_int(type)){
	return type_to_int(type);
      }
      if (r == 0){
	tie_flag = 1;
      }
    }
    if (tie_flag == 1){
      return 0;
    }
    else{
      return type_to_int(other_type(type));
    }
  }
  void inherit(GridTreeNode* parent){
    pruningData.a = parent->pruningData.a;
    pruningData.b = parent->pruningData.b;
  }
  void pruning_update_node(int n){
    if (type == 'X'){
      if (pruningData.a < n){
	pruningData.a = n;
      }
    }
    if (type == 'O'){
      if (pruningData.b > n){
	pruningData.b = n;
      }
    }
  }
  int prune_condition(int result){
    if (type == 'O'){
      if (pruningData.a >= result){
	return 1;
      }
    }
    if (type == 'X'){
      if (pruningData.b <= result){
	return 1;
      }
    }
    return 0;
  }
  int pruning_result(){
    if (grid.check() != 0){
      return grid.check();
    }
    if (dests.size() == 0 && grid.moves_remaining() == 0){
      pruning_update_node(grid.check());
      return grid.check();
    }
    GridTreeNode* child;
    while (child = expand_one()){ //i.e. while child!=0
      child->inherit(this);
      int result = child->pruning_result();
      pruning_update_node(result);
      if (prune_condition(result)){
	child->pruning_update_node(result);
	if (type == 'X'){
	  betaCuts += 1;
	  //comment this line out to suppress printing of the cuts
	  print_beta_cut();
	}
	if (type == 'O'){
	  alphaCuts += 1;
	  //comment this line out to suppress printing of the cuts
	  print_alpha_cut();
	}
	break;
      }
			
    }
    if (type == 'X'){
      return pruningData.a;
    }
    if (type == 'O'){
      return pruningData.b;
    }

  }
  int depth;
  int move;
  int best_move(int n){
    int max = 0;
    int max_i = n;
		
    for (int i = n; i < move_cut_count[depth].size(); ++i){
      if (move_cut_count[depth][i] > max){
	max = move_cut_count[depth][i];
	max_i = i;
      }
    }
    return max_i;
  }
  int first;
  int second;
  int killer_pruning_result(){
    if (grid.check() != 0){
      return grid.check();
    }
    if (dests.size() == 0 && grid.moves_remaining() == 0){
      pruning_update_node(grid.check());
      return grid.check();
    }
    GridTreeNode* child;
    //int temp = 0;
    while (child = killer_expand_one()){ //i.e. while child!=0
      child->inherit(this);
      child->depth = depth + 1;
      int result = child->killer_pruning_result();
      pruning_update_node(result);
      if (prune_condition(result)){
	child->pruning_update_node(result);
	//move_cut_count[depth][child->move] = move_cut_count[depth][child->move]+1;
	move_cut_count[depth][mt-1] = move_cut_count[depth][mt-1] + 1;

	//temp = child->move;
	//cout << move_cut_count[depth][child->move];
	//cout << "here";
	//print_mcc();
	if (type == 'X'){
	  betaCuts += 1;
	  //comment this line out to suppress printing of the cuts
	  print_beta_cut();
	}
	if (type == 'O'){
	  alphaCuts += 1;
	  //comment this line out to suppress printing of the cuts
	  print_alpha_cut();
	}
	break;
      }

    }
    if (type == 'X'){
      return pruningData.a;
    }
    if (type == 'O'){
      return pruningData.b;
    }
  }
  GridTreeNode* expand_one_n(int n);
  //vector<int> order;
  GridTreeNode* killer_expand_one();
  void print_mcc(){
    for (int i=0; i < move_cut_count[depth].size(); ++i){
      cout << move_cut_count[depth][i] << ", ";
    }
    cout << "\n";
  }
  int mt;
  GridTreeNode(){
    depth = 0;
    mt = 0;
    first = 1;
    second = 1;
  }

  int transpose_pruning_result(){

    for (int i = 0; i < transposes.size(); ++i){
      if (is_transpose(grid.squares, transposes[i])){
	/*if (type == 'X'){
	  return pruningData.a;
	  }
	  if (type == 'O'){
	  return pruningData.b;
	  }*/
	numInvoke++;
	return transpose_results[i];
      }
			
    }

    if (grid.check() != 0){
      return grid.check();
    }
    if (dests.size() == 0 && grid.moves_remaining() == 0){
      pruning_update_node(grid.check());
      return grid.check();
    }
    GridTreeNode* child;
    while (child = expand_one()){ //i.e. while child!=0
      child->inherit(this);
      int result = child->transpose_pruning_result();
      pruning_update_node(result);
      if (prune_condition(result)){
	child->pruning_update_node(result);
	if (type == 'X'){
	  betaCuts += 1;
	  //comment this line out to suppress printing of the cuts
	  print_beta_cut();
	}
	if (type == 'O'){
	  alphaCuts += 1;
	  //comment this line out to suppress printing of the cuts
	  print_alpha_cut();
	}
	break;
      }

    }
    transposes.push_back(grid.squares);
    if (type == 'X'){
      transpose_results.push_back(pruningData.a);
      return pruningData.a;

    }
    if (type == 'O'){
      transpose_results.push_back(pruningData.b);
      return pruningData.b;
    }

  }
};

GridTreeNode* GridTreeNode::expand_one(){
	
  int k = dests.size();
  for (int i = 0; i < 9; ++i){
    if (grid.get(i) == '_'){
      if (k == 0){
	GridTreeNode* newNode = new GridTreeNode;
	newNode->grid = grid;
	newNode->grid.set(i, type);
	newNode->type = other_type(type);
	dests.push_back(newNode);
	numMoves++;
	return newNode;
      }
      k -= 1;
    }
  }
  return 0;
}

/*GridTreeNode* GridTreeNode::expand_one_n(int n){
	
  if (grid.get(n) == '_'){
  GridTreeNode* newNode = new GridTreeNode;
  newNode->grid = grid;
  newNode->grid.set(n, type);
  newNode->type = other_type(type);
  dests.push_back(newNode);
		
  return newNode;
  }
  return 0;
  }*/

vector<int> swap(vector<int> a, int index1, int index2){
  int temp = a[index1];
  a[index1] = a[index2];
  a[index2] = temp;
  return a;
}

GridTreeNode* GridTreeNode::killer_expand_one(){
	
  if (order.size() <= depth){
    vector<int> o;
    vector<int> m;
    for (int i = 0; i < 9; ++i){
      o.push_back(i);
    }
    for (int i = 0; i < 9; ++i){
      m.push_back(0);
    }
    order.push_back(o);
    move_cut_count.push_back(m);
  }
  if (first == 1){
    int sd = 2;
    // Arrange by number of cuts they cause
    for (int w = 0; w < order[depth].size(); ++w){
      order[depth] = swap(order[depth], 8-w, best_move(w));
    }

    // Use move that does most cuts first
    //order[depth] = swap(order[depth], 0, best_move(0));

  }
  first = 0;
  //print_mcc();
  int k = dests.size();
  for (int i = 0; i < 9; ++i){
    if (grid.get(order[depth][i]) == '_'){
      if (k == 0){
	GridTreeNode* newNode = new GridTreeNode;
	newNode->grid = grid;
	newNode->move = mt;
	mt++;
	newNode->grid.set(order[depth][i], type);
	newNode->type = other_type(type);
	dests.push_back(newNode);
	numMoves++;
	return newNode;
      }
      k -= 1;
    }
  }
  return 0;
}

void GridTreeNode::expand_all(){
  while (expand_one() != 0);
}

class GridTree{
  GridTreeNode* start;

};

int game_result(string s){
  numMoves = 0;
  GridTreeNode* n = new GridTreeNode;
  n->grid = Grid(s);
  n->type = 'X';
  //cout << n->grid.to_string() << "\n";
  return n->result();

}

int prune(string s){
  numMoves = 0;
  alphaCuts = 0;
  betaCuts = 0;
  GridTreeNode* n = new GridTreeNode;
  n->grid = Grid(s);
  n->type = 'X';
  //cout << n->grid.to_string() << "\n";
  return n->pruning_result();

}

int killer(string s){
  numMoves = 0;
  alphaCuts = 0;
  betaCuts = 0;
  GridTreeNode* n = new GridTreeNode;
  n->grid = Grid(s);
  n->type = 'X';
  //cout << n->grid.to_string() << "\n";
  return n->killer_pruning_result();

}

char countXO(string s){
  int X = 0;
  int O = 0;
  for (int i = 0; i < s.length(); ++i){
    if (s[i] = 'X'){
      ++X;
    }
    if (s[i] = 'O'){
      ++O;
    }
  }

  if (O < X){
    return 'O';
  }
  else{
    return 'X';
  }
}

int transpose(string s){
  numInvoke = 0;
  numMoves = 0;
  alphaCuts = 0;
  betaCuts = 0;
  GridTreeNode* n = new GridTreeNode;
  n->grid = Grid(s);
  n->type = countXO(s);
  //cout << n->grid.to_string() << "\n";
  return n->transpose_pruning_result();

}

int main(int argc, char* argv[]){

  // Part A
  string s;
  if (argc >= 2){
    s = argv[1];
  }
  else{
    cout << "example usage: " << argv[0] << " O_XOXX___\n";
    return 1;
  }
  // Part B
  cout << "Game Result: " << game_result(s) << "\n";
  cout << "Moves considered without alpha - beta pruning : " << numMoves << "\n\n";

  // Part C and D
  cout << "Game Result: " << prune(s) << "\n";
  cout << "Moves considered with alpha - beta pruning : " << numMoves << "\n";
  cout << "Alpha Cuts: " << alphaCuts<<"\n";
  cout << "Beta Cuts: " << betaCuts << "\n\n";

  // Part E
  cout << "Game Result: " << killer(s) << "\n";
  cout << "Moves considered with killer heuristic : " << numMoves << "\n";
  cout << "Alpha Cuts: " << alphaCuts << "\n";
  cout << "Beta Cuts: " << betaCuts << "\n\n";

  // Part F
  //Transpose takes a while when there are many blank tiles
	
  cout << "Game Result: " << transpose(s) << "\n";
  cout << "Moves considered with transpose : " << numMoves << "\n";
  cout << "Alpha Cuts: " << alphaCuts << "\n";
  cout << "Beta Cuts: " << betaCuts << "\n";
  cout << "Rotation invariance invoked: " << numInvoke <<"\n\n";
  return 0;
}
