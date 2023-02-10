# MICROUI

UI for OpenFrameworks.
Made to be a compact user interface for software.

![IMAGE ALT TEXT HERE](microui.png)

Midi controller is now in a separate repository https://github.com/dimitre/ofxMicroUIMidiController

## Text Files for interface.
UI Elements (sliders, toggles, etc) are loaded from a text file.
So it is easy to copy paste, cut, paste somewhere else, change column, copy interfaces between softwares
and change layout things without recompile anything. Fast prototyping.
If you have a finished software and don't want external files you can stringify everything to be hard coded.
Now working on ofxMicroUIRemote which mirrors and syncronize the interface in another device using osc.
This is the interface I've been using for all my openFrameworks projects.

## Layout

The layout of a UI is defined before the elements, separated by a line of
`=========`.

Layout elements include the following. Note that fields in each
line of text (the definition of an element) must be tab-delimited.

| Layout element type | Fields |
|---------------------|--------|


## Elements

Elements in the UI text file include the following. Note that fields in each
line of text (the definition of an element) must be tab-delimited.

| Element type | Fields |
|--------------|--------|


## Accessors

Once loaded from file or string, the elements of a microUI
can be accessed via `getElement(string name)`. For example:
```c
ofxMicroUI mUI;
...
mUI.load("mui.txt");
...
mUI.getElement("myInspector").set("my new value");
```

Dimitre
[Dmtr.org](http://dmtr.org)
