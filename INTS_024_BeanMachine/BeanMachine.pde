/////////////////////////////////
//      for bean machine       //
/////////////////////////////////
PImage img;  //background image
int lyr=10;  //pin layer
int w=lyr*2-1;  //max pin (the bottom layer)
float wide=15;  //wide for each two pins
float r=2;  //multiplier for "wide"
boolean [][] pin=new boolean[lyr][w];  //pin array

/////////////////////////////////
//          for marbles        //
/////////////////////////////////
//"speed" should be the factor of "wide*r"
float speed=15;  //marble falling speed
float [] cnt=new float[lyr+10]; //number of marbles fell into the specific buckets 
int totalCnt=0;  //number of marbles fell to the bottom layer
ArrayList<Marble>marbles=new ArrayList<Marble>();  //marbles arraylist
int MaxNbMarble=100;  //number of marbles dropped at once
float prob=0.5; //probability for falling direction=1 (fall to right)


/////////////////////////////////
//     marble structure        //
/////////////////////////////////
class Marble{
  float x,y;  //marble position
  int dir=0;  //marble horizontal direction (left or right)
  float dy=speed;  //vertical speed
  float dx=0;  //horizontal speed
  
  //initialization
  Marble(float _x,float _y){
    x=_x;
    y=_y;  
  }
  
  //draw the marble at (x,y) with speed (dx,dy)
  void show(){
    x+=dx;
    y+=dy;
    stroke(255,0,0);
    point(x,y);
  }
  
  //update the marble with its horizontal speed dx
  void update(){
    for(int i=0;i<lyr;i++){
      for(int j=0;j<w;j++){
        //a marble "fell on" (not fell to) one pin
        //  o                     [        ]        [       ]
        //         =>   o     =>  [        ]    or  [       ]    (determine "o" goes to left or right)
        //  x           x         [ o x    ]        [   x o ]
        //x   x       x   x       [ x   x  ]        [ x   x ]
           
        //fall on            fall to
        //           V.S.
        //   o              
        //   x                 o
        // x   x             x   x
        
        //position on one pin: (wide*r*j,wide*r*i-speed)
        //marble position: (x,y)
        if(pin[i][j] && dist(wide*r*j,wide*r*i-speed,x,y)<1){
          //update the horizontal speed
          dir=(random(0,1)>prob)?-1:1;
          dx=speed;
          dx*=dir;
        }
      }
    } 
  }
}

/////////////////////////////////
//       initialization        //
/////////////////////////////////
void setup(){
  size(850,650);
  
  //pin initialization
  for(int i=0;i<lyr;i++){
    for(int j=0;j<w;j++){
      pin[i][j]=false; 
    }
  }
  
  //bean machine setup
  for(int i=0;i<lyr;i++){
    int cpos=lyr-1-i;
    for(int j=0;j<i+1;j++){
      pin[i][cpos]=true;
      cpos+=2;
    }
    /*
    //another bean machine
    int cpos=(i%2==0)?0:1;
    for(int j=0;j<lyr-1;j++){
      pin[i][cpos]=true;
      cpos+=2;
    }
    */
  }
  
  //cnt initialization
  for(int i=0;i<lyr+1;i++){
    cnt[i]=0;
  }
  
  //drop MaxNbMarble marbles at once 
  for(int i=0;i<MaxNbMarble;i++){
    marbles.add(new Marble(wide*r*(lyr-1),wide*r*0-10*speed));
  }
   
  //background
  img=loadImage("face.bmp");
  img.resize(850,650);
}

/////////////////////////////////
//          bean machine       //
//  distribution simulation    //
/////////////////////////////////
void draw(){
  
  //draw background
  background(191);
  tint(255,60);
  image(img,0,0);
  
  //global position adjustment
  //width of the bean machine: wide*r*w
  //height of the bean machine: wide*r*lyr
  translate((width-wide*r*w)/2.0,(height-wide*r*lyr)/2.0);
  
  //draw bean machine
  strokeWeight(10);
  for(int i=0;i<lyr;i++){
    for(int j=0;j<w;j++){
      if(pin[i][j]){
        //black color
        stroke(0);
        //spacing between each two pins: wide*r
        point(wide*r*j,wide*r*i);
      }
    }
  }
  
  //draw buckets for filling the resultant marbles
  for(int i=-2;i<w+2;i+=2){
    strokeWeight(5);
    noFill();
    line(wide*r*i,wide*r*lyr,wide*r*i,wide*r*lyr+wide*10);
  }
  line(wide*r*(-2),wide*r*lyr+wide*10,wide*r*(w+1),wide*r*lyr+wide*10);
  
  //update marbles
  strokeWeight(11);
  for(int i=0;i<marbles.size();i++){
    Marble b=marbles.get(i);
    b.show();
    b.update();
    //the marble fell to the bottom layer
    if(b.y>wide*r*lyr-speed){
      //remove that marble
      marbles.remove(i);
      //count the number of marbles fell into the specific bucket
      cnt[ceil((int(b.x/(wide*r))+1)/2.0)]++;
      //create a new marble
      marbles.add(new Marble(wide*r*(lyr-1),wide*r*0-10*speed));
      //count the number of marbles fell to the bottom layer
      totalCnt++;  
    }
  }
  
  //rescale cnt
  for(int i=0;i<lyr+1;i++){
    if(cnt[i]>10){
      float cvalue=cnt[i];
      for(int j=0;j<lyr+1;j++){
        cnt[j]=cnt[j]/cvalue*10.0;
      }
    }
  }
  
  //draw the distribution
  for(int i=-1;i<w;i+=2){
    stroke(0,0,255);
    strokeWeight(5);
    line(wide*r*i,wide*r*lyr+wide*10-wide*cnt[(i-(-1))/2],wide*r*(i+2),wide*r*lyr+wide*10-wide*cnt[((i+2)-(-1))/2]);
    //point(wide*r*i,wide*r*lyr+wide*10-wide*cnt[(i-(-1))/2]);
  }
  
  //show the total count of the marbles fell to the bottom layer
  textAlign(CENTER,CENTER);
  textSize(width/10);
  text(totalCnt,wide*r*(lyr-1),-100);
}
