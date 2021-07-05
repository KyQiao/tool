from scipy.special import sph_harm
import numpy as np

x = 1
y = 1
z = 1

rxy2 = x * x + y * y
rxy = np.sqrt(rxy2)
r2 = rxy2 + z * z
r = np.sqrt(r2)
sinTheta = rxy / r
cosTheta = z / r
sinPsi = x / rxy
cosPsi = y / rxy
print("Y6m:")
for i in range(-6, 7):
    print(sph_harm(i, 6, np.arccos(cosPsi), np.arccos(cosTheta)))

print("Y4m:")
for i in range(-4, 5):
    print(sph_harm(i, 4, np.arccos(cosPsi), np.arccos(cosTheta)))
