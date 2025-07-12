Object API
==========

Baiscs
------

* :code:`lych object list` List all objects in the scene.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Returns
       - :code:`<obj_name1> <obj_name2> ...`: Names of all objects in the scene.

Getting Object Information
--------------------------

* :code:`lych object get_loc <obj_name>` Get 3D location of an object.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<obj_name>`: Name of the object.
     * - Returns
       - :code:`<x> <y> <z>`: 3D coordinates of the object in the scene.

* :code:`lych object get_aabb <obj_name>` Get axis-aligned bounding box of an object.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<obj_name>`: Name of the object.
     * - Returns
       - :code:`<x> <y> <z> <ex> <ey> <ez>`: 3D location and extents of the axis-aligned bounding box of the object.

Modifying objects
-----------------

* :code:`lych object add <obj_name> <obj_path> <x> <y> <z> <p> <y> <r>` Add an object to the scene.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<obj_name>`: Name of the new object; :code:`<obj_path>`: path to the object file; :code:`<x> <y> <z>`: 3D coordinates where the object will be placed; :code:`<p> <y> <r>`: pitch, yaw, and roll angles for the object's orientation.
