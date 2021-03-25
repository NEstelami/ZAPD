# XML files

This document aims to be a small reference of how to create a compatible xml file for ZAPD.

## Basic XML

An example of an object xml:

```xml
<Root>
    <File Name="object_blkobj" Segment="6">
        <!-- Illusion Room Collision -->
        <Collision Name="gIllusionRoomCol" Offset="0x7564"/>

        <!-- Illusion Room DLists -->
        <DList Name="gIllusionRoomNormalDL" Offset="0x14E0"/>
        <DList Name="gIllusionRoomIllusionDL" Offset="0x53D0"/>
        
        <!-- Unused Content -->

        <!-- Illusion room DLists -->
        <DList Name="gIllusionRoomTreeDL" Offset="0x7EB0"/>
    </File>
</Root>
```

Every xml must have a `<Root>` tag. It must have at least one `<File>` child.

## Resources types

The following will be a list of the resources/tags supported by ZAPD, and the attributes needed by each one. It's worth noting that every tag expects a `Name="gNameOfTheVariable"`.

For most tags inside a `<File>` tag you should also set an `Offset` attribute. This is the offset (within the file) of the resource you are exporting.

### `File`

Example of this tag:

```xml
<File Name="object_gi_fire" Segment="6">
```

Attributes:

- `Segment`: Required. This is the segment number of the current file. Expects a decimal number, usually 6 if it is an object, or 128 for overlays.
- `BaseAddress`: Optional. RAM address of the file. Expects a hex number (with `0x` prefix). Default value: `0`.
- `RangeStart`: Optional. File offset where the extraction will begin. Hex. Default value: `0x000000000`.
- `RangeEnd`: Optional. File offset where the extraction will end. Hex. Default value: `0xFFFFFFFF`.
- `Game`: Optional. Valid values: `OOT`, `MM`, `SW97` and `OOTSW97`. Default value: `OOT`.

### `Texture`

Example:

```xml
<Texture Name="gCraterSmokeConeTex" OutName="crater_smoke_cone" Format="ia8" Width="32" Height="32" Offset="0xC30"/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Tex`.
- `OutName`: Required. The filename of the extracted `.png` file.
- `Format`: Required. The format of the image. For more info on the existing formats, see [here](TODO: add link).
- `Width`: Required. Width in pixels of the image.
- `Height`: Required. Height in pixels of the image.


### `Blob`

TODO.


### `DList`

Example:

```xml
<DList TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `DL`.


### `Scene` and `Room`

TODO. I'm hopping somebody else will do this.


### `Animation`

Example:

```xml
<Animation TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Anim`.


### `PlayerAnimation`

Example:

```xml
<PlayerAnimation TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `TODO`.


### `CurveAnimation`

Example:

```xml
<CurveAnimation TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `TODO`.
- `SkelOffset`: Required. Offset of the `CurveSkeleton` related to this Animation.


### `Skeleton`

Example:

```xml
<Skeleton TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Skel`.
- `Type`: Required. Valid values: `Normal`, `Flex` and `Curve`.
- `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin` and `Curve`.


### `Limb`

Example:

```xml
<Limb TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Limb`.
- `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin` and `Curve`.


### `Symbol`

TODO.


### `Collision`

Example:

```xml
<Collision TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Col`.


### `Scalar`

Example:

```xml
<Scalar TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `TODO`.
- `Type`: Required. Valid values: `s8`, `u8`, `s16`, `u16`, `s32`, `u32`, `s64`, `u64`, `f32`, `f64`


### `Vector`

Example:

```xml
<Vector TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `TODO`.
- `Type`: Required. Valid values: `s16`, `s32`, `f32`.
- `Dimensions`: Required. The amount of dimensions of the vector. Valid values: `3`.

Can be wrapped in an `<Array/>` tag.

### `Vtx`

Example:

```xml
<Vtx TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Vtx`.

Can be wrapped in an `<Array/>` tag.


### `Cutscene`

Example:

```xml
<Cutscene TODO/>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g` and suxfixed by `Cs`.


### `Array`

TODO.
