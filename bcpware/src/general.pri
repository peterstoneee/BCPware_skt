# this is the common include for anything compiled inside the meshlab pro


# This is the main coord type inside meshlab
# it can be double or float according to user needs.
#找到MeshLab_Scalar 定義在哪了!
DEFINES += MESHLAB_SCALAR=float
#DEFINES += MESHLAB_SCALAR=double

VCGDIR = ../../../vcglib
GLEWDIR = ../external/glew-1.7.0


# uncomment to try Eigen
# DEFINES += VCG_USE_EIGEN
# CONFIG += warn_off



# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX


