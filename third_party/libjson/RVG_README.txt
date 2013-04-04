Notes:
-Dependency path on line 270 was incrorrect, so "depdir" was defined to supply the correct path.
-Library seems to have to be installed to work correctly:
   make BUILD_TYPE=small SHARED=1
   sudo make install

