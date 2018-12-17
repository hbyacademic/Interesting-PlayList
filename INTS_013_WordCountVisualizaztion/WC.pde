IntDict WC = new IntDict();

void setup()
{
  //the size of canvas & background
  size(600,605);
  background(200);
  
  //read the contents of a file & create an array of its individual lines
  String [] getlines = loadStrings("input.txt");
  
  //combine an array of strings into one string, each separated by "\n"
  String multiLines = join(getlines, "\n");
  
  //split tokens by delimiters
  String[] token = splitTokens(multiLines,"\n .,;?!()[]`'");
  
  //creat the set(key, value)
  //key represents token
  //value represents the word count of the token
  for(int i=0; i<token.length; i++)
  {
    String ts=token[i].toLowerCase();
    
    //increment the word count of the token by 1 if finding it again
    if(WC.hasKey(ts))
      WC.increment(ts);
    else
      //set the word count of each token to 1 
      WC.set(ts,1);
  }
  
  //sort by values in desending order
  WC.sortValuesReverse();
  
  //copy the keys array to keys[]
  String [] keys = WC.keyArray();

  int totalCnt=0, wordX=0, wordY=0, rectY=5, scale=20, rectGap=20, rectHeight=rectGap-5; 
  float ratio;
  
  //calculate the total word count in token[]
  for(String str : keys)
    totalCnt+=WC.get(str);

  for(int i=0; i<keys.length; i++)
  {
    //disable drawing the stroke (outline)
    noStroke();
    
    //show the top 35 at most
    if(i<35)
    {
      //the ratio of each key
      ratio=WC.get(keys[i])*1.0/totalCnt*100;
      
      //fill in different colors
      if(ratio>0)
        fill(#8B00FF);
      if(ratio>1)
        fill(0,0,255);
      if(ratio>2)
        fill(0,255,0);
      if(ratio>3)
        fill(255,255,0);
      if(ratio>4)
        fill(#FFA500);
      if(ratio>5)
        fill(255,0,0);
  
      //histogram
      //WC.get(keys[i]) => get the value of keys[i]
      rect(0, rectY, WC.get(keys[i])*scale, rectHeight);
  
      fill(0);
      textSize(rectGap);
      
      wordX=WC.get(keys[i])*scale+rectGap/2;
      
      //show the word counts & ratio
      text(keys[i] + " : " + WC.get(keys[i]) + " (" + (nf(ratio,0, 2)) + "%)", wordX, wordY+rectGap);
      wordY+=rectGap;
      rectY+=rectGap;
    }  
  }
}
