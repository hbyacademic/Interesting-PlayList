//width of the rectangle is w
//height of the rectangle is h
int w=50;
int h=10;

//gap in y direction between each two SSDs
int gap=8;

//the top left point of "segment A" in the leftmost SSD 
int initPosX=(800-(6*w+14*h+15*gap))/2,initPosY=(300-2*w-3*h)/2;

//the switch status of numbers from 0 to 9 
int [] tab = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};

//counters for six SSDs (the order is from left to right)
//H: hour; M:minute; S:second
//L: left; R:right
int cntHL=0, cntHR=0, cntML=0, cntMR=0, cntSL=0, cntSR=0;
            
void setup(){
  size(800,300);
  //speed
  frameRate(1);
  background(16);
}

void SevenSegmentDisplay(int posX, int posY, int num){ 
  
  //lookup status table
  int idx=tab[num];
  noStroke();  
  
  /************************************************************************************
 
  ==========================
  |                        |
  |   first       second   |         first       second           first       second
  |    SSD         SSD     |          SSD         SSD               SSD         SSD
  |                        |
  |  ----A---    ----A---  |        ----A---    ----A---         ----A---    ----A---
  |  |      |    |      |  |        |      |    |      |         |      |    |      |
  |  F      B    F      B  |   0    F      B    F      B    0    F      B    F      B
  |  |      |    |      |  |        |      |    |      |         |      |    |      |
  |  ----G---    ----G---  |        ----G---    ----G---         ----G---    ----G---
  |  |      |    |      |  |        |      |    |      |         |      |    |      |
  |  E      C    E      C  |   0    E      C    E      C    0    E      C    E      C
  |  |      |    |      |  |        |      |    |      |         |      |    |      |
  |  ----D---    ----D---  |        ----D---    ----D---         ----D---    ----D---      
  |                        |
  ==========================->group
  
  *************************************************************************************/
  
  //F  
  fill(((idx>>1) & 1)*223+32,(1-((idx>>1) & 1))*32,(1-((idx>>1) & 1))*32);
  rect(posX-h,posY+h+gap,h,w);  
  //A
  fill(((idx>>6) & 1)*223+32,(1-((idx>>6) & 1))*32,(1-((idx>>6) & 1))*32);
  rect(posX,posY,w,h); 
  //B
  fill(((idx>>5) & 1)*223+32,(1-((idx>>5) & 1))*32,(1-((idx>>5) & 1))*32);
  rect(posX+w,posY+h+gap,h,w);
  //E
  fill(((idx>>2) & 1)*223+32,(1-((idx>>2) & 1))*32,(1-((idx>>2) & 1))*32);
  rect(posX-h,posY+2*h+w+3*gap,h,w);
  //G
  fill(((idx) & 1)*223+32,(1-((idx) & 1))*32,(1-((idx) & 1))*32);
  rect(posX,posY+h+w+2*gap,w,h);
  //C
  fill(((idx>>4) & 1)*223+32,(1-((idx>>4) & 1))*32,(1-((idx>>4) & 1))*32);
  rect(posX+w,posY+2*h+w+3*gap,h,w);
  //D
  fill(((idx>>3) & 1)*223+32,(1-((idx>>3) & 1))*32,(1-((idx>>3) & 1))*32);
  rect(posX,posY+2*h+2*w+4*gap,w,h);
}

void circle(int posX, int posY){ 
  noStroke();
  fill(255,0,0);
  //upper circle
  ellipse(posX,posY,h,h);
  //lower circle
  ellipse(posX,posY+w+h,h,h);
}

void draw(){
  
  //HOUR
  //gap between, two SSDs which are in the same group, is (w+h+gap+h)
  SevenSegmentDisplay(initPosX, initPosY, cntHL); 
  SevenSegmentDisplay(initPosX+w+2*h+gap, initPosY, cntHR);
  //gap between SSD and circle is (w+h+4*gap)
  circle(initPosX+2*w+3*h+5*gap,initPosY+w/2+h+h/2);
  
  //gap between circle and first SSD is (2*gap+2*h)
  
  //MINUTE
  SevenSegmentDisplay(initPosX+2*w+5*h+7*gap,initPosY, cntML);
  SevenSegmentDisplay(initPosX+3*w+7*h+8*gap, initPosY, cntMR);
  circle(initPosX+4*w+8*h+12*gap,initPosY+w/2+h+5);
  
  //SECOND
  SevenSegmentDisplay(initPosX+4*w+10*h+14*gap, initPosY, cntSL);
  SevenSegmentDisplay(initPosX+5*w+12*h+15*gap, initPosY, cntSR);
  
  cntSR++;
  //if >= 10s, 00:00:09 => 00:00:10
  if(cntSR==10)
  {
    cntSL++;
    cntSR=0;
  }
  
  //if >= 60s, 00:00:59 => 00:01:00
  if(cntSL==6)
  {
    cntMR++;
    cntSL=0;
  }
  
  //if >= 10min, 00:09:59 => 00:10:00
  if(cntMR==10)
  {
    cntML++;
    cntMR=0;
  }
  
  //if >= 60min, 00:59:59 => 01:00:00
  if(cntML==6)
  {
    cntHR++;
    cntML=0;
  }
  
  //if >= 10hr, 09:59:59 => 10:00:00
  if(cntHR==10)
  {
    cntHL++;
    cntHR=0;
  }
  
  // if >= 24hr, 23:59:59 => 00:00:00
  if(cntHL==2 && cntHR==4)
  {
    cntHL=0;
    cntHR=0;
  }
}
