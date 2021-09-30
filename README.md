# COMP8551-ProjectOsiris
A custom Hybrid ECS engine to facilitate a simple Dungeon Crawler, written in C++

<h3>Importing Libraries</h3>
<p>
  For this project, we'll be keeping our external libraries within the <i>"_dependencies"</i> directory. A relative path has been configured in Osiris.vcxproj, or <i>Osiris Properties</i>, which is an XML config file for Visual Studio (VS) environments. This config can be accessed and edited by right clicking the "Osiris" dropdown in the VS Solution Explorer and selecting "Properties".
</p>

The <i>"_dependencies"</i> directory contains three subfolders:

<p>
  <b>includes:</b> This is where we put external headers.
  <br><b>sources:</b> This is where we put .c and .cpp files that must be used within the project scope.
  <br><b>libs:</b> This is where we put pre-compiled libraries. When importing, we'll need to include them in the Linker under <i>Osiris Properties > Linker > Input > Additional Dependencies</i>.
  
</p>

