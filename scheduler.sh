g++ -ggdb -o q retinex2opencv.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`-I /usr/include/ -ljpeg -lpng -fpermissive ;
./q images/1.jpg
g++ -ggdb -o q facedetect.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`-I /usr/include/ -ljpeg -lpng -fpermissive ;
./q images/tmpmsr.jpg ;
g++ -ggdb -o q watershed.cpp `pkg-config --cflags opencv` `pkg-config --libs opencv`-I /usr/include/ -ljpeg -lpng -fpermissive ;
./q images/tmpcrop.jpg images/tmpmask.jpg;

