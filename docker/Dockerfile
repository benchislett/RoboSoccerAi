FROM ubuntu:jammy

RUN apt-get update

RUN apt-get install --yes --no-install-recommends build-essential swig xvfb libxrandr2 git ffmpeg python3.10 python3.10-dev python3-pip wget unzip g++ cmake libglu-dev libxi-dev libxmu-dev libsfml-dev libbluetooth-dev p7zip-full ssh libsdl2-dev freeglut3 freeglut3-dev libjpeg-dev libv4l-dev

RUN cd /usr/include/linux && ln -s ../libv4l1-videodev.h videodev.h

RUN pip3 install imitation scikit-build pytest gym numpy torch
RUN pip3 install git+https://github.com/DLR-RM/stable-baselines3

RUN wget https://github.com/pybind/pybind11/archive/refs/tags/v2.9.0.zip && unzip v2.9.0.zip && cd pybind11-2.9.0 && rm -rf build && mkdir build && cd build && cmake .. && make install
# RUN pip install pybind11[global]

# RUN wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/box2d/Box2D_v2.2.1.zip && unzip Box2D_v2.2.1.zip && cd Box2D_v2.2.1/Build && cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=ON -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_BUILD_DOCS=OFF .. && make && make install
# RUN wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/box2d/Box2D_v2.3.0.7z && 7z x Box2D_v2.3.0.7z && cd Box2D_v2.3.0/Box2D/Build && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=ON -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_BUILD_DOCS=OFF .. && make && make install
RUN apt-get install --yes libbox2d-dev
