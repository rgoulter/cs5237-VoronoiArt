# Delaunay Algorithm Notes

## Definitions / Terminology

- Locally Delaunay
  An edge `ab` is locally delaunay if:
  - ab is in ????
  - For two neighbouring triangles (of `ab`) `abc` and `abd`,
    `c` is out of the circumcircle of `abd` (and vice versa).

- link ???? (of vertex in graph)


## Random Incremental Algorithm

- Construct a super triangle `abc` with artificial points `a`, `b`, `c`,
  such that contains all points in `S`.

- Randomise the input points in `S`.

- For each point `p` in `S`:
  - Locate a triangle in the existing triangulation.
  - Insert the point `p` and split this triangle into three.
  - Flip all edges in the _link_ of `p` until all of them are Locally Delaunay.
    - _link_ of `p`: For all the triangles touching `p`, take their edges which
      do _not_ touch `p`.


Legalise Edge???
