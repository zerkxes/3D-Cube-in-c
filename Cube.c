#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define WIDTH  50
#define HEIGHT 50
#define CUBE_DIST 10 
#define FOV 30


typedef struct {
  float x;
  float y;
  float z;
} cube;

char output[WIDTH][HEIGHT];

   cube matrix[][5] = {
    //bottom
    {
        { .x = -0.5, .y = 0.5, .z = -0.5 },
        { .x = 0.5, .y = 0.5, .z = -0.5 },
        { .x = 0.5, .y = 0.5, .z = 0.5 },
        { .x = -0.5, .y = 0.5, .z = 0.5 },
        { .x = -0.5, .y = 0.5, .z = -0.5 },
    },
    // Top
    {
        { .x = -0.5, .y = -0.5, .z = -0.5 },
        { .x = 0.5, .y = -0.5, .z = -0.5 },
        { .x = 0.5, .y = -0.5, .z = 0.5 },
        { .x = -0.5, .y = -0.5, .z = 0.5 },
        { .x = -0.5, .y = -0.5, .z = -0.5 },
    },
    // Front
    {
        { .x = -0.5, .y = -0.5, .z = 0.5 },
        { .x = 0.5, .y = -0.5, .z = 0.5 },
        { .x = 0.5, .y = 0.5, .z = 0.5 },
        { .x = -0.5, .y = 0.5, .z = 0.5 },
        { .x = -0.5, .y = -0.5, .z = 0.5 },
    },
    // Back
    {
        { .x = -0.5, .y = -0.5, .z = -0.5 },
        { .x = 0.5, .y = -0.5, .z = -0.5 },
        { .x = 0.5, .y = 0.5, .z = -0.5 },
        { .x = -0.5, .y = 0.5, .z = -0.5 },
        { .x = -0.5, .y = -0.5, .z = -0.5 },
    },
  };

//void plotLineLow(int x0,int y0, int x1, int y1) {
//  int dx = x1-x0;
//  int dy = y1-y0;
//  int yi=1;
//
//  if ( dy < 0 ) {
//    yi = -1;
//    dy = -dy;
//  }
//  int D = ( 2*dy ) - dx;
//  int y = y0;
//  for( int x = x0; x < x1; x++) {
//    output[x][y]='@';
//    if ( D > 0 ) {
//      y += yi;
//      D+= 2 * (dy-dx);
//    }
//    else
//      D+= 2 * dy;
//  }
//}
//
//void plotLineHigh(int x0, int y0, int x1, int y1) {
//  int dx = x1-x0;
//  int dy = y1-y0;
//  int xi=1;
//
//  if ( dx < 0 ) {
//    xi=-1;
//    dx=-dx;
//  }
//  int D = 2*dx -dy;
//  int x=x0;
//
//  for(int y = y0; y < y1; y++) {
//    output[x][y]='@';
//    if( D>0 ) {
//      x+=xi;
//      D+=2*(dx-dy);
//    }
//    else
//      D+=2*dx;
//  }
//}
//
//void lineTo(int x0, int y0, int x1, int y1) {
//  output[x0][y0]='@';
//  output[x1][y1]='@';
//  if (abs(y1-y0) > abs(x1-x0)){
//    if( x0>x1 )
//      plotLineLow(x1,y1,x0,y0);
//    else
//      plotLineLow(x0,y0,x1,y1);
//  }
//  else {
//    if( y0>y1 )
//      plotLineHigh(x1,y1,x0,y0);
//    else
//      plotLineHigh(x0,y0,x1,y1);
//  }
//}
//

