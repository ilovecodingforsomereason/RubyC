# RubyC

a pure c port of minecraft rd-132211 from may 2009. 

got tired of dealing with old java launcher bugs just to play a build with two blocks in it, so i rewrote the whole thing in c. it runs natively, replicates the old physics, and doesn't need a runtime to boot up.

---

## how to run it

you just need gcc and glfw.

```bash
# ubuntu / debian
sudo apt install build-essential libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev

# build it
make clean && make

# run it (keep terrain.png in the same folder)
./rubyc
```

---

## controls

* `w` `a` `s` `d` - move around
* mouse - look around
* `space` - jump
* left click - place block
* right click - break block
* `enter` - save world to level.dat
* `r` - respawn if you fall into the void
* `esc` - quit

---

## files

* `ruby_dung.c` - window, input, main game loop
* `timer.c` - keeping the game ticks locked at 60hz
* `player.c` - camera look and walking speed
* `textures.c` - loading the sprite sheet via stb_image
* `phys/aabb.c` - handling block collisions
* `level/` - folder for chunk rendering and drawing arrays
