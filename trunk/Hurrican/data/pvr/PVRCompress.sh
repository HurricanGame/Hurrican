#!/bin/bash

powerof2 () {
  P2W=`convert ../$f -format "%[fx:2^(ceil(log(w)/log(2)))]" info:`
  P2H=`convert ../$f -format "%[fx:2^(ceil(log(h)/log(2)))]" info:`
  if test $P2W -lt 32; then
    P2W=32
  fi
  if test $P2H -lt 32; then
    P2H=32
  fi
  echo ${P2W}x${P2H}
} 

rm -rf tc2
rm -rf tc4
rm -rf XRGB1555
rm -rf ../png32

mkdir tc2
mkdir tc4
mkdir XRGB1555
mkdir ../png32

cd ..
for f in ../*.bmp
do
  cd pvr
  echo "Processing $f file..."

  # Grow the current size to a power of 2 (and at a min of 32 pixels)
  powerof2

  # Convert the original image to 32 bit png (change magneta into transparent)
  convert ../$f -background magenta -extent ${P2W}x${P2H} -transparent magenta PNG32:../png32/$f.png
  
  # Convert the 32 bit png into the pvrtc formats
  ./PVRTexTool -fPVRTC2 -i../png32/$f.png -o tc2/$f.pvr
  ./PVRTexTool -fPVRTC4 -i../png32/$f.png -o tc4/$f.pvr
  ./PVRTexTool -f1555 -i../png32/$f.png -o XRGB1555/$f.pvr
  mv XRGB1555/$f.pvr XRGB1555/$f.XRGB1555

  cd ..
done

for f in *.png
do
  cd pvr
  echo "Processing $f file..."
 
  powerof2

  convert ../$f -background transparent -transparent magenta -extent ${P2W}x${P2H} PNG32:../png32/$f.png

  ./PVRTexTool -fPVRTC2 -i../png32/$f.png -o tc2/$f.pvr
  ./PVRTexTool -fPVRTC4 -i../png32/$f.png -o tc4/$f.pvr
  ./PVRTexTool -f1555 -i../png32/$f.png -o XRGB1555/$f.pvr
  mv XRGB1555/$f.pvr XRGB1555/$f.XRGB1555

  cd ..
done