void lineTo(int x0,int y0, int x1, int y1) {
  const int dx = x1-x0;
  const int dy = y1-y0;
  const int absdx = abs(dx);
  const int absdy = abs(dy);
  output[x0][y0]='.';
  output[x1][y1]='.';

  int x = x0;
  int y = y0;

  //slope < 1 
  if( absdx > absdy ) {
    int d = 2 * absdy - absdx;

    for(int i = 0; i < absdx ; i++ ) {
      x = dx < 0 ? x-1:x+1;
      if(d<0)
        d+=2*absdy;
      else {
        y = dy < 0 ? y-1: y+1;
        d+=2*(absdy-absdx);
      }
      //printf("%d\t%d", x,y);
      output[x][y]='.';
    }
  }
  else {
    // slope > 1 
    int d = 2 * absdx - absdy;

    for(int i = 0; i<absdy; i++) {
      y = dy < 0? y-1:y+1;
      if(d < 0)
        d+=2*absdx;
      else {
        x = dx < 0? x-1:x+1;
        d+=2*(absdx-absdy);
      }
      //printf("%d\t%d", x,y);
      output[x][y]='.';
    }
  }
}

float transform3DTo2D(float xy, float z) {
  float angleInRad = (FOV / 180.0) * 3.141;
  return xy / ( z * tan(angleInRad / 2.0));
}

cube rotate3D(cube inp, float roll, float pitch, float yaw) {
  cube out;

  out.x = cos(yaw) * cos(pitch) * inp.x + 
          (cos(yaw) * sin(pitch) * sin(roll) - 
              sin(yaw) * cos(roll)) * inp.y +
          (cos(yaw) * sin(pitch) * sin(roll) -
              sin(yaw) * cos(roll)) * inp.z;

  out.y = sin(yaw) * cos(pitch) * inp.x +
          (sin(yaw) * sin(pitch) * sin(roll) +
              cos(yaw) * cos(roll)) * inp.y +
          (sin(yaw) * sin(pitch) * cos(roll) -
              cos(yaw) * sin(roll)) * inp.z;

  out.z = -sin(pitch) * inp.x + 
           cos(pitch) * sin(roll) * inp.y +
           cos(pitch) * cos(roll) * inp.z;

  return out;
}

void displayOut() {
  for(int i=0;i<WIDTH;i++) {
    for(int j=0; j<HEIGHT;j++) {
      printf("%c", output[i][j]);
    }
    printf("\n");
  }
}


void draw(float mouseX, float mouseY) {
  const float mouseXRatio = (mouseX / WIDTH) * 3.141;
  const float mouseYRatio = (mouseY / HEIGHT) * 3.141;
  cube prevCube = rotate3D(matrix[0][0], mouseXRatio, mouseYRatio, 0);
  float x0 = transform3DTo2D(prevCube.x, prevCube.z + CUBE_DIST);
  float y0 = transform3DTo2D(prevCube.y, prevCube.z + CUBE_DIST);
  // cube prev;
  // prev.x = 0.0;
  // prev.y = 0.0;
  // prev.z = 0.0;
  // prev = rotate3D(prev, mouseXRatio, mouseYRatio, 0);
  // float x0 = transform3DTo2D(prev.x, prev.z + CUBE_DIST);
  // float y0 = transform3DTo2D(prev.y, prev.z + CUBE_DIST);
  cube temp;
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 5; j++) {
      temp = rotate3D(matrix[i][j], mouseXRatio, mouseYRatio, 0 );
      float x1 = transform3DTo2D(temp.x, temp.z + CUBE_DIST);
      float y1 = transform3DTo2D(temp.y, temp.z + CUBE_DIST);
      float x1Cp = x1;
      float y1Cp = y1;
      x0 = x0 * WIDTH + WIDTH*0.5;
      x1 = x1 * WIDTH + WIDTH*0.5;
      y0 = y0 * HEIGHT + HEIGHT*0.5;
      y1 = y1 * HEIGHT + HEIGHT*0.5;
      //printf("%d\t%d\t%d\t%d\n", (int)x0, (int)y0, (int)x1, (int)y1);
      lineTo((int)x0, (int)y0, (int)x1, (int)y1);
      x0=x1Cp;
      y0=y1Cp;
    }
    
  }
  displayOut();
}



int main(void) {
int mouseX = 0;
int mouseY = 0;
printf("\x1b[2J");
 for( ; ; ) {
   draw(mouseX,mouseY);
   mouseX++;
   mouseY++;
  usleep(100000);
   
   for(int i=0;i<WIDTH;i++){
     for(int j=0;j<HEIGHT;j++){
       output[i][j]=' ';
     }
   }
 }
     return 0;
}
