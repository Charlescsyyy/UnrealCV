from typing import Any, Dict

import numpy as np

from lychsim.utils import FACES

__all__ = ['AABB', 'OBB']


def get_corners(min_pt, max_pt):
    min_x, min_y, min_z = min_pt
    max_x, max_y, max_z = max_pt
    points = np.array([
        [min_x, min_y, min_z],
        [max_x, min_y, min_z],
        [max_x, max_y, min_z],
        [min_x, max_y, min_z],
        [min_x, min_y, max_z],
        [max_x, min_y, max_z],
        [max_x, max_y, max_z],
        [min_x, max_y, max_z],
    ])
    return points


class AABB:
    """LychSim Axis-Aligned Bounding Box (AABB).
    """

    def __init__(
        self, center: np.ndarray = None, extent: np.ndarray = None,
        translation: np.ndarray = None
    ):
        self.center = center
        self.extent = extent
        self.translation = translation

    def __repr__(self):
        return (
            f"AABB(center={tuple(self.center.tolist())}, "
            f"extent={tuple(self.extent.tolist())}), "
            f'translation={tuple(self.translation.tolist())})')

    @property
    def corners(self):
        """Returns the corners of the AABB.
        """
        corners = get_corners(
            self.center - self.extent,
            self.center + self.extent)
        return corners + self.translation

    def to_dict(self):
        """Returns a dictionary representation of the AABB.
        """
        return dict(
            center=self.center.tolist(),
            extent=self.extent.tolist(),
            translation=self.translation.tolist())

    @classmethod
    def from_dict(cls, data_dict: Dict[str, Any]):
        return cls(
            center=np.array(data_dict['center']),
            extent=np.array(data_dict['extent']),
            translation=np.array(data_dict['translation']))

    def __eq__(self, other):
        if not isinstance(other, AABB):
            return False
        return (np.array_equal(self.center, other.center) and
                np.array_equal(self.extent, other.extent) and
                np.array_equal(self.translation, other.translation))


class OBB:
    """LychSim Oriented Bounding Box (OBB).
    """

    def __init__(
        self, center: np.ndarray = None, extent: np.ndarray = None,
        rotation: np.ndarray = None, translation: np.ndarray = None
    ):
        self.center = center
        self.extent = extent
        self.rotation = rotation
        self.translation = translation

    def __repr__(self):
        return (
            f'OBB(center={tuple(self.center.tolist())}, '
            f'extent={tuple(self.extent.tolist())}, '
            f'rotation={self.rotation.tolist()}, '
            f'translation={tuple(self.translation.tolist())})')

    @property
    def corners(self):
        """Returns the corners of the OBB.
        """
        corners = get_corners(
            self.center - self.extent,
            self.center + self.extent)
        return (self.rotation @ corners.T).T + self.translation

    def to_dict(self):
        """Returns a dictionary representation of the AABB.
        """
        return dict(
            center=self.center.tolist(),
            extent=self.extent.tolist(),
            rotation=self.rotation.tolist(),
            translation=self.translation.tolist())

    @classmethod
    def from_dict(cls, data_dict: Dict[str, Any]):
        return cls(
            center=np.array(data_dict['center']),
            extent=np.array(data_dict['extent']),
            rotation=np.array(data_dict['rotation']),
            translation=np.array(data_dict['translation']))

    def __eq__(self, other):
        if not isinstance(other, OBB):
            return False
        return (np.array_equal(self.center, other.center) and
                np.array_equal(self.extent, other.extent) and
                np.array_equal(self.rotation, other.rotation) and
                np.array_equal(self.translation, other.translation))
