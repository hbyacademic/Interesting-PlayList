PImage bg;
void setup(){
  size(729,700);
}

void Carpet(int r,int group){
  bg=createImage(729,729,ALPHA);
  
  for(int dy=-r/2;dy<=r/2;dy++){
    for(int dx=-r/2;dx<=r/2;dx++){
      int currentX=bg.width/2+dx;
      int currentY=bg.height/2+dy;
      int px=currentX,py=currentY;
      boolean IsCenter=false;
      while(currentX>0 || currentY>0){
        if(currentX%group==(group/2) && currentY%group==(group/2)){
          IsCenter=!IsCenter;
          break;
        }
        currentX/=group;
        currentY/=group;
      }
      if(IsCenter)
        bg.pixels[py*bg.width+px]=color(0);
      else
        bg.pixels[py*bg.width+px]=color(255);
    }
  }     
  image(bg,0,0); 
}

int i=3;
void draw(){
 Carpet(i,3);
 i+=2;
 if(i==bg.width){
   save("output.png");
   noLoop();
 }
 i%=bg.width;
}
