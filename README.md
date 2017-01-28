# Grapher2D
A 2d graphing calculator.

##Features
1. Graphs functions using opengl.
2. Has a 't' variable that mesures the amount of seconds since the function was submited to the grapher and a 'at' variable that mesures the amount of seconds since launch. They can be used to create moving functions.
3. Very very fast (the implicit functions use OpenCL to run at incredible speed!)

##Usage
Enter the equations into the textboxes. You can add and take away equation slots using the add graph and "X" buttons.

##Disclaimer
Must have an OpenGL 3.3+and OpenCL 1.0+ capable graphics card and driver

##Compiling
This is a more experimental branch than the main one, the VS folder contains a Visual Studio project with the code inside. It will most like NOT compile on any machine but mine if it is run without any configuration. Maybe later I will add more protable code file, but for now, if you want to compile this project you will have to do it manually. You must have OpenCL avalible to do this!. (This project requires freetype and GLFW to be linked to it to run. Their headers are included in the VS file in the ext folder along with .lib files for linking in windows. If you wish to compile this on any other platform you will have to build those libraries for that platform. 
