import numpy as np

from .colors import *
from .general import rgbd2rgb
from .io import *
from .model_output import *


FACES = np.array([
    [0, 3, 2, 1],
    [4, 5, 6, 7],
    [0, 1, 5, 4],
    [2, 3, 7, 6],
    [0, 4, 7, 3],
    [1, 2, 6, 5],
])
