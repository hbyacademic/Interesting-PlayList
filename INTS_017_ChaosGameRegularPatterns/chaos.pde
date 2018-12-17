float x,y;
float radius=250;
float midX,midY;
int NumOfcolor=7;
color [] c = {#FF0000, #00FF00, #0000FF, #FFFF00, #FF00FF, #00FFFF, #808080};

//key pararmeters setup
int NumOfpoints=10;
float [][] PointSet = new float[NumOfpoints][2];
float r=1.0/5.0;
float ratio=1-r;

void setup(){
  background(192);
  size(600,600); 
  strokeWeight(2);
  stroke(0);
  
  //center of the circle
  midX=width/2;
  midY=width/2;
  
  //generate some seed points
  for(int i=0;i<NumOfpoints;i++){
    PointSet[i][0] = midX+radius*cos(2*PI/NumOfpoints*i);
    PointSet[i][1] = midY-radius*sin(2*PI/NumOfpoints*i);
    point(PointSet[i][0],PointSet[i][1]);
  } 
  
  //one point for generating fractal pattern
  x=int(random(width));
  y=int(random(width));
  
  fill(192);
  ellipse(midX,midY,2*radius,2*radius); 
}


void draw(){
  //speedup  
  for(int i=0;i<100;i++){
    int dice=int(random(NumOfpoints));
    stroke(c[dice%NumOfcolor]);
    
    //x=lerp(x1,x2,ratio)
    //      ratio      (1-ratio)
    // |->        <-|->         <-|
    //x1............x............x2
    x=lerp(x,PointSet[dice][0],ratio);
    y=lerp(y,PointSet[dice][1],ratio);
    point(x,y);
  }
}
