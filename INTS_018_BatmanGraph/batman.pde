void setup(){
  //3D scene
  size(700,600,P3D);
  strokeWeight(5);
  textSize(100);
  textAlign(CENTER,CENTER);
}

int width=700,height=600;
float x,y,scale=30;
int cnt=0, INT_MAX=2147483647; 

void show(float degree){

  rotateY(degree*cnt);
  
  //component 1
  for(x=3*scale;x<=7*scale;x+=0.01){
    y=(3*scale)*sqrt(-pow((x/(7*scale)),2)+1);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  for(x=-(7*scale);x<=-(3*scale);x+=0.01){
    y=(3*scale)*sqrt(-pow((x/(7*scale)),2)+1);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  
  //component 2
  for(x=(4*scale);x<=(7*scale);x+=0.01){
    y=-(3*scale)*sqrt(-pow((x/(7*scale)),2)+1);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  for(x=-(7*scale);x<=-(4*scale);x+=0.01){
    y=-(3*scale)*sqrt(-pow((x/(7*scale)),2)+1); 
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  
  //component 3
  for(x=-200;x<=200;x+=0.01){
    y=abs(x/2.0)-(3*sqrt(33)-7)*(x*x)/(112*scale)+sqrt(scale*scale-pow((abs(abs(x)-2*scale)-1*scale),2))-3*scale;
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  
  //component 4
   for(x=(0.75*scale);x<(1*scale);x+=0.01){
    y=(9*scale)-8*abs(x);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  for(x=-(1*scale);x<-(0.75*scale);x+=0.01){
    y=(9*scale)-8*abs(x);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  
  //component 5
  for(x=(0.5*scale);x<=(0.75*scale);x+=0.01){
    y=3*abs(x)+(0.75*scale);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  for(x=-(0.75*scale);x<=-(0.5*scale);x+=0.01){
    y=3*abs(x)+(0.75*scale);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  
  //component 6
  for(x=-(0.5*scale);x<=(0.5*scale);x+=0.01){
    y=2.25*scale;
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  
  //component 7
  for(x=(1*scale);x<=(3*scale);x+=0.01){
    y=1.5*scale-0.5*abs(x)-(6*sqrt(10)/14.0)*(sqrt(3*scale*scale-x*x+2*scale*abs(x))-2*scale);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
  for(x=-(3*scale);x<=-(1*scale);x+=0.01){
    y=1.5*scale-0.5*abs(x)-(6*sqrt(10)/14.0)*(sqrt(3*scale*scale-x*x+2*scale*abs(x))-2*scale);
    stroke(random(0,256),random(0,256),random(0,256));
    point(x,-y);
  }
}

void draw(){
  background(192);
  
  //translate (0,0) to (width/2,height/2)
  translate(width/2,height/2);
  
  cnt++;
  cnt%=(INT_MAX);
  
  fill(0);
  text("Batman", 0, 100-height/2);
  text("Graph", 0, 450-height/2);
  
  //rotate along y-axis per PI/9 degrees
  show(PI/9); 
}
