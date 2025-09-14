Object API
==========

Getting Camera Information
--------------------------

* :code:`lych cam get_loc <cam_id>` Get 3D location of a camera.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<cam_id>`: ID of the camera.
     * - Returns
       - :code:`<x> <y> <z>`: 3D coordinates of the camera in the scene.

* :code:`lych cam get_rot <cam_id>` Get rotation of a camera.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<cam_id>`: ID of the camera.
     * - Returns
       - :code:`<p> <y> <r>`: Get pitch, yaw, and roll angles of the camera.

* :code:`lych cam get_fov <cam_id>` Get field of view of a camera.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<cam_id>`: ID of the camera.
     * - Returns
       - :code:`<fov>`: Field of view angle in degrees.

* :code:`lych cam get_c2w <cam_id>` Get the camera-to-world transformation matrix.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<cam_id>`: ID of the camera.
     * - Returns
       - :code:`<16 floats>`: 4x4 transformation matrix in row-major order.
