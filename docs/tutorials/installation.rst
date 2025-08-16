Installation
============

Install UE5
-----------

TODO.

Install and Compile LychSim Plugin
----------------------------------

1. **Create a new Unreal Engine project.**

   - Create a new project using the "Games - First Person" template.

   .. figure:: figures/installation_01.png
      :align: center

2. **Add LychSim plugin source code to the Unreal project.**

   - First create the `Plugins` directory in your Unreal project folder.
   - For windows users, link the code with the following command (in `cmd` with administrator privileges):

   .. code-block:: batch

      mklink /J ^
      "C:\Users\username\Documents\Unreal Projects\LychSimTest\Plugins\LychSim" ^
      "C:\Users\username\Documents\LychSim\ue_plugin\LychSim"

3. **Re-generate project files.** This helps Unreal Engine and Visual Studio recognize the new plugin.

   - *Option 1:* Right-click on the `.uproject` file of your Unreal project and select "Generate Visual Studio project files".
   - *Option 2:* Open a command prompt and navigate to the Unreal Engine installation directory, then run the following command:

   .. code-block:: batch

      "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" ^
      -projectfiles -project="C:\Users\username\Documents\Unreal Projects\LychSimTest\LychSimTest.uproject" ^
      -game -engine

4. **Compile the plugin in Visual Studio.**

   - Open the generated `.sln` file in Visual Studio.
   - Click on "Build" in the top menu, then select "Build Solution".

   .. figure:: figures/installation_04.png
      :align: center

5. **Enable the LychSim plugin in Unreal Engine.**

   - Open your Unreal Engine project.
   - Go to "Edit" > "Plugins".
   - Find the LychSim plugin in the list and enable it.
   - Restart Unreal Engine if prompted.

   .. figure:: figures/installation_03.png
      :align: center

6. **Verify the installation.**

   - Start the Unreal Engine project by clicking the "Play" button.
   - From the Unreal Engine built-in Cmd, run the following command: :code:`vget /object`.

   .. figure:: figures/installation_02.png
      :align: center
