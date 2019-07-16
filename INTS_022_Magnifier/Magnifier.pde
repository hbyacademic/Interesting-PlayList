PImage img;
void setup(){
  size(720,480);
  img=loadImage("face.bmp");
  img.resize(720,480);
  rectMode(CENTER); noFill(); 
  textAlign(CENTER,CORNER);  textSize(50);
}

void draw(){  
  image(img,0,0); 
  int px=mouseX, py=mouseY, border=81, zoomFactor=2;
  //make sure we haven't walked out off the edge
  px=constrain(px,border/2,img.width-1-border/2); 
  py=constrain(py,border/2,img.height-1-border/2);
  
  //the selected region will be copied and rescaled 
  //copy(src,sx,sy,sw,sh,dx,dy,dw,dh)
  //src: source image 
  //(sx,sy): upper left corner of the source image
  //sw (sh): source image width (height)
  //(dx,dy): upper left corner of the destination image
  //dw (dh): destination image width (height)
  copy(img,px-(border/zoomFactor)/2,py-(border/zoomFactor)/2,
  border/zoomFactor,border/zoomFactor,px-border/2,py-border/2,border,border);
  
  strokeWeight(2); rect(px,py,81,81);
  strokeWeight(30); text("Zoom factor: "+zoomFactor,360,50);
}
