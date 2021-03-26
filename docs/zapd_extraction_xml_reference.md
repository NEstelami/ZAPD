# ZAPD extraction xml reference

This document aims to be a small reference of how to create a compatible xml file for ZAPD.

## Basic XML

An example of an object xml:

```xml
<Root>
    <File Name="object_jj" Segment="6">

        <Animation Name="gJabuJabuAnim" Offset="0x1F4C"/>

        <Skeleton Name="gJabuJabuSkel" Type="Flex" LimbType="Standard" Offset="0xB9A8"/>
            
        <!-- Jabu Jabu eye textures -->
        <Texture Name="gJabuJabuEyeOpenTex" OutName="jabu_jabu_eye_open" Format="rgb5a1" Width="16" Height="32" Offset="0x7698"/>
        <Texture Name="gJabuJabuEyeHalfTex" OutName="jabu_jabu_eye_half" Format="rgb5a1" Width="16" Height="32" Offset="0x7A98"/>
        <Texture Name="gJabuJabuEyeClosedTex" OutName="jabu_jabu_eye_closed" Format="rgb5a1" Width="16" Height="32" Offset="0x7E98"/>


        <Collision Name="gJabuJabu1Col" Offset="0x0A1C"/>
        <Collision Name="gJabuJabu2Col" Offset="0x1830"/>
        <Collision Name="gJabuJabu3Col" Offset="0xBA8C"/>

    </File>
</Root>
```

Every xml must have a `<Root>` tag. It must have at least one `<File>` child.

## Resources types

The following is a list of the resources/tags supported by ZAPD, and the attributes needed by each one.

For most tags inside a `<File>` tag you should also set an `Offset` attribute. This is the offset (within the file) of the resource you are exporting.

It's worth noting that every tag expects a `Name="gNameOfTheVariable"`. This is will be the name of the extracted variable. Every variable must be prefixed with `g` and the suffix should represent the type of the variable.

## File

Example of this tag:

```xml
<File Name="object_gi_fire" Segment="6">
```

Attributes:

- `Name`: Required. The name of the file in `baserom/` that will be extracted.
- `Segment`: Required. This is the segment number of the current file. Expects a decimal number, usually 6 if it is an object, or 128 for overlays.
- `BaseAddress`: Optional. RAM address of the file. Expects a hex number (with `0x` prefix). Default value: `0`.
- `RangeStart`: Optional. File offset where the extraction will begin. Hex. Default value: `0x000000000`.
- `RangeEnd`: Optional. File offset where the extraction will end. Hex. Default value: `0xFFFFFFFF`.
- `Game`: Optional. Valid values: `OOT`, `MM`, `SW97` and `OOTSW97`. Default value: `OOT`.

## Texture

Textures are extracted as `.png` files.

Example:

```xml
<Texture Name="gCraterSmokeConeTex" OutName="crater_smoke_cone" Format="ia8" Width="32" Height="32" Offset="0xC30"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Tex`.
- `OutName`: Required. The filename of the extracted `.png` file.
- `Format`: Required. The format of the image. Valid values: `rgba32`, `rgb5a1`, `i4`, `i8`, `ia4`, `ia8`, `ia16`, `ci4` and `ci8`.
- `Width`: Required. Width in pixels of the image.
- `Height`: Required. Height in pixels of the image.

## Blob

Blob are extracted as `.bin` files.

Example:

```xml
<Blob Name="gFireTempleBlob_00CCC0" Size="0x10E" Offset="0xCCC0"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Blob`.
- `Size`: Required. Amount of bytes to extract. Hex.

## DList

Example:

```xml
<DList Name="gGiGreenRupeeInnerColorDL" Offset="0x04A0"/>
```

Attributes:

- `Name`: Required. Suxffixed by `DL`.

## Scene and Room

TODO. I'm hopping somebody else will do this.

## Animation

Example:

