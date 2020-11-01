                                *** SCENE CREATOR ***
                    developed by Iliya Iliev and Simeon Angelov
                    
- Scene Creator is a little debug program (OPENGL Based) that allows to build a game scene and write the texture coordinates and sources into a .txt file.


*REQUIREMENTS*

    Installed GLFW library
    sudo apt update
    sudo apt install libglfw3
    sudo apt install libglfw3-dev
    
    After installing the needed libraries, in order to run the program follow the steps:
    1) cd SceneCreator/project/
    2) mkdir build
    4) cd build
    5) cmake ..
    6) ./SceneCreator

*CONFIGURATION FILE*
    There is a .json configuration file located in /SceneCreator/project/src/config.json. It contains the following information about configuring the app.
    "title" - this field of type string contains the name of the application window.
    "screen_resolution" - this field of type JSON object contains the relevant data about the window size and depth.
    Its fields are:
     - "width" - a field of type unsigned int, responsible for the width of the application window.
     - "height" - a field of type unsigned int, responsible for the height of the application window.
     - "depth" - a field of type unsigned int, responsible for the depth of the application window.
    "xml_save_path" - this field of type string holds the default save path for the resources .xml file. Can be changed during runtime.
    All listed fields must be present in your JSON configuration file and be of the correct type, otherwise the application initializion will fail.
    You can specify a custom JSON configuration path, when you start the program the following way:
    ./SceneCreator path_to_json_configuration_file
    Unless an absolute path is specified, all resources like shader, image, json, xml and other files must contain the "../" prefix.

*USER INTERFACE*
    
    For the current moment, the background is defined once, can be found at:
    SceneCreator/project/main.cpp : textureBackground.LoadFromPath("");
    
    All the windows, sliders, selectors etc, are developed with ImGui.
    
    The TOP of the main window "DEBUG", you can see Button - "Write changes into file" and some information about RENDERING(ms/frame), FPS, Resolution, Memory Usage of current loaded textures.

    The Resolution can be changed in the JSON configuration file via the "screen_resolution" field.
    
    (Rendering, depends of VSYNC there is no wait timers or something else. Delta time depends only on the Refresh Rate Frequency(HZ) of monitor.)

*TEXTURES*

    The format of textures that the tool works with are: JPG, JPEG, PNG, DDS.
    
    To load a texture, you can activate the tick "Texture Option", write the path, and add the texture. Or SIMPLY Drag and Drop the texture from outside the window.
    
    To work with a texture you need to SELECT the current object by activating the radio button or SIMPLY click once on RIGHT MOUSE BUTTON. The mouse should be into the coordinates of the texture.
    
    To UNSELECT the texture you need SIMPLY click again the radio button or click once on RIGHT MOUSE BUTTON.
    
    When a texture is selected, the value of alpha channel is changed to show the user that you have selected the object.
    
    !WARNING! - you can work only with ONE selected item at a time !!! Even though, if you select 2 items the coords will be applied to both objects !!!
    
    After selecting the texture, you can change the values X, Y, ANGLE, SCALE by the sliders, or have precision settings with the + and - buttons.
    
    X, Y values can be changed also by Dragging the texture AFTER select the tick "ENABLE DRAGGING" (Don't forget to select the texture before/after to enable the Drag function)
    
    You can simulate ROTATION by selecting the tick "SIMULATE ROTATION". After selecting the tick, the texture will rotate in based on the SPEED that will appear. You can modify the speed with a simple click on the buttons + -.
    
    To work with the SOURCE of texture, you can select "ENABLE IMAGE CUT". A small preview will appear that shows how the texture will look after the modifications. (You can see also the white rect appear of the texture itself). Pressing the button "APPLY CHANGES", the texture WILL NOT BE CUT in Real Time (work in progress for this feature), but the source coordinates will be ready for save into the TXT file. You can see that the values "Previous values" are changed after pressing the button.
    
    To apply some Shaders on the selected texture, you can click the tick "Shaders". A window will appear with all the shader lists with their default values. To apply a shader you can click the button "RESET VALUES" and "Apply...". After applying the shader, it should appear. You can modify runtime the values of the shader.
    To Disable shader just apply the NONE shader.
    
    In the menu "TEXTURE OPTIONS", you can replace the selected texture with another one, or remove it. (The texture will be removed from VRAM)
    
*ANIMATIONS*

    An Animation is a simple vector of textures. To load an animation, you need to click the tick "ANIMATIONS OPTIONS", write the path or SIMPLY drag the path into the path area label.
    
    In "ANIM SETTINGS" drop menu, you can write the FPS.
    
    !WARNING! If the FPS are not inserted, the anim can not be loaded. You can have only 1 animation active (currently). Shaders are not applied to animations !
    The files in the folder should be in alphabetical order
    
    After pressing the button "Create ID's and Textures please check the console. (Due to 1 thread application, when loading images the draw will freeze. Work in progress)
    
    After having 1 active animation, you can reload another at the same place or delete it. 
    
    In the "ANIM OPTIONS" drop menu, you have PAUSE, PLAY, STOP and current FPS slider that you can slide to have the needed effect.
    
*WRITE CHANGES INTO FILE*

    Currently this function will export the file with coords at the path, specified by the "xml_save_path" field in the JSON configuration file.
    It can be however changed during the application runtime via the "Path Options" dialog window, which is next to the button "Write changes into file ...".
