Darwin is an experimental high-performance 3D voxel engine written in C++.
----

![Darwin Example Rendering 1](https://content.wuala.com/contents/Danvil/Public/darwin/2011-08-12%20Cornell%20Radiosity.png)
![Darwin Example Rendering 2](https://content.wuala.com/contents/Danvil/Public/darwin/2011-07-10%2056M%20Cubes.jpg)
![Darwin Example Rendering 3](https://content.wuala.com/contents/Danvil/Public/darwin/2011-10-13%20Room%204.png)

Please give me credit if you use my code or art files or parts of it.
I would be happy to hear if my work is useful to you.


Build instructions:
----
mkdir build; cd build; cmake ..


Run instructions:
----
1. Download the asset files from here
https://content.wuala.com/contents/Danvil/Public/Darwin_assets.tar.gz?key=wZSXjVZ8yDkb

2. Run Cornell with the path to the assets as command line option
./Cornell /home/david/Darwin_assets

3. Admire cubes with radiosity :)


Dependencies:
----

* glut freeglut3-dev
* GLEW libglew1.6-dev
* boost libboost-all-dev
* Eigen3 libeigen3-dev
* FTGL libftgl-dev
* DEVIL libdevil-dev
* freetype2 libfreetype6-dev
* libnoise libnoise-dev
