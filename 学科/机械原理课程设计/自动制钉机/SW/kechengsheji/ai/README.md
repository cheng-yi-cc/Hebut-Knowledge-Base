# Editable CAD assembly package

These files are intended for editing in AutoCAD or another mechanical CAD tool.
Use STEP files for modification. The STL export is only a mesh preview/export
format and is not the recommended editing source.

Recommended AutoCAD workflow:

1. Open AutoCAD.
2. Run `IMPORT`.
3. Select `automatic_nail_machine_editable_assembly.step` for the full assembly,
   or import the smaller subsystem STEP files one at a time.
4. Keep units in millimeters.
5. Edit imported 3D solids/subassemblies with AutoCAD solid editing tools.

Files:

- `automatic_nail_machine_editable_assembly.step`: full machine assembly.
- `base_frame_and_fasteners.step`: base plate, rails, supports, bolts.
- `motor_belt_main_shaft_transmission.step`: motor, belt, main shaft, bearings.
- `feed_straighten_and_length_control.step`: wire coil, straightening rollers, feed and ratchet mechanism.
- `cam_clamp_shear_and_point_extrusion.step`: clamp, cutter, cam, cold-point extrusion station.
- `cold_heading_crank_slider.step`: crank-slider cold heading station.
- `output_chute_and_sample_nails.step`: output chute and sample nails.