```xml
<Animation Name="gWallmasterDamageAnim" Offset="0x590"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Anim`.

## PlayerAnimation

Example:

```xml
<PlayerAnimation Name="gPlayer3Anim" Offset="0x2310"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Anim`.

## CurveAnimation

Example:

```xml
<CurveAnimation Name="gEnBoxCurveAnim_4B60" SkelOffset="0x5EB8" Offset="0x4B60"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Anim`.
- `SkelOffset`: Required. Offset of the `CurveSkeleton` related to this Animation.

## Skeleton

Example:

```xml
<Skeleton Name="gEnBoxCurveSkel" Type="Curve" LimbType="Curve" Offset="0x5EB8"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Skel`.
- `Type`: Required. Valid values: `Normal`, `Flex` and `Curve`.
- `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin` and `Curve`.

## Limb

Example:

```xml
<Limb Name="gChuGirlHeadLimb" LimbType="Standard" Offset="0x6E34"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Limb`.
- `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin` and `Curve`.

## Symbol

A special element that allows declaring a variable without actually extracting it from the current file. Useful when a resource references an element from another file. The symbol will be declared as `extern`.

Example:

```xml
<Symbol Name="gJsjutanShadowTex" Type="u8" Size="1" Count="0x800" Offset="0x4E70"/>
```

Attributes:

- `Type`: The type of the declared variable. If missing, then it will default to `void*`.
- `TypeSize`: The size in bytes of the type. If missing, then it will default to `4` (the size of a word and a pointer). Integer or hex value.
- `Count`: Optional. If it is present, the variable will be declared as an array instead of a plain variable. The value of this attribute specifies the length of the array. If `Count` is present but it has no value (`Count=""`), then the length of the array will not be specified either in the declared variable. Integer or hex value.

## Collision

Example:

```xml
<Collision Name="gDesertColossusBombableWallCol" Offset="0x1A58"/>
```

Attributes:

- `Name`: Required. Suxffixed by `Col`.

## Scalar

✅ Can be wrapped in an [`Array`](#Array) tag.

Example:

```xml
<Scalar Type="u64" Name="pad34F8"/>
```

Attributes:

- `Name`: Required. Suxffixed by ~~`TODO`~~.
- `Type`: Required. Valid values: `s8`, `u8`, `s16`, `u16`, `s32`, `u32`, `s64`, `u64`, `f32` and `f64`.

## Vector

✅ Can be wrapped in an [`Array`](#Array) tag.

Example:

```xml
<Array Name="D_04002040" Count="24" Offset="0x2040">
    <Vector Type="s16" Dimensions="3" />
</Array>
```

Attributes:

- `Name`: Required. Suxffixed by ~~`TODO`~~.
- `Type`: Required. Specifies the vector's type (`Vec3s`, `Vec3i` and `Vec3f`). Valid values: `s16`, `s32` and `f32`.
- `Dimensions`: Required. The amount of dimensions of the vector. Valid values: `3`.

## Vtx

✅ Can be wrapped in an [`Array`](#Array) tag.

Example:

```xml
<Array Name="gTriforceVtx" Count="32" Offset="0x0000">
    <Vtx/>
</Array>
```

Attributes:

- `Name`: Required. Suxffixed by `Vtx`.

## Cutscene

Example:

```xml
<Cutscene TODO/>
```

Attributes:

- `Name`: Required. Suxffixed by `Cs`.

## Array

The `Array` element is special, because it needs an inner element to work. It will declare an array of it's inner element.

Currently, only [`Scalar`](#Scalar), [`Vector`](#Vector) and [`Vtx`](#Vtx) support being wrapped in an array.

Example:

```xml
<Array Name="gTrialBarrierEnergyVtx" Count="102" Offset="0x4FD0">
    <Vtx/>
</Array>
```

Attributes:

- `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g`. The sufix is mandated by the element contained.
- `Count`: Required. Amount of elements. Integer.
