# Circumstellar
# An interactive journey beyond the Event Horizon...

For now, this should build and run against any recent (>=0.9.0) version of Cinder. All previz and the main app are linked in Circumstellar.sln, so go crazy. 

Previz notes:
- BlackHoleGPU: failed attempt at dynamic buffers and transform feedback
- CoordConverTest: Mucking about with different methods for going from rect to polar and back. Math enthusiasts will probably note the somewhat ugly hack herein, but it works!
- DustPreviz: Attractors
- DustTest: Vizdev for the dustcloud
- SkipComponents: "Asymmetrical" buffers and interleaved attributes for transform feedback
- TFMultiAttr: Reacquainting myself with multi-attribute/buffer transform feedback- 

# And now...Pictures!
**"Accretion Disk" first pass 2016.01.08**
![Alt text](/content/doc/circumstellar_2016_01_08.png?raw=true "Optional Title")
**Attractor test first pass 2016.01.08** (see _tests/DustPreviz)
![Alt text](/content/doc/circumstellar_dust_test_2016_01_08.png?raw=true "Optional Title")
**Fun with attractors 2016.01.08** (see _tests/DustPreviz)
![Alt text](/content/doc/circumstellar_dust_test_2016_01_08_02.png?raw=true "Optional Title")
**"Accretion Disk" test perspective 2016.01.09**
![Alt text](/content/doc/circumstellar_2016_01_09.png?raw=true "Optional Title")
**Full composite test first pass 2016.01.16**
![Alt text](/content/doc/circumstellar_2016_01_16.png?raw=true "Optional Title")
**Multi attr transform feedback test 2016.01.18** (see _tests/TFMultiAttr)
![Alt text](/content/doc/multattr_tf_2016_01_18.png?raw=true "Optional Title")
**Transform feedback interleaved and skipped components test 2016.01.19**
![Alt text](/content/doc/testing_glsl_attr_mapping.png?raw=true "Optional Title")
**...A little prettier 2016.01.22** (see _tests/SkipComponents)
![Alt text](/content/doc/skip_components_2016_01_22.png?raw=true "Optional Title")
**Timed spawners and final mouse interaction 2016.01.26** (see master build)
![Alt text](/content/doc/timed_spawn_2016_01_26.png?raw=true "Optional Title")
**...and just to prove to Hai that I can in fact do interleaved transform feedback in Cinder...** (see ParticleSphereGPU Cinder sample...once i submit a PR)
![Alt text](/content/doc/for_hai.png?raw=true "Optional Title")
