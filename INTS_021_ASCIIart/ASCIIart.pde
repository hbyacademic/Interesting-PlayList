PImage img;
void setup(){
  size(720,480);
  img=loadImage("face.bmp");
  img.resize(720,480);
  frameRate(2);
}

//printable characters
String str=".`-_':,;^=+/\"|)\\<>)iv%xclrs{*}I?!][1taeo7zjLunT#JCwfy325Fp6mqSghVd4EgXPGZbYkOA&8U$@KHDBWNMR0Q";

void ASCII_art(PImage input, int TextSize){            
  //transformation from RGB to grayscale
  input.filter(GRAY);
  textAlign(CENTER,CENTER);
  fill(0); strokeWeight(50);
  textSize(TextSize);
  
  //transformation from grayscale to ASCII art
  for(int y=0;y<input.height;y+=(TextSize-2)){
    for(int x=0;x<input.width;x+=(TextSize-2)){
      //remap the grayscale color to printable character
      int quant=int(map(green(int(input.pixels[y*input.width+x])),0,255,0,str.length()-1));
      text(str.charAt(quant),x,y);
    }
  }
}

int TextSize=4,cnt=1;
void draw(){
 background(192);
 //5 6 7 8 9 10 11 12 13 14 15 14 13 12 11 10 9 8 7 6 5 4....
 TextSize=TextSize+cnt;
 if((TextSize%15)==0 || (TextSize%15)==4) cnt*=-1;
 
 ASCII_art(img,TextSize); 
 textAlign(LEFT,CORNER); fill(0,0,255); strokeWeight(50); textSize(30);
 text("TextSize: "+TextSize,120,50);
 text("Sampling rate: "+nf(1.0/((TextSize-2)*(TextSize-2)),1,3),40,100);
 //saveFrame("######.png");
}
