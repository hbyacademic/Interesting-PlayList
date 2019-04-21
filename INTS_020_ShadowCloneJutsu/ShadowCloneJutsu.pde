PImage img;
void setup(){
  size(720,480);
  img=loadImage("face.bmp");
  img.resize(720,480);
  frameCount=1;
}

//do vertical division first, and then horizontal division
//slcing width => vgap (hgap)
PImage shadowCloneJutsu(PImage img, int vgap, int hgap){

  //original image -> mid -> output
  //the result of doing vertical division
  PImage mid=createImage(img.width,img.height,RGB);
  //the result of doing vertical & horizontal division
  PImage output=createImage(img.width,img.height,RGB);
  //number of division in horizontal direction
  int nbxpart=img.width/vgap;  
  //number of division in vertical direction
  int nbypart=img.height/hgap;
  
  ///////////////////////////////
  ////   vertical division  /////
  ///////////////////////////////
  //for exapmle, 01234567 (horizontal division is the same concept as vertical division)
  //even-label part: 0246
  // odd-label part: 1357
  // 01234567 after vertical division -> 02461357
  for(int i=0;i<nbxpart;i++){
    for(int y=0;y<img.height;y++){
      //even-label part
      if(i%2==0){
        for(int x=i*vgap, nx=int(i/2)*vgap; x<int(i+1)*vgap && nx<(int(i/2)+1)*vgap; x++,nx++){
          mid.pixels[y*img.width+nx]=img.pixels[y*img.width+x];
        }
      }
      //odd-label part
      else{
        for(int x=i*vgap, nx=(int(i/2)+int(nbxpart/2))*vgap; x<int(i+1)*vgap && nx<(int(i/2)+int(nbxpart/2)+1)*vgap; x++,nx++){
          mid.pixels[y*img.width+nx]=img.pixels[y*img.width+x];
        }
      }
    }
  }
  
  ///////////////////////////////
  ////  horizontal division  ////
  ///////////////////////////////
  for(int i=0;i<nbypart;i++){
    for(int x=0; x<img.width; x++){
      //even-label part
      if(i%2==0){
        for(int y=i*hgap, ny=int(i/2)*hgap; y<int(i+1)*hgap && ny<(int(i/2)+1)*hgap; y++,ny++){
          output.pixels[ny*img.width+x]=mid.pixels[y*img.width+x];
        }
      }
      //odd-label part
      else{
        for(int y=i*hgap, ny=(int(i/2)+int(nbypart/2))*hgap; y<int(i+1)*hgap && ny<(int(i/2)+int(nbypart/2)+1)*hgap; y++,ny++){
          output.pixels[ny*img.width+x]=mid.pixels[y*img.width+x];
        }
      }
    }
  }
  return output;
}

void draw(){
  int SlicingGap=1;
  if(frameCount%100==0)
    img=shadowCloneJutsu(img,SlicingGap,SlicingGap);
  image(img,0,0);
}
