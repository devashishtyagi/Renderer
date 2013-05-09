Renderer
=====================
A renderer for visualizing voxel data files.

Requirements
--------------------
* glfw
* glm
* glu
* OpenGL >= 4.2

Running
--------------------
* make - To compile the binaries needed
* make run - To run the compiled binaries and generate a visualization of the voxel data file. The path to default date files is specified in the Makefile.
* make run FILES=<your sample file> - To specify your datafile at runtime

Options
--------------------
* Up and Down - To zoom in and zoom out of the model
* TAB - To swtich between various wireframe models.
* HOME - To remove the side boundaries and enable viewing of internal details of the model
* Left and Right - Move between various data files (if specified at runtime)
* Space - Switch between various color modes