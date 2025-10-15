Known Issues
============

1. Artifacts and low-quality rendering with Lumen
-------------------------------------------------

Current :code:`LychSim` implementation may not fully support Lumen features and may exhibit artifacts or lower quality rendering compared to native Lumen.

**Fix.** Planned to be fixed in future releases.

.. figure:: figures/known_issues/lumen.png

   Artifacts and low-quality rendering with Lumen.

2. Cannot connect to UE5
------------------------

This issue may arise when the default port, *i.e.*, 9000, is blocked or in use by another process. This is often caused by previously uncleaned instances of the server or other applications occupying the same port.

**Fix.** Run :code:`netstat -ano | findstr :9000` to check for unwanted processes. Then kill these processes using :code:`taskkill /PID <pid> /F`.
