REM graphics
lcc -Wf-bo1 -c dev/data/tiles/sprite_tileset.c -o sprite_tileset.o
lcc -Wf-bo1 -c dev/data/tiles/sprite_bkg_tileset.c -o sprite_bkg_tileset.o
lcc -Wf-bo1 -c dev/data/tiles/bkg_tileset.c -o bkg_tileset.o
REM maps
lcc -Wf-bo2 -c dev/data/maps/game_map.c -o game_map.o
lcc -Wf-bo2 -c dev/data/maps/gui_map.c -o gui_map.o
REM logic
lcc -c dev/main.c -o main.o
REM build
lcc -Wl-yt1 -Wl-yo4 -Wl-ya0 main.o sprite_tileset.o sprite_bkg_tileset.o bkg_tileset.o game_map.o gui_map.o -o rom/Sheep_Kaboom.gb
REM clean
del *.o *.lst