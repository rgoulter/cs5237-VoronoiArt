# CS5237 Project.

## Visual Studio version

Version of Visual Studio which the Solution files use from the zips seems to be 2008.

## Phase II

See the PPT for info. Building up triangulations using the Random-Incremental Algorithm
discussed in class.

Phase II requires OpenGL, and thus requires GLUT.
If I recall correctly, the following links should be useful for installing GLUT on Windows.
http://user.xmission.com/~nate/glut.html for the files, and follow the instructions in the readme for it.
(Email for help if you're unsure about this).

## General Project Structure

Phase II is being handled entirely within the Phase II directory.
(This is due to difficulty handling Visual Studio, blargh).
As such, its subfolder 'basics' refers to Phase I structures,
and its subfolder 'basicsP2' refers to Phase II structures.

The Phase II folder itself has a 'main.cpp' file, which has an
OpenGL/GLUT application, and the logic for the Phase II program.
(The folder CS5237 in the base of the repo hosts a similar file
for Phase I).

Basics (P1) involves the data structures LongInt and PointSet.
Basics (P2) involves the data structures PointSetArray, and Trist.
PointSetArray extends PointSet, to allow for things like getting the point,
and clearing all points. Trist is a TRIangle STructure, and manages
the triangle point table as described in the notes on Ordered Triangle.

THINGS TO BE AWARE OF:
Indexing points is kindof a bitch / kindof inconsistent at the moment.
e.g. PSA.getPoint(idx, x, y) has a 1-based index as argument (since the
triangles are constructed with 1-based index points), but PS.inTri has
0-based indices. (Since that's what the internal vector uses?).
Indexing Triangles has the issue that Trist's methods expect an OrTri
(Ordered Triangle), rather than just a tIdx (triangle index). This is
resolved by shifting tIdx as described in Ordered Triangle notes.
