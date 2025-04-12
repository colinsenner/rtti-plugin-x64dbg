# Installation 

1. Compile the solution in Release mode for x86 and x64.
2. Copy `Rtti.dp32` from the /bin to your `x64dbg\release\x32\plugins` directory.
3. Copy `Rtti.dp64` from the /bin to your `x64dbg\release\x64\plugins` directory.

# Usage

1. Open the example binary `ClassRTTIPluginTest64d.exe` in x64dbg.
2. Run the program until the breakpoint is hit. The instruction will be `lea rcx, ...; call classrttiplugin...`, the call is the constructor for the class.
3. Step over the call and follow in dump `rax` to get to the class instance. 
4. Right-click the address in the the memory dump and select `Rtti-plugin-x64 -> Dump Rtti`.  If the address contains RTTI information, a dialog will appear at the bottom of the screen and also in the `Log` window.