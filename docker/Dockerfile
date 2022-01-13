FROM stablebaselines/rl-baselines-zoo

RUN apt-get update

RUN apt-get install --yes --no-install-recommends wget unzip g++ cmake libglu-dev libxi-dev libsfml-dev p7zip-full

RUN pip install scikit-build

RUN wget https://github.com/pybind/pybind11/archive/refs/tags/v2.9.0.zip && unzip v2.9.0.zip && cd pybind11-2.9.0 && rm -rf build && mkdir build && cd build && cmake .. && make install
# RUN pip install pybind11[global]

# RUN wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/box2d/Box2D_v2.2.1.zip && unzip Box2D_v2.2.1.zip && cd Box2D_v2.2.1/Build && cmake -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=ON -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_BUILD_DOCS=OFF .. && make && make install
RUN wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/box2d/Box2D_v2.3.0.7z && 7z x Box2D_v2.3.0.7z && cd Box2D_v2.3.0/Box2D/Build && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBOX2D_INSTALL=ON -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=ON -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_BUILD_DOCS=OFF .. && make && make install
