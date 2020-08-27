# Another 3DEngine

A 3D Game Engine inspired by javidx9 tutorial on YouTube

***

## Using included SDL2 files

* Compile for debugging
```
g++ -g -Wall -Wextra -L./lib -I./include src/3DEngine.cpp -o bin/3DEngine -lSDL2
```

* Compile for release
```
g++ -O2 -s -DNDEBUG -Wall -Wextra -L./lib -I./include src/3DEngine.cpp -o bin/3DEngine -lSDL2
```

* Run
```
LD_LIBRARY_PATH=lib bin/3DEngine
```
***

## Using local SDL2 files

* Install SDL2
```
sudo apt install libsdl2-dev
```

* Compile for debugging
```
g++ -g -Wall -Wextra src/3DEngine.cpp -o bin/3DEngine -lSDL2
```

* Compile for release
```
g++ -O2 -s -DNDEBUG -Wall -Wextra src/3DEngine.cpp -o bin/3DEngine -lSDL2
```

* Run
```
bin/3DEngine
```
