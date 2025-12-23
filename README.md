# black++ole

Simulation d'un trou noir de Schwarzschild en C++ 100% CPU avec backward ray tracing .

## Physique

### Métrique de Schwarzschild

Un trou noir courbe l'espace-temps. Cette courbure est décrite par la métrique de Schwarzschild :

$$\mathrm{d}s^{2} = -\left(1-\frac{2M}{r}\right)\mathrm{d}t^{2}+ \left(1-\frac{2M}{r}\right)^{-1}\mathrm{d}r^{2}+ r^{2}\,\mathrm{d}\theta^{2}+ r^{2}\sin^{2}\theta\,\mathrm{d}\varphi^{2}$$

### Équations des géodésiques

Les photons suivent des géodésiques déterminées par les symboles de Christoffel :

$$\frac{d x^{\mu}}{d\lambda} = k^{\mu}$$
$$\frac{d k^{\mu}}{d\lambda}= - \Gamma^{\mu}_{\alpha\beta}k^{\alpha} k^{\beta}$$

La résolution numériques se fait via RK4.

## Ray tracing

Pour chaque pixel, on lance un photon **depuis la caméra** et on rewind sa trajectoire vers ses origines (étoiles lointaines ou trou noir).

1. **Caméra locale** : Construction d'un vecteur d'onde dans le référentiel local de la caméra
2. **Changement de base** : Projection dans le référentiel global via une tétrade orthonormale
3. **Intégration** : Rewind de la trajectoire du photon avec RK4 jusqu'à :
   - Dépassement de `r_max` → couleur du skybox
   - Franchissement de `r_s` (horizon) → pixel noir

## Compilation

```bash
cd build
cmake ..
make
./black++ole
```

Les frames sont générées dans `output/`.
Compiler en une video avec quelque chose comme

```bash
ffmpeg -framerate 24 -i output/frame_%03d.ppm -c:v libx264 -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" video.mp4
```


## Structure

- `include/` : Headers (vecteurs, photons, trou noir, image)
- `src/` : Implémentations
- `assets/` : Skybox PPM
- `docs/` : Documentation détaillée
