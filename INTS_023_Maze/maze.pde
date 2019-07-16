int N=20; //the number of rows & columns of the maze
int [] L = new int[N+1];
int [] R = new int[N+1];
//the cell (N,N) with four direction walls (up, dowm, left, right)
boolean [][][] lines = new boolean [N][N][4];

//cells
// =================
// | (0,0) | (0,1) | ....
// =================
// | (1,0) | (1,1) |
// =================
// .....

//walls of the cell (i,j)
//    up  -> =========
//  left  -> | (i,j) | <- right 
//  down  -> =========

void generateMaze(){
  //initial L & R
  for(int i=0;i<N;i++){
    L[i]=i;  R[i]=i;
  }
  //if N=5, L={0,1,2,3,4,5,5}
  //                       ^
  L[N]=N-1;
  
  //all walls
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      for(int k=0;k<4;k++){
        lines[i][j][k]=true;
      }
    }
  }
  
  //generate each row (excluding the last row) of the maze
  for(int r=0;r<N-1;r++){
    for(int i=0;i<N;i++){
      
      //remove vertical wall
      //(r-th row) the cell (r,i) and the cell (r,j)
      // =========         =========
      // | i | j |    =>   | i   j |
      // =========         =========
      
      int j=L[i+1];
      //"i!=j" ===> we cannot remove the right wall of the last cell in each row
      //"random(0,1)>0.5" ===> probability = 1/2 for removing the vertical wall 
      if(i!=j && random(0,1)>0.5){
        //link L[j] to R[i] (i.e. connect cell (r,i) and cell (r,j))
        R[L[j]]=R[i];  L[R[i]]=L[j];
        //link i to j
        R[i]=j;  L[j]=i;
        
        //remove the right wall of the cell (r,i) & the left wall of the cell (r,j)
        // =========         =========
        // | i | j |    =>   | i   j |
        // =========         =========
        //     ^
        //     |_____ right wall of the cell (r,i) = left wall of the cell (r,j)
        lines[r][i][3]=false;
        lines[r][i+1][2]=false;
      }
      
      //construct horizontal wall
      //                 =====        =====
      //r-th row         | i |        | i |
      //                 |   |   =>   =====
      //(r+1)-th row     | i |        | i | 
      //                 =====        =====
      
      //"i!=L[i]" ===> we cannot construct the horizontal wall if the cell has the right wall
      // =====
      // | i |
      // ===== <- if we construct the horizontal wall, then this cell will be isolated
      //"random(0,1)>0.5" ===> probability = 1/2 for constructing the horizontal wall 
      if(i!=L[i] && random(0,1)>0.5){
        //link L[i] to R[i]
        R[L[i]]=R[i];  L[R[i]]=L[i];
        //link i to i
        R[i]=i;  L[i]=i;
      } 
      else{
        //remove the down wall of the cell (r,i) & the up wall of cell (r+1,i)
        lines[r][i][1]=false;
        lines[r+1][i][0]=false; 
      }
   }
  }
  
  //the last row of the maze (similar to the other rows)
  for(int i=0;i<N;i++){
    int j=L[i+1];
    if(i!=j && (i==L[i] || random(0,1)>0.5)){
      //link L[j] to R[i]
      R[L[j]]=R[i];  L[R[i]]=L[j];
      //link i to j
      R[i]=j;  L[j]=i; 
      
      lines[N-1][i][3]=false;
      lines[N-1][i+1][2]=false;
    }
    //link L[i] to R[i]
    R[L[i]]=R[i];  L[R[i]]=L[i];
    //link i to i
    R[i]=i;  L[i]=i;
  }
  
  //remove the left wall of the cell(0,0)
  lines[0][0][2]=false;
  //remove the right wall of the cell(N-1,N-1)
  lines[N-1][N-1][3]=false;
}

//draw the maze
void drawMaze(int i,int j){
  int gap=27,px=(width-gap*N)/2,py=(height-gap*N)/2;
  //stroke(random(255),random(255),random(255));
  if(lines[i][j][0])  line(      px+gap*j,       py+gap*i, (px+gap)+gap*j,       py+gap*i); //up
  if(lines[i][j][1])  line(      px+gap*j, (py+gap)+gap*i, (px+gap)+gap*j, (py+gap)+gap*i); //down
  if(lines[i][j][2])  line(      px+gap*j,       py+gap*i,       px+gap*j, (py+gap)+gap*i); //left
  if(lines[i][j][3])  line((px+gap)+gap*j,       py+gap*i, (px+gap)+gap*j, (py+gap)+gap*i); //right
}

void setup(){
  strokeWeight(10);
  size(600,600);
  frameRate(20);
}

boolean Once=true;
int i=0,j=0;
void draw(){
  if(Once){ generateMaze(); Once=!Once;}
  drawMaze(i%N,j++); //draw the cells seqentially
  if(j==N){ j%=N; i++;}
}
