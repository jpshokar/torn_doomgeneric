torn - framework														2022-08-23
-----------------------------------------------------------------------------------

	Thanks for using torn, an all-in-one framework for developing quick yet efficent applications.


Fundemental Archtecture:
-----------------------------

	torn is a modularized framework. This means that unnecessary modules in your program can be ommited. This ability of ommision can signifcantly reduce the overall application size and compilation time as you are now only including the necessary modules for your program. 

NOTE: Some modules **do** depend on other modules. You need to include all the modules
that may require that module for it to work! 


Rendering Backends 
--------------------------------

	Torn will initially support OpenGL and will support D3D11 in the future, and to support this new commodity, torn will be structured in an immediate mode fashion. 

	Torn utilizes shared libraries (which can be disabled) to modularize these rendering backends on different platforms, as well as using the compile-time pre-processors to automatically select the required back-end depending on your system if you are opting out of the shared-library approach. Alternatively, you can disable this and use your own rendering backend. 


