# RasCull

Experimental software-rasterized occlusion culling plugin for Godot

This plugin uses a GDNative software rasterizer written in C to implement occlusion culling in the Godot engine.

Currently it's a proof-of-concept, and doesn't perform well enough for production usage:
- Visible object gathering is bottlenecked by GDScript
  - ~5ms in the default test scene (i7-3770k)
- The rasterizer itself uses the bresenham line algorithm, and is single-threaded without SIMD optimizations
  - ~5-8ms rasterize, ~5-8ms depth test at 256*128 in the default test scene (i7-3770k)
