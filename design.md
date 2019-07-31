## Window manager

### Window element:
- Shader:
  * One fragment shader / element type

- Color:
  * Active color accent
  * Active color dark
  * Inactive color accent
  * Inactive color dark
  * Active background color
  * Inactive background color

- Can be of elementary type:
  * [Rounded] Rectangle
  * Ellipse
  * Text

#### Alignment:

* center anchor
  - vertical
  - horizontal

* horizontal
  - any two of: left/right/width

* vertical
  - any two of: top/bottom/height

### Height snapping

### Linking properties:

Start generating from the components that produce width and propagate values through the network  

### Layout manager

Planes need to be updated when the linked attribute changes
Attributes need to be encoded and decoded to be transmitted in a generalizable way => bindings > bind setWidth to on EndChanged

**Flow:** window changes > update layout manager with windowsize
