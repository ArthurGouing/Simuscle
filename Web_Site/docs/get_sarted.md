---
sidebar_position: 3
---
# Getting started

Let's discover what **Simuscle** is capable of in less than 5 minutes.

## General workflow

*Simuscle workflow explain*

## Download

Refere to the [Download](./download) page to see how to install Simuscle on your device.

(Currently, Simuscle is only supported on Linux)

## Installation

This step can be the harder step, follow the installation instruction that match your device caracteristique:
- [Installation Linux (with AMD GPU)](./download/installation)
- TODO ...

## Export geometry to Simuscle

You should have access to a human body musculo-skeleton included in Simuscle. 
A Blender file containg the geometry, muscles and bones is located at `Simuscle/Example/Human_Example.blend` 
You can open it to see how the musculo-skeleton have to be done. The constraint to export it to SImuscle is detailled on the
[Tutorial](./Tutorial/workflow) part.

If you had correctly installed the Blender add-on, you can export the musculo-skeleton to Simuscle by clicking on 
`File->Exoirt->Simuscle (dir)`. You can choose the name of your project

That will create a directory with all the geometries and informations that Simuscle need to compute the simulation.

## Open Simuscle

If Simuscle is correctly installed, you can open your project with the follwing command:
```bash
simuscle path/to/yout/project/dir
```
Now a window with the human musculo-skeleton should apear, and you should see the human geometry
