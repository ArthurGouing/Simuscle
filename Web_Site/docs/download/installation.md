---
sidebar_position: 2
---

# Installation

You must have installed OpenGL, a C++ compiler (I used GNU compiler 12.3). (Dear ImGUI, stbimage, glad should already be include in the folder)

Once you have download all the required library, compile Simuscle with Cmake. You can simply paste the follwing command:
```bash
cd Simuscle/
mkdir build/
cd build/
cmake ../
make install
cd ../
```

Now you can run Simuscle by doing the folowing command:  (faire une install meilleur pour avoir Simuscle dans les app)
```bash
./Simuscle
```

Great job ! If you manage to install Simuscle, you probably done the hardest part !
