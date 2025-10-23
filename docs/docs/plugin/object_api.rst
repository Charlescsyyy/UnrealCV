Object API
==========

Basics
------

:mono:`lych obj list`
"""""""""""""""""""""

List all objects in the scene.

**Returns:**

.. list-table::
   :header-rows: 0
   :widths: 25 75

   * - :mono:`obj_id_list` : :mono:`str`
     - IDs of all objects in the scene.

Getting Object Information
--------------------------

:mono:`lych obj get_loc`
""""""""""""""""""""""""

Get 3D locations of objects.

**Examples:**

.. code-block::

   lych obj get_loc obj_01 obj_02
   lych obj get_loc -all

**Parameters:**

.. list-table::
   :header-rows: 0
   :widths: 25 75

   * - :mono:`obj_id_list` : :mono:`str`
     - IDs of all objects in the scene.
   * - :mono:`-all` : :mono:`bool`
     - If set, get locations of all objects in the scene.

**Returns:**

.. list-table::
   :header-rows: 0
   :widths: 25 75

   * - :mono:`output_json` : :mono:`str`
     - A JSON string containing object IDs and their corresponding 3D locations. Example: :code:`{"status": "ok", "outputs": [{"obj_id": "obj_01", "location": [x1, y1, z1]}, {"obj_id": "obj_02", "location": [x2, y2, z2]}]}`.

Modifying objects
-----------------

* :code:`lych object add <obj_name> <obj_path> <x> <y> <z> <p> <y> <r>` Add an object to the scene.

  .. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<obj_name>`: Name of the new object; :code:`<obj_path>`: path to the object file; :code:`<x> <y> <z>`: 3D coordinates where the object will be placed; :code:`<p> <y> <r>`: pitch, yaw, and roll angles for the object's orientation.

* :code:`lych object set_mtl <obj_name> <material_path> <element_idx>` Set material of an object.

.. list-table::
     :header-rows: 0
     :widths: 25 75

     * - Parameters
       - :code:`<obj_name>`: Name of the object; :code:`<material_path>`: Path to the material file; :code:`<element_idx>`: Index of the material element to modify.

Getting part and object level segmentations
-------------------------------------------

* :code:`lych /segmentation/mode object` Set segmentation mode to object level.

* :code:`lych /segmentation/mode part` Set segmentation mode to part level.

After setting the segmentation mode, you can use the following command to get the segmentation image.

* :code:`lych /camera/1/object_mask png`
